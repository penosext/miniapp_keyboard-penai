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
