#pragma once
#include <iostream>
#include <string>
#include <regex>
#include <Shlwapi.h>
#include <WTypesbase.h>

#pragma comment(lib, "shlwapi.lib")
class Utility
{
public:
	std::string getScheme(LPCSTR url);
	std::string getHostName(LPCSTR url);
	std::string getPath(LPCSTR url);
	LPCSTR getFileName(LPCSTR path);
};

