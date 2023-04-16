#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

// Model 관련 
Model LSystem::CreateTrunk(Vector3& startPos, Vector3& endPos, DirectX::XMVECTOR& quaternion, const float& thickness, const float& distance)
{
    Model model;
    model.modelType = ModelType::CubeModel;
    model.dataCount = 10;
    model.data = new float[10];

    Vector3 position = (startPos + endPos) / 2.0f;

    model.data[0] = position.x;
    model.data[1] = position.y;
    model.data[2] = position.z;
    model.data[3] = DirectX::XMVectorGetX(quaternion);
    model.data[4] = DirectX::XMVectorGetY(quaternion);
    model.data[5] = DirectX::XMVectorGetZ(quaternion);
    model.data[6] = DirectX::XMVectorGetW(quaternion);
    model.data[7] = thickness;       // width
    model.data[8] = thickness;       // depth
    model.data[9] = distance;       // height

    return model;
}

Model LSystem::CreateCylinder(Vector3& startPos, Vector3& endPos, DirectX::XMVECTOR& quaternion, const float& radiusDown, const float& distance, const int& segment)
{
    Model model;
    model.modelType = ModelType::CylinderModel;
    model.dataCount = 10;
    model.data = new float[11];

    Vector3 position = (startPos + endPos) / 2.0f;
	float radiusUp = radiusDown * this->deltaThickness_;

    model.data[0] = position.x;
    model.data[1] = position.y;
    model.data[2] = position.z;
    model.data[3] = DirectX::XMVectorGetX(quaternion);
    model.data[4] = DirectX::XMVectorGetY(quaternion);
    model.data[5] = DirectX::XMVectorGetZ(quaternion);
    model.data[6] = DirectX::XMVectorGetW(quaternion);
    model.data[7] = radiusDown;       // Radius Down
    model.data[8] = radiusUp;       // Radius Up
    model.data[9] = distance;       // Height
    model.data[10] = segment;       // Segment

    return model;
}

Model LSystem::CreateLeaf(std::vector<Vector3>* leaf, Vector3& direction)
{
    int size = leaf->size();

    // !!! color 일단 그린 고정
    Vector4 green{ 0.19f, 0.35f, 0.15f, 0.0f };

    DirectX::XMFLOAT3 axisZero(0.0f, 0.0f, 0.0f); // 잎의 방향 벡터와 현재 state의 direction 벡터가 같은 방향일 경우, 두 벡터를 Cross 연산한 값을 사용하지 않고 Default 값으로 사용하기 위해 선언 (오류 발생)

    DirectX::XMFLOAT3 dir_axis(direction.x, direction.y, direction.z);
    DirectX::XMVECTOR radianY = DirectX::XMVector3AngleBetweenNormals(DirectX::XMLoadFloat3(&axisY), DirectX::XMLoadFloat3(&dir_axis)); // 잎이 자라는 방향인 (0, 1, 0) 벡터와 현재 state의 direction 벡터 사이의 각

    DirectX::XMVECTOR realAxisY = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&axisY), DirectX::XMLoadFloat3(&dir_axis))); // 두 벡터가 같은 방향이 아닐 때 사용하는 real axis
    DirectX::XMVECTOR rotationQuaternion;

    if (!DirectX::XMVector3Equal(realAxisY, DirectX::XMLoadFloat3(&axisZero))) // Cross 연산의 값이 Zero 벡터가 아닌지 체크 후, 아니라면 realAxis 사용
    {
        rotationQuaternion = DirectX::XMQuaternionRotationAxis(realAxisY, DirectX::XMVectorGetX(radianY));
    }
    else
    {
        rotationQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisY), DirectX::XMVectorGetX(radianY));
    }

    Model model;

    model.modelType = ModelType::LeafModel;
    model.vertexCount = size;
    model.vertexTypes = new VertexType[size];
    model.indexCount = (size - 1) * 6;
    model.indices = new int[model.indexCount];

    // TEMP
    for (int i = 0; i < size; i++)
    {

        Vector3 tempPos{ (*leaf)[i].x - (*leaf)[0].x, (*leaf)[i].y - (*leaf)[0].y, (*leaf)[i].z - (*leaf)[0].z }; // 잎 시작점 기준으로 모든 좌표 원점으로 이동  

        DirectX::XMFLOAT3 vertex = { tempPos.x, tempPos.y, tempPos.z };
        DirectX::XMVECTOR vertexVector = DirectX::XMLoadFloat3(&vertex);
        DirectX::XMVECTOR rotatedVertexVector = DirectX::XMVector3Rotate(vertexVector, rotationQuaternion);
        DirectX::XMFLOAT3 rotatedVertex;
        DirectX::XMStoreFloat3(&rotatedVertex, rotatedVertexVector);

        tempPos.x = rotatedVertex.x + (*leaf)[0].x, tempPos.y = rotatedVertex.y + (*leaf)[0].y, tempPos.z = rotatedVertex.z + (*leaf)[0].z; // 원점 기준으로 모든 좌표 회전 변환 한 뒤 다시 원위치

        model.vertexTypes[i] = VertexType{ tempPos , green };

    }

    int i = 0;
    int vertex = 1;
    while (i < (size - 1) * 5 - 1)
    {
        int a = vertex++;
        int b = vertex;

        model.indices[i++] = 0;
        model.indices[i++] = a;
        model.indices[i++] = b;

        model.indices[i++] = 0;
        model.indices[i++] = b;
        model.indices[i++] = a;
    }
    // model.indices[--i] = 1;

    return model;
}

// Public
LSystem::LSystem()
{
    rules_ = std::vector<LRule>();
    word_ = new std::vector<LLetter>();

    axisX = DirectX::XMFLOAT3(1, 0, 0);
    axisY = DirectX::XMFLOAT3(0, 1, 0);
    axisZ = DirectX::XMFLOAT3(0, 0, 1);

    leafDirection = { 0.0f, 1.0f, 0.0f };

    this->state_ =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f),
        0.3f
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

void LSystem::GetWord(char* out)
{
    int index = 0;
    for (LLetter& letter : *this->word_)
    {
        out[index] = letter.GetLetter();
        index++;
    }
}

std::vector<LRule> LSystem::GetRules() const
{
    return this->rules_;
}

std::string LSystem::GetRuleText() const
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

float LSystem::GetThickness() const
{
    return this->state_.thickness;
}

float LSystem::GetDeltaThickness() const
{
    return this->deltaThickness_;
}

void LSystem::SetAngleChange(const float& val)
{
    this->angleChange_ = val;
}

void LSystem::SetDistance(const float& val)
{
    this->distance_ = val;
}

float LSystem::GetLeafAngleChange() const
{
    return this->leafAngleChange_;
}

float LSystem::GetLeafDistance() const
{
    return this->leafDistance_;
}

void LSystem::SetLeafAngleChange(const float& val)
{
    this->leafAngleChange_ = val;
}

void LSystem::SetLeafDistance(const float& val)
{
    this->leafDistance_ = val;
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

void LSystem::SetThickness(const float& val)
{
    this->state_.thickness = val;
}

void LSystem::SetDeltaThickness(const float& val)
{
    this->deltaThickness_ = val;
}

// Rule
void LSystem::AddRule(const std::string& ruleText)
{
    this->rules_.push_back(LRule(ruleText));
}

void LSystem::AddRule(const char& key, const std::string& value)
{
    bool found = false;
    int index;
    for (int i = 0; i < this->rules_.size(); i++)
    {
        auto& rule = this->rules_[i];
        if (rule.GetBefore().IsEqual(key))
        {
            found = true;
            index = i;
            break;
        }
    }

    if (!found)
    {
        this->rules_.push_back(LRule(key, value));
    }
    else
    {
        this->rules_[index].SetRule(key, value);
    }
}

void LSystem::AddRule(const std::string& key, const std::string& value)
{
    this->AddRule(key[0], value);
}

void LSystem::DeleteRule(const char& key)
{
    //for (LRule& rule : this->rules_)
    for (int i = 0; i < this->rules_.size(); i++)
    {
        if (this->rules_[i].GetBefore().GetLetter() == key)
        {
            this->rules_.erase(this->rules_.begin() + i);
            break;
        }
    }
}

void LSystem::ClearRule()
{
    this->rules_ = std::vector<LRule>();
}

void LSystem::ClearState()
{
    this->state_ =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f),
        0.3f
    };
}

// Run
void LSystem::Iterate()
{
    bool changed;
    std::vector<LLetter>* v = new std::vector<LLetter>();

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
    if (this->word_->size() < 1)
    {
        return;
    }

    float width = 0.5;
    std::stack<State> ss;
    std::vector<Vector3>* leaf = nullptr;
    std::stack<std::vector<Vector3>*> leafstack;

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
                out->push_back(CreateCylinder(startPos, endPos, this->state_.quaternion, this->state_.thickness, this->distance_, 50));
                startPos = this->state_.position;
                break;
            }
            case LLetter::Type::NoDrawForward: {
                this->Move();
                endPos = this->state_.position;
                break;
            }
            case LLetter::Type::NoDrawForward2:
            {
                // No Draw + Move foward
                this->Move(leafDistance_);
                //endPos = this->state_.position;
                //out->push_back(CreateLineModel(startPos, endPos));
                break;
            }
            case LLetter::Type::RollLeft:
            {
                // angleChange_ 만큼 x축 회전
                if (drawingLeaf_) {
                    this->Rotate(0, leafAngleChange_);
                }
                else {
                    this->Rotate(0, angleChange_);
                }
                break;
            }
            case LLetter::Type::RollRight:
            {
                // -angleChange_ 만큼 x축 회전
                if (drawingLeaf_) {
                    this->Rotate(0, -leafAngleChange_);
                }
                else {
                    this->Rotate(0,- angleChange_);
                }
                break;
            }
            case LLetter::Type::PitchUp:
            {
                // angleChange_ 만큼 y축 회전
                if (drawingLeaf_) {
                    this->Rotate(1, leafAngleChange_);
                }
                else {
                    this->Rotate(1, angleChange_);
                }
                break;
            }
            case LLetter::Type::PitchDown:
            {
                // -angleChange_ 만큼 y축 회전
                if (drawingLeaf_) {
                    this->Rotate(1, -leafAngleChange_);
                }
                else {
                    this->Rotate(1, -angleChange_);
                }
                break;
            }
            case LLetter::Type::TurnLeft:
            {
                // angleChange_ 만큼 z축 회전
                //this->Rotate(2, angleChange_);

                if (drawingLeaf_) {
                    this->Rotate(2, leafAngleChange_);
                }
                else {
                    this->Rotate(2, angleChange_);
                }

                break;
            }
            case LLetter::Type::TurnRight:
            {
                // -angleChange_ 만큼 z축 회전
                //this->Rotate(2, -angleChange_);

                if (drawingLeaf_) {
                    this->Rotate(2, -leafAngleChange_);
                }
                else {
                    this->Rotate(2, -angleChange_);
                }

                break;
            }
            case LLetter::Type::TurnAround:
            {
                // 180도 z축 회전
                this->Rotate(2, 180.f);
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

                this->drawingLeaf_ = true;

                leafstack.push(leaf);
                break;
            }
            case LLetter::Type::MarkingPoint:
            {
                leaf->push_back(this->state_.position);

                break;
            }
            case LLetter::Type::EndingPoint:
            {
                out->push_back(CreateLeaf(leaf, this->state_.direction));

                if (!leafstack.empty()) {
                    leaf = leafstack.top();
                    leafstack.pop();
                }
                else {
                    leaf = nullptr;
                }

                this->drawingLeaf_ = false;
                this->leafDirection = { 0.0f, 1.0f, 0.0f };

                break;
            }
            case LLetter::Type::None:
            {
                break;
            }
         }
    }
}

void LSystem::LoadPreset(std::string& filename)
{
    // TODO: 잘못된 파일 예외 처리하기
    std::ifstream ifs;

    this->Reset();

    ifs.open(filename);

    std::string inp;
    while (ifs >> inp)
    {
        if (inp == "rule")
        {
            while (ifs >> inp)
            {
                if (inp == "end")
                {
                    break;
                }

                int index = inp.find(':');
                this->AddRule(inp.substr(0, index), inp.substr(index + 1, inp.size()));
            }
        }
        else
        {
            int index = inp.find(':');

            std::string key = inp.substr(0, index);
            std::string value = inp.substr(index + 1, inp.size());

            if (key == "word")
            {
                this->SetWord(value);
            }
            else if (key == "angle")
            {
                this->SetAngleChange(std::stof(value));
            }
            else if (key == "thickness")
            {
                this->SetThickness(std::stof(value));
            }
            else if (key == "deltaThickness")
            {
                this->SetDeltaThickness(std::stof(value));
            }
            else if (key == "distance")
            {
                this->SetDistance(std::stof(value));
            }
        }
    }

    return;
}

// Private
void LSystem::Move()
{
    // Heading Vector에 distance 곱해서 움직여주기
    this->state_.position.x += this->state_.direction.x * this->distance_;
    this->state_.position.y += this->state_.direction.y * this->distance_;
    this->state_.position.z += this->state_.direction.z * this->distance_;
    this->state_.thickness *= this->deltaThickness_;
}
void LSystem::Move(float distance) // Symbol : G (Leaf)
{
    // Heading Vector에 distance 곱해서 움직여주기
    this->state_.position.x += this->leafDirection.x * distance;
    this->state_.position.y += this->leafDirection.y * distance;
    this->state_.position.z += this->leafDirection.z * distance;
}

// 현재 state를 기준으로 회전
void LSystem::Rotate(const unsigned short& axis, const float& angle)
{
    // axis
    // 0: Roll, X
    // 1: Pitch, Y
    // 2: Turn, Z
    float rad = angle / 180.0f * PI;
    float cos = std::cosf(rad);
    float sin = std::sinf(rad);

    float x, y, z;
    if (drawingLeaf_) 
    {
        x = this->leafDirection.x;
        y = this->leafDirection.y;
        z = this->leafDirection.z;
    }
    else
    {
        x = this->state_.direction.x;
        y = this->state_.direction.y;
        z = this->state_.direction.z;
    }

    float newX = x;
    float newY = y;
    float newZ = z;

    switch (axis)
    {
		case 0:
		{
			if (!drawingLeaf_)
			{
				// Roll, x
				this->state_.quaternion = DirectX::XMQuaternionMultiply(this->state_.quaternion, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), rad));
			}

			newY = cos * y - sin * z;
			newZ = sin * y + cos * z;
			break;
		}
		case 1:
		{
			if (!drawingLeaf_)
			{
				// Pitch, y
				this->state_.quaternion = DirectX::XMQuaternionMultiply(this->state_.quaternion, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisY), rad));
			}

			newX = cos * x + sin * z;
			newZ = -sin * x + cos * z;
			break;
		}
		case 2:
		{
			if (!drawingLeaf_)
			{
				// Turn, z
				this->state_.quaternion = DirectX::XMQuaternionMultiply(this->state_.quaternion, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisZ), rad));
			}

			newX = cos * x - sin * y;
			newY = sin * x + cos * y;
			break;
		}
    }

    if (drawingLeaf_)
    {
        this->leafDirection.x = newX;
        this->leafDirection.y = newY;
        this->leafDirection.z = newZ;
    }
    else
    {
        this->state_.direction.x = newX;
        this->state_.direction.y = newY;
        this->state_.direction.z = newZ;
    }

    this->state_.direction.Normalized();
    this->leafDirection.Normalized();
}

void LSystem::Reset()
{
    this->ClearRule();
    this->ClearState();
    this->angleChange_ = 90.0f;
    this->distance_ = 1.0f;
    this->deltaThickness_ = 1.0f;
    //float leafAngleChange_ = 22.5f;
    //float leafDistance_ = 0.5f;
    //bool drawingLeaf_ = false;
}