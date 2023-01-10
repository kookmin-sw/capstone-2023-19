#include <iostream>
#include <queue>
#include "LRule.hpp"

LRule::LRule()
{

}

LRule::LRule(const std::string& text)
{
    int split = text.find("->");
    
    if (split < 0)
    {
        throw "Invalid rule format";
    }

    this->before_ = text.substr(0, split);
    this->after_ = text.substr(split + 2);
}

LRule::LRule(const std::string& key, const std::string& value) : before_(key), after_(value)
{
}

LRule::~LRule()
{

}

std::string LRule::get_before() const
{
    return this->before_;
}

std::string LRule::get_after() const
{
    return this->after_;
}