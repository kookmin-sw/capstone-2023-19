#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <cmath>
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

// Public
LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();

    this->state_ = { {0.0, 0.0, 0.0}, 0.0 };
}

LSystem::~LSystem()
{
    delete this->word_;
    this->word_ = nullptr;
}

// Get, Set
std::string LSystem::GetWord() const
{
    std::string wordText;
    for (const LLetter& letter : *(this->word_))
    {
        wordText += letter.GetLetter();
    }

    return wordText;
}

std::string LSystem::GetRules() const
{
    std::string rulesText;
    for (const LRule& rule : this->rules_)
    {
        rulesText += rule.GetRule() + '\n';
    }
    return rulesText;
}

float LSystem::GetAngleChange() const
{
    return this->angleChange_;
}

float LSystem::GetDistance() const
{
    return this->distance_;
}

void LSystem::SetAngleChange(const float& val)
{
    this->angleChange_ = val;
}

void LSystem::SetDistance(const float& val)
{
    this->distance_ = val;
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
            if (rule.GetBefore().IsEqual(letter.GetLetter()))
            {
                std::vector<LLetter> letters = rule.GetAfter();
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


void LSystem::GetResultVertex(std::vector<State>* out)
{
    if (this->word_->size() < 1)
    {
        throw "No word";
    }

    std::stack<LSystem::State> ss;

    out->push_back(this->state_);     // start state
    for (const LLetter& letter : *(this->word_))
    {
        switch (letter.GetType())
        {
            case LLetter::Type::Forward:
                this->Move();
                out->push_back(this->state_);
                break;
            case LLetter::Type::Left:
                this->Turn();
                break;
            case LLetter::Type::Right:
                this->Turn(false);
                break;
            case LLetter::Type::Push:
                ss.push(this->state_);
                break;
            case LLetter::Type::Pop:
                // !!! State에 타입 추가하기 (ex. Skip)
                // Draw 없는 이동
                this->state_ = ss.top();
                ss.pop();
                out->push_back(this->state_);
                break;
            case LLetter::Type::None:
                break;
        }
    }
}

// Private
void LSystem::Move()
{
    // 현재 위치에서 y-up vector를 기준으로 얼마나 회전된 상태인지
    float rad = this->state_.angle / 180.0 * pi;

    this->state_.position.x += std::sin(rad) * this->distance_;
    this->state_.position.y += std::cos(rad) * this->distance_;
}

void LSystem::Turn(const bool& isLeft)
{
    if (isLeft)
    {
        this->state_.angle -= this->angleChange_;
    }
    else
    {
        this->state_.angle += this->angleChange_;
    }
}

// void LSystem::SetPosition(const Vector3&)
// {

// }