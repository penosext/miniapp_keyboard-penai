#include "strUtils.hpp"
#include <random>
#include <sstream>

std::string strUtils::trim(const std::string &str) { return trimStart(trimEnd(str)); }
std::string strUtils::trimEnd(const std::string &str)
{
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}
std::string strUtils::trimStart(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string strUtils::randomId()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; i++)
        ss << dis(gen);
    return ss.str();
}

std::vector<std::string> strUtils::split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> result;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end > start)
            result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    if (start < str.length())
        result.push_back(str.substr(start));
    return result;
}

std::string strUtils::join(const std::vector<std::string> &vec, const std::string &delimiter)
{
    if (vec.empty())
        return "";
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0)
            result += delimiter;
        result += vec[i];
    }
    return result;
}
