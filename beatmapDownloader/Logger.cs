using System;

namespace Downloader
{
    public static class Logger
    {
        private static string LogFilePath => Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "downloader.log");

        public static void LogInfo(string message)
        {
            var logMessage = $"[INFO] {DateTime.Now:yyyy-MM-dd HH:mm:ss} - {message}";
            WriteLine(logMessage);
        }

        public static void LogError(string message, Exception? ex = null)
        {
            var logMessage = $"[ERROR] {DateTime.Now:yyyy-MM-dd HH:mm:ss} - {message}";
            if (ex != null)
            {
                logMessage += $"\nException: {ex.Message}\nStack Trace: {ex.StackTrace}";
            }
            WriteLine(logMessage);
        }

        private static void WriteLine(string message)
        {
            Console.WriteLine(message);
            try
            {
                File.AppendAllText(LogFilePath, message + Environment.NewLine);
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"Failed to write to log file: {ex.Message}");
            }
        }
    }
}
