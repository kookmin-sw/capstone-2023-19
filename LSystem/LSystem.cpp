#include <string>
#include <vector>
#include <iostream>
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

bool IsOperator(char let)
{
    if (let == '+' || let == '-')
    {
        return true;
    }

    return false;
}

void AddWordToVector(std::vector<LLetter>* v, const std::string& word)
{
    for (const char& let : word)
    {
        if (IsOperator(let))
        {
            // +, -인 경우 이전의 추가한 letter 수정
            LLetter* target = &(v->back());
            target->set_letter(target->get_letter() + let);
        }
        else
        {
            LLetter letter = LLetter(let);
            v->push_back(letter);
        }
    }
}

void AddWordToVector(std::vector<LLetter>* v, const LLetter& letter)
{
    v->push_back(letter);
}

LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();
    wordText_ = "";
    rulesText_ = "";
}

LSystem::~LSystem()
{
    
}

// Get, Set
std::string LSystem::get_word() const
{
    return this->wordText_;
}

std::string LSystem::get_rules() const
{
    return this->rulesText_;
}

void LSystem::SetWord(const std::string& word)
{
    AddWordToVector(this->word_, word);
    this->wordText_ = word;
}

// Word
void LSystem::ClearWord()
{
    delete this->word_;
    this->word_ = nullptr;
    this->wordText_ = "";
}

// Rule
void LSystem::AddRule(const std::string& ruleText)
{
    this->rules_.push_back(LRule(ruleText));
    this->rulesText_ += ruleText + '\n';
}

void LSystem::AddRule(const std::vector<std::string>& rules)
{
    for (const std::string& rule : rules)
    {
        this->rules_.push_back(LRule(rule));
        this->rulesText_ += rule + '\n';
    }
}

void LSystem::AddRule(const std::string& key, const std::string& value)
{
    this->rules_.push_back(LRule(key, value));
    this->rulesText_ += key + "->" + value + '\n';
}

void LSystem::ClearRule()
{
    std::vector<LRule>().swap(this->rules_);
    this->rulesText_ = "";
}

// Run
void LSystem::Iterate()
{
    std::string newLetter;
    std::string wordText = "";
    bool changed;
    std::vector<LLetter> *v = new std::vector<LLetter>();

    for (const LLetter& letter : *(this->word_))
    {
        changed = false;

        // !!! map으로 수정하기
        for (const LRule& rule : this->rules_)
        {
            if (rule.get_before() == letter.get_letter())
            {
                newLetter = rule.get_after();
                AddWordToVector(v, newLetter);
                wordText += newLetter;
                changed = true;
                break;
            }
        }

        if (!changed)
        {
            AddWordToVector(v, letter);
            wordText += letter.get_letter();
        }
    }
    
    // 변환된 word로 수정
    this->ClearWord();
    this->word_ = v;
    this->wordText_ = wordText;
}

void LSystem::Iterate(int n)
{
    for (int i = 0; i < n; i++)
    {
        this->Iterate();
    }
}
