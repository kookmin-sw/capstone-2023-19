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