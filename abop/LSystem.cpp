#include <string>
#include <vector>
#include <stack>
#include <cassert>
#include <cmath>
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"


// TEMP
float Dot(const Vector3& vec1, const Vector3& vec2) {
    float ret = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    return ret;
}

float Distance(const Vector3& vec) {
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

void Cross(Vector3& vec1, const Vector3& vec2) {
    float newX = (vec1.y * vec2.z) - (vec1.z * vec2.y);
    float newY = (vec1.z * vec2.x) - (vec1.x * vec2.z);
    float newZ = (vec1.x * vec2.y) - (vec1.y * vec2.x);
    vec1.x = newX, vec1.y = newY, vec1.z = newZ;
}

// TEMP
Model CreateTrunk(Vector3 startPos, Vector3 endPos, Vector3 heading, Vector3 right, const float& distance)
{
    // !!! TEMP

    Model model;
    model.modelType = ModelType::CubeModel;
    model.dataCount = 9;
    model.data = new float[9];

    Vector3 position = (startPos + endPos) / 2.0f;
    Vector3 axisX = { 1.0f, 0.0f, 0.0f };
    Vector3 axisY = { 0.0f, 1.0f, 0.0f };
    Vector3 axisZ = { 0.0f, 0.0f, 1.0f };

    //float angleX = atan2(rotation.y, rotation.z);
    ////float angleY = atan2(rotation.z, rotation.x);
    //float angleY = atan2(rotation.x, rotation.z);
    //float angleZ = atan2(rotation.y, rotation.x);

    model.data[0] = position.x;
    model.data[1] = position.y;
    model.data[2] = position.z;
    Vector3 cross = heading;
    Cross(cross, right);
    cross.Normalized();

    model.data[3] = acosf(Dot(cross, axisX));
    model.data[4] = acosf(Dot(cross, axisZ));
    model.data[5] = acosf(Dot(cross, axisY));
    //model.data[3] = rotation.x * PI / 180.0f;     // pitch
    //model.data[4] = rotation.z * PI / 180.0f;     // roll
    //model.data[5] = rotation.y * PI / 180.0f;     // yaw
    //model.data[3] = 90.0f * PI / 180.0f;     // pitch
    //model.data[4] = 0.0f * PI / 180.0f;     // roll
    //model.data[5] = 0.0f * PI / 180.0f;     // yaw
    model.data[6] = 0.3f;       // size.x
    model.data[7] = 0.3f;       // size.y
    model.data[8] = distance;       // size.z (height)

    return model;
}

Model CreateLeaf(std::vector<Vector3>* leaf)
{
    int size = leaf->size();

    // !!! color 일단 그린 고정
    Vector4 green { 0.0f, 1.0f, 0.0f, 0.0f };

    Model model;

    model.vertexCount = size;
    model.vertexTypes = new VertexType[size];
    model.indexCount = (size - 1) * 3;
    model.indices = new int[model.indexCount];

    // TEMP
    for (int i = 0; i < size; i++)
    {
        model.vertexTypes[i] = VertexType { (*leaf)[i], green };
    }

    int i = 0;
    int vertex = 1;
    while (i < (size - 1) * 3 - 1)
    {
        model.indices[i++] = 0;
        model.indices[i++] = vertex++;
        model.indices[i++] = vertex;
    }
    model.indices[--i] = 1;

    return model;
}

// Public
LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();

    this->state_ = 
    { 
        {0.0f, 0.0f, 0.0f}, 
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}   
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

    float width = 0.5f;
    std::stack<State> ss;
    std::vector<Vector3>* leaf = nullptr;

    Vector3 startPos;
    Vector3 endPos;

    startPos = this->state_.position;
    for (const LLetter& letter : *(this->word_))
    {
        // Move: 이동 후 현재 위치 end에 저장 후 push
        switch (letter.GetType())
        {
            case LLetter::Type::Forward:
            {
                // Draw + Move forward
                this->Move();
                endPos = this->state_.position;
                out->push_back(CreateTrunk(startPos, endPos, this->state_.heading, this->state_.right,  this->distance_));
                startPos = this->state_.position;
                break;
            }
            case LLetter::Type::NoDrawForward:
            case LLetter::Type::NoDrawForward2:
            {
                // No Draw + Move foward
                this->Move();
                endPos = this->state_.position;
                //out->push_back(CreateLineModel(startPos, endPos));
                break;
            }
            case LLetter::Type::RollLeft:
            {
                // angleChange_ 만큼 y축 회전
                this->Rotate(1, -angleChange_);
                break;
            }
            case LLetter::Type::RollRight:
            {
                // -angleChange_ 만큼 y축 회전
                this->Rotate(1, angleChange_);
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
                this->Rotate(2, -angleChange_);
                break;
            }
            case LLetter::Type::TurnRight:
            {
                // -angleChange_ 만큼 z축 회전
                this->Rotate(2, angleChange_);
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
            case LLetter::Type::StartingPoint:
            {
                leaf = new std::vector<Vector3>();
                
                leaf->push_back(this->state_.position);
                break;
            }
            case LLetter::Type::MarkingPoint:
            {
                leaf->push_back(this->state_.position);
                break;
            }
            case LLetter::Type::EndingPoint:
            {
                out->push_back(CreateLeaf(leaf));
                leaf = nullptr;
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
            float dot = Dot(this->state_.heading, this->state_.right);
            Vector3 cross = this->state_.heading;
            Cross(cross, this->state_.right);
            this->state_.heading = this->state_.heading * cos;
            this->state_.heading = this->state_.heading + this->state_.right * dot * (1 - cos);
            this->state_.heading = this->state_.heading - cross * sin;
			break;
		}
		case 1:
		{
            // Roll, y, Heading
            float dot = Dot(this->state_.right, this->state_.heading);
            Vector3 cross = this->state_.right;
            Cross(cross, this->state_.heading);
            this->state_.right = this->state_.right * cos;
            this->state_.right = this->state_.right + this->state_.heading * dot * (1 - cos);
            this->state_.right = this->state_.right - cross * sin;
			break;
		}
		case 2:
		{
            // Yaw, z, Up
            Vector3 axis = this->state_.heading;
            Cross(axis, this->state_.right);

            // direction.Rotate(axis, angle);
            float dot1 = Dot(this->state_.heading, axis);
            Vector3 cross1 = this->state_.heading;
            Cross(cross1, axis);
            this->state_.heading = this->state_.heading * cos;
            this->state_.heading = this->state_.heading + axis * dot1 * (1 - cos);
            this->state_.heading = this->state_.heading - cross1 * sin;

            // right.Rotate(axis, angle);
            float dot2 = Dot(this->state_.right, axis);
            Vector3 cross2 = this->state_.right;
            Cross(cross2, axis);
            this->state_.right = this->state_.right * cos;
            this->state_.right = this->state_.right + axis * dot2 * (1 - cos);
            this->state_.right = this->state_.right - cross2 * sin;

			break;
		}
    }

    this->state_.heading.Normalized();
    this->state_.right.Normalized();
}