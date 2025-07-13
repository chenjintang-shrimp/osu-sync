#include<iostream>
#include"osuInstance.hpp"
#include "apiRequest.hpp"
using namespace std;

int main()
{
    apiRequest api;
    api.getToken();
    cout<<api.getBeatmapSetDetails("977").second.title<<endl;
    return 0;
}