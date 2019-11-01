/*
@note:
	读取配置文件, #后面的内容为注释,	支持[section]章节
@eg:
	#define CONFIG_NOTIFY_TEST (*CSingleton<CBitMapConfigFile>::instance())("notify", "test").c_str())
@config file:
	[notify] #notify section
	test=123
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

//#include "singleton.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

using namespace std;
namespace comm
{
	class CConfigDocument
	{
	public:
		CConfigDocument();
		CConfigDocument(const char* szFileName);
		virtual ~CConfigDocument();
	
	
	public:
		string& operator[](const char* szName);
		string& operator()(const char* szSection, const char* szName);
	
		int ParseFile(const char* szConfigFile);
	
	private:
		static int StrimString(char* szLine);
		int ParseFile();
	
	private:
		ifstream m_ConfigFile;
		map<string, string> m_ConfigMap;
	};
}
#endif

