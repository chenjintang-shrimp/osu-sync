#include<iostream>
#include"osuInstance.hpp"
using namespace std;

int main()
{
    fs::path osuFolderPath;
    cin>>osuFolderPath;
    osuInstance osu(osuFolderPath);
    vector<string> bslist=osu.getAllBeatmapSets(folder);
    for(auto kv:bslist)
        cout<<kv<<endl;
    return 0;
}