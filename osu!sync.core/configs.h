#pragma once
#include<string>
#include<vector>
#include<map>
//本头文件定义了一些全局的参数和一些杂七杂八的东西

#define ISLAZER 1<<8
#define HASSONGSFILE 1<<7
#define USECONFIGFILE 1<<6
#define NEEDEXPORT 1<<5
#define NEEDUPLOAD 1<<4
#define NEEDDOWNLOAD 1<<3
#define HASUSERNAME 1<<2
#define OTHERLANGUAGE 1<<1
#define NEEDIMPORT 1<<0

typedef std::map<std::string, std::string> paramList_t;
std::map<std::string, int> paramMapper = {
	{"--lazer",ISLAZER},
	{"--songsFile",HASSONGSFILE},
	{"--config-file",USECONFIGFILE},
	{"--export",NEEDEXPORT},
	{"--upload",NEEDUPLOAD},
	{"--download",NEEDDOWNLOAD},
	{"--upload",NEEDUPLOAD},
	{"--username",HASUSERNAME},
	{"--language",OTHERLANGUAGE},
	{"--import",NEEDIMPORT}
};