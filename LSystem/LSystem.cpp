#include <string>
#include <vector>
#include <iostream>
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();
}

LSystem::~LSystem()
{
    delete this->word_;
    this->word_ = nullptr;
}

// Get, Set
std::string LSystem::get_word() const
{
    std::string wordText;
    for (const LLetter& letter : *(this->word_))
    {
        wordText += letter.get_letter();
    }

    return wordText;
}

std::string LSystem::get_rules() const
{
    std::string rulesText;
    for (const LRule& rule : this->rules_)
    {
        rulesText += rule.get_rule() + '\n';
    }
    return rulesText;
}

void LSystem::SetWord(const std::string& word)
{
    delete this->word_;
    this->word_ = new std::vector<LLetter>();

    for (const char& letter : word)
    {
        this->word_->push_back(LLetter(letter));
    }
}

// Rule
void LSystem::AddRule(const std::string& ruleText)
{
    this->rules_.push_back(LRule(ruleText));
}

void LSystem::AddRule(const char& key, const std::string& value)
{
    this->rules_.push_back(LRule(key, value));
}

void LSystem::AddRule(const std::string& key, const std::string& value)
{
    this->rules_.push_back(LRule(key, value));
}

// Run
void LSystem::Iterate()
{
    bool changed;
    std::vector<LLetter> *v = new std::vector<LLetter>();

    for (const LLetter& letter : *(this->word_))
    {
        changed = false;

        for (const LRule& rule : this->rules_)
        {
            if (rule.get_before().IsEqual(letter.get_letter()))
            {
                std::vector<LLetter> letters = rule.get_after();
                // #1
                // for (const LLetter& letter : letters)
                // {
                //     v->push_back(letter);
                // }
                
                // #2
                v->insert(v->end(), letters.begin(), letters.end());    

                changed = true;
                break;
            }
        }

        if (!changed)
        {
            v->push_back(letter);
        }
    }
    
    this->word_ = v;
}

void LSystem::Iterate(int n)
{
    for (int i = 0; i < n; i++)
    {
        this->Iterate();
    }
}
