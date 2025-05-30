using System.Collections.Specialized;
using System.Net.Http;
using System.Net.Http.Json;

namespace Downloader
{
    public class Program
    {
        private static readonly TimeSpan Timeout = TimeSpan.FromMinutes(5);
        private static readonly int MaxRetries = 3;
        private static readonly TimeSpan RetryDelay = TimeSpan.FromSeconds(5);
        private static readonly int DefaultConcurrent = 25;
        static string GenerateURL(string onlineId, string? filename = null)
        {
            var mirror = MirrorConfig.CurrentMirror;
            if (int.TryParse(onlineId, out int beatmapId))
            {
                return mirror.GetDownloadUrl(beatmapId, filename);
            }
            return $"{mirror.BaseUrl}{onlineId}";
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

        private static async Task<string?> GetBeatmapName(string onlineId)
        {
            try
            {
                using var client = new HttpClient();
                client.Timeout = TimeSpan.FromSeconds(10);
                var response = await client.GetFromJsonAsync<BeatmapInfo>($"https://api.chimu.moe/v1/map/{onlineId}");
                if (response != null)
                {
                    return $"{onlineId} {response.Artist} - {response.Title}";
                }
            }
            catch (Exception ex)
            {
                Logger.LogError($"获取谱面信息时发生错误: {ex.Message}");
            }
            return null;
        }

        private class BeatmapInfo
        {
            public string Artist { get; set; } = "";
            public string Title { get; set; } = "";
        }        public static async Task downloadBeatmap(string onlineId, string savePath, string? beatmapName = null)
        {
            var attempt = 0;
            Exception? lastException = null;
            
            while (attempt < MaxRetries)
            {
                attempt++;
                try 
                {
                    var url = GenerateURL(onlineId, onlineId);
                    Logger.LogInfo($"[{onlineId}] 尝试 {attempt}/{MaxRetries}: 正在从 {url} 下载谱面");
                    
                    using var client = new HttpClient();
                    client.Timeout = Timeout;
                    
                    using var response = await client.GetAsync(url, HttpCompletionOption.ResponseHeadersRead);
                    Logger.LogInfo($"[{onlineId}] 响应状态: {response.StatusCode}");

                    if (!response.IsSuccessStatusCode)
                    {
                        throw new HttpRequestException($"下载谱面失败: {response.StatusCode}");
                    }

                    var contentLength = response.Content.Headers.ContentLength;
                    Logger.LogInfo($"[{onlineId}] 文件大小: {(contentLength.HasValue ? $"{contentLength.Value / 1024 / 1024:F2}MB" : "未知")}");

                    // 使用临时文件下载
                    var tempFile = savePath + ".tmp";
                    try
                    {
                        await using var stream = await response.Content.ReadAsStreamAsync();
                        Logger.LogInfo($"[{onlineId}] 正在下载到临时文件: {tempFile}");
                        await CopyToFileWithProgressAsync(stream, tempFile, contentLength);
                        
                        Logger.LogInfo($"[{onlineId}] 正在验证下载的文件...");
                        await ValidateDownloadedFile(tempFile);
                        
                        // 下载和验证成功后，移动到目标位置
                        if (File.Exists(savePath))
                        {
                            File.Delete(savePath);
                        }
                        File.Move(tempFile, savePath);
                        
                        Logger.LogInfo($"[{onlineId}] 下载成功完成！");
                        return;
                    }
                    catch (Exception)
                    {
                        if (File.Exists(tempFile))
                        {
                            File.Delete(tempFile);
                        }
                        throw;
                    }
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
                if (args.Length < 2)
                {
                    Logger.LogError("参数无效");
                    Console.WriteLine("用法: ");
                    Console.WriteLine("下载谱面: beatmapDownloader <谱面ID> <保存路径> [并发数]");
                    Console.WriteLine("选择镜像: beatmapDownloader --mirror <镜像名称> [谱面ID] [保存路径] [并发数]");
                    Console.WriteLine("列出镜像: beatmapDownloader --list-mirrors");
                    Console.WriteLine("\n并发数默认为25，可选择1-50之间的值");
                    return;
                }

                // 解析并发数参数
                int concurrent = DefaultConcurrent;
                string[] processArgs = args;
                
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
                    processArgs = args.Skip(2).ToArray();
                }

                // 检查是否有并发数参数
                if (processArgs.Length >= 3)
                {
                    if (int.TryParse(processArgs[2], out int parsedConcurrent))
                    {
                        concurrent = Math.Max(1, Math.Min(50, parsedConcurrent));
                        if (concurrent != parsedConcurrent)
                        {
                            Logger.LogInfo($"并发数已调整到有效范围: {concurrent}");
                        }
                    }
                    else
                    {
                        Logger.LogInfo($"无效的并发数参数，使用默认值: {DefaultConcurrent}");
                    }
                }

                var onlineId = processArgs[0];
                var savePath = processArgs[1];

                // 如果保存路径是目录，则在目录下创建文件
                if (Directory.Exists(savePath) || savePath.EndsWith("/") || savePath.EndsWith("\\"))
                {
                    savePath = Path.Combine(savePath, "temp"); // 临时路径，会被下面的代码修改
                }
                
                // 确保保存路径的父目录存在
                var saveDir = Path.GetDirectoryName(savePath);
                if (!string.IsNullOrEmpty(saveDir) && !Directory.Exists(saveDir))
                {
                    Directory.CreateDirectory(saveDir);
                }

                // 创建下载管理器实例（使用指定的并发数）
                var downloadManager = new DownloadManager(concurrent);
                
                // 支持批量下载多个谱面ID
                var ids = onlineId.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                if (ids.Length > 1)
                {
                    Logger.LogInfo($"检测到多个谱面ID: {ids.Length}个");
                }

                foreach (var id in ids)
                {
                    var outputPath = Path.Combine(
                        Path.GetDirectoryName(savePath) ?? "",
                        $"{id}.osz"); // 强制使用.osz扩展名
                    
                    downloadManager.QueueDownload(id, outputPath);
                }

                Logger.LogInfo($"开始下载 {ids.Length} 个谱面 (并发数: {concurrent})");
                await downloadManager.ProcessQueueAsync();
                Logger.LogInfo("所有下载任务已完成");
            }
            catch (Exception ex)
            {
                Logger.LogError("发生严重错误", ex);
                Environment.Exit(1);
            }
        }
    }
}