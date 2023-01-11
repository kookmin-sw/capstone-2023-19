#include <iostream>
#include <vector>
#include "LLetter.hpp"
#include "LRule.hpp"

LRule::LRule()
{

}

LRule::LRule(const std::string& text)
{
    int split = text.find("->");
    
    if (split < 0)
    {
        // !!! valid check 추가하기
        throw "Invalid rule format";
    }

    this->set_rule(text[0], text.substr(split + 2));
}

LRule::LRule(const char& key, const std::string& value)
{
    this->set_rule(key, value);
}

LRule::LRule(const std::string& keyStr, const std::string& value)
{
    this->set_rule(keyStr[0], value);
}

LRule::~LRule()
{

}

LLetter LRule::get_before() const
{
    return *(this->before_);
}

std::vector<LLetter> LRule::get_after() const
{
    return this->after_;
}

std::string LRule::get_rule() const
{
    std::string ruleText = "";
    ruleText += this->before_->get_letter();
    ruleText += " -> ";

    for (const LLetter& let : this->after_)
    {
        ruleText += let.get_letter();
    }

    return ruleText;
}

void LRule::set_rule(const char& key, const std::string& value)
{
    this->before_ = new LLetter(key);
    this->after_ = std::vector<LLetter>();
    for (const char& ch : value)
    {
        this->after_.push_back(LLetter(ch));
    }
}