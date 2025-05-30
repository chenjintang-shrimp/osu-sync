using System.Collections.Concurrent;

namespace Downloader
{
    public class DownloadManager
    {
        private readonly ConcurrentQueue<DownloadItem> downloadQueue = new();
        private readonly SemaphoreSlim concurrencyLimiter;
        private readonly int maxConcurrentDownloads;
        private readonly HttpClient httpClient;
        private int activeDownloads;
        private int completedDownloads;
        private readonly object progressLock = new();
        
        public DownloadManager(int maxConcurrent = 25)
        {
            maxConcurrentDownloads = Math.Max(1, Math.Min(50, maxConcurrent));
            concurrencyLimiter = new SemaphoreSlim(maxConcurrentDownloads);
            httpClient = new HttpClient
            {
                Timeout = TimeSpan.FromMinutes(5)
            };
        }

        public void QueueDownload(string onlineId, string savePath)
        {
            downloadQueue.Enqueue(new DownloadItem(onlineId, savePath));
            Logger.LogInfo($"已将谱面 {onlineId} 加入下载队列");
        }

        public async Task ProcessQueueAsync(CancellationToken cancellationToken = default)
        {
            var allTasks = new List<Task>();
            var downloadTasks = new HashSet<Task>();
            activeDownloads = 0;
            completedDownloads = 0;
            var totalDownloads = downloadQueue.Count;
            
            async Task StartNewDownloadAsync()
            {
                if (downloadQueue.TryDequeue(out var item))
                {
                    await concurrencyLimiter.WaitAsync(cancellationToken);
                    Interlocked.Increment(ref activeDownloads);
                    
                    var task = ProcessDownloadItemAsync(item, cancellationToken)
                        .ContinueWith(t =>
                        {
                            concurrencyLimiter.Release();
                            Interlocked.Decrement(ref activeDownloads);
                            Interlocked.Increment(ref completedDownloads);
                            
                            lock (progressLock)
                            {
                                var progress = (double)completedDownloads / totalDownloads;
                                Logger.LogInfo($"整体进度: {progress:P2} ({completedDownloads}/{totalDownloads}) " +
                                            $"[活动下载: {activeDownloads}]");
                            }
                        }, TaskContinuationOptions.ExecuteSynchronously);
                    
                    downloadTasks.Add(task);
                    allTasks.Add(task);
                }
            }
            
            // 启动初始下载任务
            for (int i = 0; i < maxConcurrentDownloads && !downloadQueue.IsEmpty; i++)
            {
                await StartNewDownloadAsync();
            }
            
            // 持续监控并启动新的下载任务
            while (downloadTasks.Count > 0)
            {
                var completed = await Task.WhenAny(downloadTasks);
                downloadTasks.Remove(completed);
                
                if (!downloadQueue.IsEmpty && !cancellationToken.IsCancellationRequested)
                {
                    await StartNewDownloadAsync();
                }
            }

            // 等待所有下载完成
            await Task.WhenAll(allTasks);
            
            // 显示最终统计
            Logger.LogInfo($"\n下载统计:");
            Logger.LogInfo($"总计: {totalDownloads} 个谱面");
            Logger.LogInfo($"完成: {completedDownloads} 个谱面");
            var failedCount = totalDownloads - completedDownloads;
            if (failedCount > 0)
            {
                Logger.LogInfo($"失败: {failedCount} 个谱面");
            }
        }

        private async Task ProcessDownloadItemAsync(DownloadItem item, CancellationToken cancellationToken)
        {
            try
            {
                await Program.downloadBeatmap(item.OnlineId, item.SavePath);
            }
            catch (Exception ex)
            {
                Logger.LogError($"处理下载项时发生错误: {item.OnlineId}", ex);
            }
        }
    }

    public record DownloadItem(string OnlineId, string SavePath);
}
