#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <iterator>
#include "Utils.hpp"

std::string join(const std::vector<std::string>& strings, std::string delim)
{
    if (strings.empty()) {
        return std::string();
    }

    return std::accumulate(strings.begin() + 1, strings.end(), strings[0],
        [&delim](std::string x, std::string y) {
            return x + delim + y;
        }
    );
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

int StringToInt(const std::string& str)
{
    int sum = 0;
    int iterate = 1;
    std::string text = str;

    while (text.size())
    {
        sum += (text[text.size() - 1] - '0') * iterate;
        iterate *= 10;

        text = text.substr(0, text.size() - 1);
    }

    return sum;
}