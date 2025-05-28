using System.Collections.Concurrent;

namespace Downloader
{
    public class DownloadManager
    {
        private readonly ConcurrentQueue<DownloadItem> downloadQueue = new();
        private readonly SemaphoreSlim concurrencyLimiter;
        private readonly int maxConcurrentDownloads;
        
        public DownloadManager(int maxConcurrent = 3)
        {
            maxConcurrentDownloads = maxConcurrent;
            concurrencyLimiter = new SemaphoreSlim(maxConcurrent);
        }

        public void QueueDownload(string onlineId, string savePath)
        {
            downloadQueue.Enqueue(new DownloadItem(onlineId, savePath));
            Logger.LogInfo($"已将谱面 {onlineId} 加入下载队列");
        }

        public async Task ProcessQueueAsync(CancellationToken cancellationToken = default)
        {
            var tasks = new List<Task>();
            
            while (!downloadQueue.IsEmpty)
            {
                if (cancellationToken.IsCancellationRequested)
                {
                    Logger.LogInfo("收到取消请求，正在停止下载队列处理");
                    break;
                }

                if (downloadQueue.TryDequeue(out var item))
                {
                    await concurrencyLimiter.WaitAsync(cancellationToken);
                    
                    var task = ProcessDownloadItemAsync(item, cancellationToken)
                        .ContinueWith(_ => concurrencyLimiter.Release(), TaskContinuationOptions.ExecuteSynchronously);
                    
                    tasks.Add(task);

                    // 如果已达到最大并发数，等待任意一个任务完成
                    if (tasks.Count >= maxConcurrentDownloads)
                    {
                        await Task.WhenAny(tasks);
                        tasks.RemoveAll(t => t.IsCompleted);
                    }
                }
            }

            // 等待所有剩余的下载完成
            await Task.WhenAll(tasks);
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
