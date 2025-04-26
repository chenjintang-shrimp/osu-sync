using System.Collections.Specialized;
using System.Net.Http;

namespace Downloader
{
    public class Program
    {
        static string GenerateURL(string onlineId)
        {
            string generatedURL="https://b10.sayobot.cn:25225/beatmaps/";
            if(onlineId.Length <= 4)
            {
                generatedURL = generatedURL + "0/" + onlineId;
            }
            else
            {
                generatedURL = generatedURL + onlineId.Substring(0,onlineId.Length-4)+"/"+onlineId.Substring(onlineId.Length-4);
            }
            generatedURL = generatedURL + "/full?filename=" + onlineId;
            return generatedURL;
        }
        public static async void downloadBeatmap(string onlineId,string savePath)
        {
            using var cilent = new HttpClient();
            var url = GenerateURL(onlineId);
            var response = await cilent.GetAsync(url);
            await using var stream = await response.Content.ReadAsStreamAsync();
            System.Console.WriteLine(response.StatusCode.ToString());
            await using var fileStream = File.Create(savePath);
            await stream.CopyToAsync(fileStream);
        }
        public static void Main(string[] args)
        {
            var onlineId = args[0];
            var savePath=args[1];
            downloadBeatmap(onlineId,savePath);
        }
    }
}