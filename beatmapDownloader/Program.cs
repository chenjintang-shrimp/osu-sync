using System.Collections.Specialized;
using System.Net.Http;

namespace Downloader
{
    public class Program
    {
        private static readonly TimeSpan Timeout = TimeSpan.FromMinutes(5);
        private static readonly int MaxRetries = 3;
        private static readonly TimeSpan RetryDelay = TimeSpan.FromSeconds(5);
        private static readonly DownloadManager downloadManager = new(3);

        static string GenerateURL(string onlineId)
        {
            var mirror = MirrorConfig.CurrentMirror;
            var url = $"{mirror.BaseUrl}{onlineId}";
            if (mirror.requiresNoskip)
            {
                url += "?noskip=1";
            }
            return url;
        }

        private static async Task ValidateDownloadedFile(string filePath)
        {
            if (!File.Exists(filePath))
            {
                throw new FileNotFoundException("下载的文件未找到", filePath);
            }

            using var fs = new FileStream(filePath, FileMode.Open, FileAccess.Read);
            if (fs.Length == 0)
            {
                throw new InvalidDataException("下载的文件为空");
            }

            // 验证文件是否为有效的.osz文件（osu!谱面文件）
            byte[] header = new byte[4];
            await fs.ReadAsync(header, 0, 4);
            if (header[0] != 0x50 || header[1] != 0x4B) // 检查ZIP文件头
            {
                throw new InvalidDataException("下载的文件不是有效的谱面文件");
            }
        }
        
        private static async Task CopyToFileWithProgressAsync(Stream source, string destination, long? totalBytes = null)
        {
            const int bufferSize = 81920; // 80 KB buffer
            byte[] buffer = new byte[bufferSize];
            long totalBytesRead = 0;

            // 检查是否存在未完成的下载
            if (File.Exists(destination))
            {
                var fileInfo = new FileInfo(destination);
                if (totalBytes.HasValue && fileInfo.Length < totalBytes.Value)
                {
                    totalBytesRead = fileInfo.Length;
                    Logger.LogInfo($"发现未完成的下载，从 {totalBytesRead / 1024 / 1024:F2}MB 处继续");
                    source.Seek(totalBytesRead, SeekOrigin.Begin);
                }
            }
            
            using var destinationStream = new FileStream(destination, 
                                                       totalBytesRead > 0 ? FileMode.Append : FileMode.Create, 
                                                       FileAccess.Write, 
                                                       FileShare.None, 
                                                       bufferSize);
            
            int bytesRead;
            while ((bytesRead = await source.ReadAsync(buffer)) != 0)
            {
                await destinationStream.WriteAsync(buffer.AsMemory(0, bytesRead));
                totalBytesRead += bytesRead;
                
                if (totalBytes.HasValue)
                {
                    var progress = (double)totalBytesRead / totalBytes.Value;
                    Logger.LogInfo($"下载进度: {progress:P2}");
                }
                else
                {
                    Logger.LogInfo($"已下载: {totalBytesRead / 1024 / 1024:F2} MB");
                }
            }
        }

        public static async Task downloadBeatmap(string onlineId, string savePath)
        {
            var attempt = 0;
            while (attempt < MaxRetries)
            {
                attempt++;
                try 
                {
                    using var client = new HttpClient();
                    client.Timeout = Timeout;
                    var url = GenerateURL(onlineId);
                    Logger.LogInfo($"尝试 {attempt}/{MaxRetries}: 正在从 {url} 下载谱面 {onlineId}");
                    
                    var response = await client.GetAsync(url);
                    Logger.LogInfo($"响应状态: {response.StatusCode}");

                    if (!response.IsSuccessStatusCode)
                    {
                        throw new HttpRequestException($"下载谱面失败: {response.StatusCode}");
                    }

                    await using var stream = await response.Content.ReadAsStreamAsync();
                    var contentLength = response.Content.Headers.ContentLength;

                    Logger.LogInfo($"正在保存谱面到 {savePath}");
                    await CopyToFileWithProgressAsync(stream, savePath, contentLength);
                    
                    Logger.LogInfo("正在验证下载的文件...");
                    await ValidateDownloadedFile(savePath);
                    
                    Logger.LogInfo("下载成功完成！");
                    return;
                }
                catch (Exception ex)
                {
                    Logger.LogError($"下载谱面时发生错误 (尝试 {attempt}/{MaxRetries})", ex);
                    
                    if (attempt < MaxRetries)
                    {
                        Logger.LogInfo($"{RetryDelay.TotalSeconds} 秒后重试...");
                        await Task.Delay(RetryDelay);
                    }
                    else
                    {
                        Logger.LogError("已达到最大重试次数。下载失败。");
                        throw;
                    }
                }
            }
        }

        public static async Task Main(string[] args)
        {
            try
            {
                if (args.Length < 2 || (args[0] == "--mirror" && args.Length < 3))
                {
                    Logger.LogError("参数无效");
                    Console.WriteLine("用法: ");
                    Console.WriteLine("下载谱面: beatmapDownloader <谱面ID> <保存路径>");
                    Console.WriteLine("选择镜像: beatmapDownloader --mirror <镜像名称> [谱面ID] [保存路径]");
                    Console.WriteLine("列出镜像: beatmapDownloader --list-mirrors");
                    return;
                }

                // 处理命令行参数
                if (args[0] == "--list-mirrors")
                {
                    MirrorConfig.ListAvailableMirrors();
                    return;
                }
                
                if (args[0] == "--mirror")
                {
                    if (!MirrorConfig.SetMirror(args[1]))
                    {
                        Logger.LogError($"未知的镜像站: {args[1]}");
                        MirrorConfig.ListAvailableMirrors();
                        return;
                    }
                    
                    if (args.Length < 4)
                    {
                        return; // 只是设置镜像站，不下载
                    }
                    
                    // 移除镜像参数，继续处理下载
                    args = args.Skip(2).ToArray();
                }

                var onlineId = args[0];
                var savePath = args[1];
                
                // 支持批量下载多个谱面ID
                var ids = onlineId.Split(',', StringSplitOptions.RemoveEmptyEntries);
                foreach (var id in ids)
                {
                    downloadManager.QueueDownload(id.Trim(), Path.Combine(
                        Path.GetDirectoryName(savePath) ?? "",
                        $"{id.Trim()}{Path.GetExtension(savePath)}"));
                }

                Logger.LogInfo($"开始下载 {ids.Length} 个谱面");
                await downloadManager.ProcessQueueAsync();
            }
            catch (Exception ex)
            {
                Logger.LogError("发生严重错误", ex);
                Environment.Exit(1);
            }
        }
    }
}