namespace Downloader
{
    public class MirrorConfig
    {
        public static readonly Dictionary<string, Mirror> AvailableMirrors = new()
        {
            {
                "sayobot",
                new Mirror(
                    "Sayobot",
                    "https://txy1.sayobot.cn/download/beatmap/",
                    requiresNoskip: false
                )
            },
            {
                "catboy",
                new Mirror(
                    "Catboy",
                    "https://catboy.best/d/",
                    requiresNoskip: false
                )
            },
            {
                "chimu",
                new Mirror(
                    "Chimu",
                    "https://api.chimu.moe/v1/download/",
                    requiresNoskip: false
                )
            },
            {
                "nerinyan",
                new Mirror(
                    "Nerinyan",
                    "https://api.nerinyan.moe/d/",
                    requiresNoskip: true
                )
            }
        };

        private static string currentMirrorKey = "sayobot";

        public static Mirror CurrentMirror
        {
            get => AvailableMirrors[currentMirrorKey];
            private set => currentMirrorKey = AvailableMirrors.First(x => x.Value == value).Key;
        }

        public static bool SetMirror(string mirrorKey)
        {
            mirrorKey = mirrorKey.ToLower();
            if (AvailableMirrors.ContainsKey(mirrorKey))
            {
                currentMirrorKey = mirrorKey;
                Logger.LogInfo($"已切换到镜像站: {AvailableMirrors[mirrorKey].Name}");
                return true;
            }
            return false;
        }

        public static void ListAvailableMirrors()
        {
            Console.WriteLine("\n可用的镜像站:");
            foreach (var mirror in AvailableMirrors)
            {
                Console.WriteLine($"- {mirror.Key}: {mirror.Value.Name} ({mirror.Value.BaseUrl})");
            }
        }
    }

    public record Mirror(string Name, string BaseUrl, bool requiresNoskip);
}
