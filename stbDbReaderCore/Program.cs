//思路：
/*
 * 需要的只是快速列出所有bsid，那就枚举所有谱面的bsid然后去重，输出一行一个
 * 调用方式：
 * stbDbReaderCore --list [osudbpath]
 * 如果需要一个谱面集的详细信息，就直接找到它输出，一行一个属性
 * stdDbReaderCore --details [bid] [osudbpath]
 */
using System.Collections;
using System.Collections.Specialized;
using System.Numerics;
using System.Reflection.Metadata.Ecma335;
using osu_database_reader.BinaryFiles;

namespace Program
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Count<string>() < 2)
                return;
            if (args[0]=="--list")
            {
                var db = OsuDb.Read(args[1]);
                ArrayList allBsids=new ArrayList();
                ArrayList tmpBsids=new ArrayList();
                foreach(var beatmap in db.Beatmaps)
                    tmpBsids.Add(beatmap.BeatmapSetId.ToString());
                foreach(var bsid in tmpBsids)
                {
                    bool flag = false;
                    foreach(var tmp in allBsids)
                        if(tmp.ToString() == bsid.ToString())
                        {
                            flag = true;
                            break;
                        }
                    if (!flag)
                        allBsids.Add(bsid.ToString());
                }
                foreach (var bsid in allBsids)
                    Console.WriteLine(bsid);
            }
            else if (args[0]=="--details")
            {
                var bid = args[1];
                var db = OsuDb.Read(args[2]);
                foreach (var beatmap in db.Beatmaps)
                {
                    if (beatmap.BeatmapSetId.ToString() == bid.ToString())
                    {
                        Console.WriteLine(beatmap.Title);
                        Console.WriteLine(beatmap.TitleUnicode);
                        Console.WriteLine(beatmap.Artist);
                    }
                }
            }
        }
    }
}