#include <sstream>
#include <string>
#include <vector>
#include "Utils.hpp"

std::string join(const std::vector<std::string>& strings, std::string delim)
{
    std::stringstream ss;
    std::copy(strings.begin(), strings.end(),
        std::ostream_iterator<std::string>(ss, delim.c_str()));
    return ss.str();
}

std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string buffer;

    while (getline(ss, buffer, delimiter))
    {
        result.push_back(buffer);
    }

    return result;
}