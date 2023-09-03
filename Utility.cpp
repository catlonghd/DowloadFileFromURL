#include "Utility.h"


std::string Utility::getScheme(LPCSTR url)
{
    std::regex url_regex(R"(^(https?)://([^/]+)(/.*)?$)");
    std::string url_str(url);
    std::smatch url_match;
    if (std::regex_search(url_str, url_match, url_regex))
    {
        std::string host = url_match[1];
        return host;
    }
    return "";
}


std::string Utility::getHostName(LPCSTR url)
{
    std::regex url_regex(R"(^(https?)://([^/]+)(/.*)?$)");
    std::string url_str(url);
    std::smatch url_match;
    if (std::regex_search(url_str, url_match, url_regex))
    {
        std::string host = url_match[2];
        return host;
    }
    return "";
}

std::string Utility::getPath(LPCSTR url)
{
    std::regex url_regex(R"(^(https?)://([^/]+)(/.*)?$)");
    std::string url_str(url);
    std::smatch url_match;
    if (std::regex_search(url_str, url_match, url_regex)) 
    {
        std::string path = url_match[3];
        return path;
    }
    return "";
}

LPCSTR Utility::getFileName(LPCSTR path)
{
    return PathFindFileNameA(path);
}