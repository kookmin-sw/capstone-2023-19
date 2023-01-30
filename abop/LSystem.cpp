#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

#pragma region LSystemUtils
// moveVector를 wantLength의 길이로 normalize
void Normalized(Vector3& moveVector, float wantLength)
{
    float vectorLen = hypot(moveVector.x, moveVector.y);

    // for only line
    moveVector.x = moveVector.x / vectorLen * wantLength;
    moveVector.y = moveVector.y / vectorLen * wantLength;
}

Vector3 AddVector(const Vector3& a, const Vector3& b)
{
    return Vector3
    {
        a.x + b.x,
        a.y + b.y,
        a.z + a.z
    };
}

// TEMP
Model CreateLineModel(const Vector3& start, const Vector3& end)
{
    // xy 평면의 line
    
    float halfWidth = 0.2;
    // !!! color 일단 블랙 고정
    Vector4 black { 0, 0, 0, 0 };
    Model model;

    // 이동 벡터
    Vector3 move { end.x - start.x, end.y - start.y, 0 };
    // 수직 벡터
    Vector3 left { -move.y, move.x, 0 };
    Vector3 right { move.y, -move.x, 0 };
    // 수직 벡터의 길이를 half width 만큼
    Normalized(left, halfWidth);
    Normalized(right, halfWidth);
    
    model.vertexCount = 4;
    model.vertexTypes[0] = VertexType { AddVector(end, left), black };
    model.vertexTypes[1] = VertexType { AddVector(end, right),  black };
    model.vertexTypes[2] = VertexType { AddVector(start, right), black };
    model.vertexTypes[3] = VertexType { AddVector(start, left), black };

    return model;
}

#pragma endregion LSystemUtils

// Public
LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();

    this->state_ = 
    { 
        {0.0f, 0.0f, 0.0f}, 
        {0.0f, 1.0f, 0.0f}
        // , {0.0f, 0.0f, -1.0f}
    };
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


void LSystem::GetResultVertex(std::vector<Model>* out)
{
    // out -> jk,
    if (this->word_->size() < 1)
    {
        throw "No word";
    }

    float width = 0.5;
    std::stack<State> ss;

    Vector3 startPos;
    Vector3 endPos;

    startPos = this->state_.position;
    for (const LLetter& letter : *(this->word_))
    {
        // Move: 이동 후 현재 위치 end에 저장 후 push
        switch (letter.GetType())
        {
            case LLetter::Type::Forward:
            case LLetter::Type::Forward2:
            {
                // Draw + Move forward
                this->Move();
                endPos = this->state_.position;
                out->push_back(CreateLineModel(startPos, endPos));
                startPos = this->state_.position;
                break;
            }
            case LLetter::Type::NoDrawForward:
            {
                // No Draw + Move foward
                this->Move();
                endPos = this->state_.position;
                out->push_back(CreateLineModel(startPos, endPos));
                break;
            }
            case LLetter::Type::RollLeft:
            {
                // angleChange_ 만큼 y축 회전
                this->Rotate(1, angleChange_);
                break;
            }
            case LLetter::Type::RollRight:
            {
                // -angleChange_ 만큼 y축 회전
                this->Rotate(1, -angleChange_);
                break;
            }
            case LLetter::Type::PitchUp:
            {
                // angleChange_ 만큼 x축 회전
                this->Rotate(0, angleChange_);
                break;
            }
            case LLetter::Type::PitchDown:
            {
                // -angleChange_ 만큼 x축 회전
                this->Rotate(0, -angleChange_);
                break;
            }
            case LLetter::Type::TurnLeft:
            {
                // angleChange_ 만큼 z축 회전
                this->Rotate(2, angleChange_);
                break;
            }
            case LLetter::Type::TurnRight:
            {
                // -angleChange_ 만큼 z축 회전
                this->Rotate(2, -angleChange_);
                break;
            }
            case LLetter::Type::TurnAround:
            {
                // 180도 z축 회전
                this->Rotate(2, 180.0f);
                break;
            }
            case LLetter::Type::Push:
            {
                // 현재 State 저장
                ss.push(this->state_);
                break;
            }
            case LLetter::Type::Pop:
            {
                // 이전 State 복원
                // 위치도 같이 옮겨지는 경우 No draw
                this->state_ = ss.top();
                ss.pop();
                startPos = this->state_.position;
                break;
            }
            case LLetter::Type::None:
            {
                break;
            }
        }
    }
}

// Private
void LSystem::Move()
{
    // Heading Vector에 distance 곱해서 움직여주기
    this->state_.position.x += this->state_.heading.x * this->distance_;
    this->state_.position.y += this->state_.heading.y * this->distance_;
    this->state_.position.z += this->state_.heading.z * this->distance_;
}

// 현재 state를 기준으로 회전
void LSystem::Rotate(const unsigned short& axis, const float& angle)
{
    // axis
    // 0: Pitch, x, Left
    // 1: Roll, y, Heading
    // 2: yaw, z, Up
    float rad = angle / 180.0f * PI;
    float cos = std::cos(rad);
    float sin = std::sin(rad);

    float x = this->state_.heading.x;
    float y = this->state_.heading.y;
    float z = this->state_.heading.z;

    switch (axis)
    {
		case 0:
		{
            // Pitch, x, Left
            float newY = cos * y - sin * z;
			float newZ = sin * y + cos * z;
            this->state_.heading.y = newY;
            this->state_.heading.z = newZ;
            
			break;
		}
		case 1:
		{
            // Roll, y, Heading
			float newX = cos * x + sin * z;
            float newZ = -1 * sin * x + cos * z;
            this->state_.heading.x = newX;
            this->state_.heading.z = newZ;

			break;
		}
		case 2:
		{
            // Yaw, z, Up
            float newX = cos * y + sin * x;
            float newY = -1 * sin * y + cos * x;

            this->state_.heading.x = newX;
            this->state_.heading.y = newY;
			break;
		}
    }
}