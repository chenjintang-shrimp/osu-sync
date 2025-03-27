// See https://aka.ms/new-console-template for more information
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using osu.Shared;
using osu_database_reader;
using System.Text.Json.Serialization;
using osu_database_reader.BinaryFiles;
using System.Collections.Specialized;

public class beatmapSet
{
    public string beatMapSetName;
    public string beatMapSetId;
    public beatmapSet(string name,string id)
    {
        beatMapSetName = name;
        beatMapSetId = id;
    }
}
class Program
{
    static void Main(string[] args)
    {
        string stableDbFilePath = Console.ReadLine();
        string exportJsonPat = Console.ReadLine();
        if (string.IsNullOrEmpty(stableDbFilePath))
        {
            Console.WriteLine("Invalid file path.");
            return;
        }
        var db = OsuDb.Read(stableDbFilePath);
        
        System.Console.WriteLine("osu version:"+db.OsuVersion);
        System.Console.WriteLine("account name:" + db.AccountName);
        System.Console.WriteLine("Total beatmaps:"+db.Beatmaps.Count);

        int[] beatmapIds=new int[db.Beatmaps.Count];
        int beatMapindex = 1;

        foreach(var beatmap in db.Beatmaps)
        {
            if(beatMapindex != 0&&beatmap.BeatmapSetId!=beatmapIds[beatMapindex-1])
                beatmapIds[beatMapindex++]=beatmap.BeatmapSetId;
        }
        beatmapIds=beatmapIds.Distinct<int>().ToArray();
        
    }
}