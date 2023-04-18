#include <vector>
#include <string>
#include "RandomSeed.hpp"
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
    int total = this->after_.size();
    int index = dist(gen) % total;

    return this->after_[index];
}

std::string LRule::GetRule() const
{
    std::string ruleText = "";
    ruleText += this->before_->GetLetter();
    ruleText += " -> ";

    int total = this->after_.size();
    int index = dist(gen) % total;

    for (const LLetter& let : this->after_[index])
    {
        ruleText += let.GetLetter();
    }

    return ruleText;
}

void LRule::GetKey(char* out)
{
    for (int i = 0; i < this->key_.size(); i++)
    {
        out[i] = this->key_[i];
    }
}

std::string LRule::GetKeyString() const
{
    return this->key_;
}

void LRule::GetValue(char* out, const int& index)
{
    for (int i = 0; i < this->values_[index].size(); i++)
    {
        out[i] = this->values_[index][i];
    }
}

std::string LRule::GetValueString(const int& index) const
{
    return this->values_[index];
}

int LRule::GetRuleCount() const
{
    return this->values_.size();
}

void LRule::SetRule(const char& key, const std::string& value)
{
    this->before_ = new LLetter(key);
    this->key_ = key;

    auto tempLetter = std::vector<LLetter>();
    std::string tempString = "";

    for (const char& ch : value)
    {
        tempLetter.push_back(LLetter(ch));
        tempString += ch;
    }

    this->after_.push_back(tempLetter);
    this->values_.push_back(tempString);
}

void LRule::DeleteAfter(const std::string& value)
{
    // !!! to be update
}

