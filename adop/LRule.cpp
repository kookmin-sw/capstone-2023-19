#include <vector>
#include <string>
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

    this->SetRule(text[0], text.substr(split + 2));
}

LRule::LRule(const char& key, const std::string& value)
{
    this->SetRule(key, value);
}

LRule::LRule(const std::string& keyStr, const std::string& value)
{
    this->SetRule(keyStr[0], value);
}

LRule::~LRule()
{

}

LLetter LRule::GetBefore() const
{
    return *(this->before_);
}

std::vector<LLetter> LRule::GetAfter() const
{
    return this->after_;
}

std::string LRule::GetRule() const
{
    std::string ruleText = "";
    ruleText += this->before_->GetLetter();
    ruleText += " -> ";

    for (const LLetter& let : this->after_)
    {
        ruleText += let.GetLetter();
    }

    return ruleText;
}

void LRule::SetRule(const char& key, const std::string& value)
{
    this->before_ = new LLetter(key);
    this->after_ = std::vector<LLetter>();
    for (const char& ch : value)
    {
        this->after_.push_back(LLetter(ch));
    }
}