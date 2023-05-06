#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <stack>
#include <cmath>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "Utils.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

#pragma region Model
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
	float radiusUp = radiusDown * mDeltaThickness;

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
#pragma endregion

#pragma region utils
struct RuleKey
{
    std::string previous;
    std::string before;
    std::string next;
    std::string condition;
};

RuleKey ConvertKeyToRuleKey(std::string key)
{
    // previous, before, next
    RuleKey ruleKey = RuleKey();
    RemoveAll(key, ' ');    // 공백 제거

    int conditionIndex = key.find(':');
    if (conditionIndex != -1)
    {
        // condition이 있는 경우
        ruleKey.condition = key.substr(conditionIndex + 1, key.size() - conditionIndex);
        key = key.substr(0, conditionIndex);
    }

    int index = 0;
    int preIndex = key.find('<');
    int nextIndex = key.find('>');

    if (preIndex != -1)
    {
        ruleKey.previous = key.substr(0, preIndex);
        index = preIndex + 1;
    }

    if (nextIndex != -1)
    {
        ruleKey.before = key.substr(index, nextIndex - index);
        ruleKey.next = key.substr(nextIndex + 1, key.size() - nextIndex);
    }
    else
    {
        ruleKey.before = key.substr(index, key.size() - index);
        ruleKey.next = "";
    }

    return ruleKey;
}

std::vector<LLetter> ConvertStringToLLetter(std::string str)
{
    RemoveAll(str, ' ');

    std::vector<LLetter> letters = std::vector<LLetter>();
    for (int i = 0; i < str.size(); i++)
    {
        LLetter letter = LLetter(str[i]);

        if (i + 1 <= str.size() && str[i + 1] == '(')
        {
            int end = str.find(')', i);

            if (!end)
            {
                // error
                return letters;
            }

            // 파라미터 추가
            std::string parameterString = str.substr(i + 2, end - (i + 2));
            parameterString.erase(
                remove(parameterString.begin(), parameterString.end(), ' '),
                parameterString.end());
            letter.SetParameters(split(parameterString, ','));

            i = end;    // 괄호 위치 (다음 iter에서 i++ 됨)
        }

        letters.push_back(LLetter(letter));
    }

    return letters;
}

#pragma endregion

// Public
LSystem::LSystem()
{
    mRules = std::map<std::string, LRule>();
    mWord = std::vector<LLetter>();
    mIgnores = std::map<char, bool>();

    axisX = DirectX::XMFLOAT3(1, 0, 0);
    axisY = DirectX::XMFLOAT3(0, 1, 0);
    axisZ = DirectX::XMFLOAT3(0, 0, 1);

    mLeafDirection = { 0.0f, 1.0f, 0.0f };

    mState =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f),
        0.3f
    };
}

LSystem::~LSystem()
{
}

// Get, Set
std::string LSystem::GetWord() const
{
    std::string wordText;
    for (const LLetter& letter : mWord)
    {
        wordText += letter.GetLetter();
    }

    return wordText;
}

void LSystem::GetWord(char* out)
{
    int index = 0;
    for (const LLetter& letter : mWord)
    {
        for (const char& c : letter.GetLetter())
        {
            out[index] = c;
            index++;
        }
    }
}

std::map<std::string, LRule> LSystem::GetRules()
{
    return mRules;
}

float LSystem::GetAngleChange() const
{
    return mAngleChange;
}

float LSystem::GetDistance() const
{
    return mDistance;
}

float LSystem::GetThickness() const
{
    return mState.thickness;
}

float LSystem::GetDeltaThickness() const
{
    return mDeltaThickness;
}

void LSystem::SetAngleChange(const float& val)
{
    mAngleChange = val;
}

void LSystem::SetDistance(const float& val)
{
    mDistance = val;
}

float LSystem::GetLeafAngleChange() const
{
    return mLeafAngleChange;
}

float LSystem::GetLeafDistance() const
{
    return mLeafDistance;
}

void LSystem::SetLeafAngleChange(const float& val)
{
    mLeafAngleChange = val;
}

void LSystem::SetLeafDistance(const float& val)
{
    mLeafDistance = val;
}

void LSystem::SetWord(const std::string& word)
{
    mWord = ConvertStringToLLetter(word);
}

void LSystem::SetThickness(const float& val)
{
    mState.thickness = val;
}

void LSystem::SetDeltaThickness(const float& val)
{
    mDeltaThickness = val;
}

void LSystem::AddRule(std::string key, const std::string& value)
{
    RuleKey ruleKey = ConvertKeyToRuleKey(key);

    if (ruleKey.before.empty() || value.empty())
    {
        // error
        return;
    }

    std::string formatKey = "";
    bool startParam = false;
    for (const char& c : ruleKey.before)
    {
        if (startParam)
        {
            if (c == ',')
            {
                formatKey += c;
            }
            else if (c == ')')
            {
                startParam = false;
                formatKey += c;
            }
        }
        else
        {
            if (c == '(')
            {
                startParam = true;
            }

            formatKey += c;
        }
    }

    if (mRules.find(formatKey) != mRules.end())
    {
        // 동일한 key의 rule이 이미 있는 경우
        mRules[formatKey].AddAfter(LLetter(ruleKey.previous),
                                   LLetter(ruleKey.next),
                                   ConvertStringToLLetter(value),
                                   ruleKey.condition);
    }
    else
    {
        // 동일한 key의 rule이 없는 경우
        mRules.insert({ formatKey, LRule(LLetter(ruleKey.previous),
                                         LLetter(ruleKey.before),
                                         LLetter(ruleKey.next),
                                         ConvertStringToLLetter(value),
                                         ruleKey.condition) });
    }
}

void LSystem::DeleteRule(const std::string& key, const int& afterId)
{
    RuleKey ruleKey = ConvertKeyToRuleKey(key);

    auto iter = mRules.find(LLetter(ruleKey.before).GetFormat());

    if (iter == mRules.end())
    {
        // 없는 key의 rule을 삭제하는 경우 error
        return;
    }

    if (iter->second.DeleteAfter(afterId))
    {
        // after 삭제 후 남은 after가 없는 경우
        // L system rule map에서도 삭제
        mRules.erase(ruleKey.before);
    }
}

void LSystem::ClearRule()
{
    mRules.clear();
}

void LSystem::ClearState()
{
    mState =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f),
        0.3f
    };
}

// Ignore
std::map<char, bool> LSystem::GetIgnores()
{
    return mIgnores;
}

void LSystem::AddIgnore(char symbol)
{
    mIgnores.insert({ symbol, false });
}

void LSystem::DeleteIgnore(char symbol)
{
    if (mIgnores.find(symbol) != mIgnores.end())
    {
        mIgnores.erase(symbol);
    }
}

// Run
void LSystem::Iterate()
{
    std::vector<LLetter> iterated = std::vector<LLetter>();

    int size = mWord.size();
    //char previous = NULL;
    int depth = 0;
    std::vector<std::string> previousDepth = std::vector<std::string>();
    previousDepth.push_back("");  // 시작 depth에 NULL 추가
    std::string before;
    std::string next;
    for (int i = 0; i < size; i++)
    {
        LLetter beforeLLetter = mWord.at(i);
        before = beforeLLetter.GetLetter();

        // 괄호가 나오면 depth 조정
        if (before == "[")
        {
            depth++;

            if (previousDepth.size() < depth + 1)
            {
                // depth가 커지면 previous vector에 NULL 추가
                previousDepth.push_back("");
            }
        }
        else if (before == "]")
        {
            depth--;
        }

        // mRules는 LLetter의 format을 key로 가진 map 구조
        std::string letterFormat = beforeLLetter.GetFormat();
        if (mRules.find(letterFormat) != mRules.end())
        {
            // 해당 key(letter)를 가진 rule이 있는 경우

            // !!! not good
            // nextCheck가 ignore에 포함 안되면 갱신, 끝까지 갱신 안되면 NULL
            std::string nextCheck;
            next = "";
            int index = 1;
            int tempDepth = depth;
            while (i + index < size)
            {
                nextCheck = mWord.at(i + index++).GetLetter();

                // before 기준으로 tempDepth 계산
                if (nextCheck == "[")
                {
                    tempDepth++;
                }
                else if (nextCheck == "]")
                {
                    tempDepth--;
                }

                if (mIgnores.find(nextCheck[0]) == mIgnores.end())
                {
                    if (nextCheck[0] != '[' && nextCheck[0] != ']')
                    {
                        // 괄호는 ignore에 추가

                        if (depth == tempDepth)
                        {
                            // before의 depth와 tempDepth가 일치하는 경우
                            next = nextCheck;
                            break;
                        }
                    }
                }
            }

            std::vector<LLetter> after;
            if (previousDepth[depth].empty() && depth - 1 >= 0)
            {
                // 현재 depth의 previous 이력이 없는 경우 직전의 분기한 경우
                // 이전 depth의 previous로 체크
                after = mRules[letterFormat].GetAfter(previousDepth[depth - 1], next, beforeLLetter);
            }
            else
            {
                after = mRules[letterFormat].GetAfter(LLetter(previousDepth[depth]), LLetter(next), beforeLLetter);
            }

            if (after.size() == 0)
            {
                // CS rule이 없는 경우
                iterated.push_back(beforeLLetter);
            }
            else
            {
                iterated.insert(iterated.end(), after.begin(), after.end());
            }
        }
        else
        {
            iterated.push_back(beforeLLetter);
        }

        if (mIgnores.find(before[0]) == mIgnores.end())
        {
            // ignores에 포함 안되는 경우
            // 다음 depth별 previous 체크 symbol 갱신
            if (before[0] != '[' && before[0] != ']')
            {
                // [, ] 도 ignore에 포함
                previousDepth[depth] = before;
            }
        }
    }

    mWord = iterated;
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
    if (mWord.size() < 1)
    {
        return;
    }

    float width = 0.5;
    std::stack<State> ss;
    std::vector<Vector3>* leaf = nullptr;
    std::stack<std::vector<Vector3>*> leafstack;

    Vector3 startPos;
    Vector3 endPos;

    startPos = mState.position;
    for (const LLetter& letter : mWord)
    {
        // Move: 이동 후 현재 위치 end에 저장 후 push
        switch (letter.GetType())
        {
            case LLetter::Type::Forward:
            {
                // Draw + Move forward
                this->Move();
                endPos = mState.position;
                out->push_back(
                    CreateCylinder(startPos, endPos, mState.quaternion, 
                                   mState.thickness, mDistance, 50));
                startPos = mState.position;
                break;
            }
            case LLetter::Type::NoDrawForward: {
                this->Move();
                endPos = mState.position;
                break;
            }
            case LLetter::Type::NoDrawForward2:
            {
                // No Draw + Move foward
                this->Move(mLeafDistance);
                //endPos = mState.position;
                //out->push_back(CreateLineModel(startPos, endPos));
                break;
            }
            case LLetter::Type::RollLeft:
            {
                // mAngleChange 만큼 x축 회전
                if (mDrawingLeaf) {
                    this->Rotate(0, mLeafAngleChange);
                }
                else {
                    this->Rotate(0, mAngleChange);
                }
                break;
            }
            case LLetter::Type::RollRight:
            {
                // -mAngleChange 만큼 x축 회전
                if (mDrawingLeaf) {
                    this->Rotate(0, -mLeafAngleChange);
                }
                else {
                    this->Rotate(0,- mAngleChange);
                }
                break;
            }
            case LLetter::Type::PitchUp:
            {
                // mAngleChange 만큼 y축 회전
                if (mDrawingLeaf) {
                    this->Rotate(1, mLeafAngleChange);
                }
                else {
                    this->Rotate(1, mAngleChange);
                }
                break;
            }
            case LLetter::Type::PitchDown:
            {
                // -mAngleChange 만큼 y축 회전
                if (mDrawingLeaf) {
                    this->Rotate(1, -mLeafAngleChange);
                }
                else {
                    this->Rotate(1, -mAngleChange);
                }
                break;
            }
            case LLetter::Type::TurnLeft:
            {
                // mAngleChange 만큼 z축 회전
                //this->Rotate(2, mAngleChange);

                if (mDrawingLeaf) {
                    this->Rotate(2, mLeafAngleChange);
                }
                else {
                    this->Rotate(2, mAngleChange);
                }

                break;
            }
            case LLetter::Type::TurnRight:
            {
                // -mAngleChange 만큼 z축 회전
                //this->Rotate(2, -mAngleChange);

                if (mDrawingLeaf) {
                    this->Rotate(2, -mLeafAngleChange);
                }
                else {
                    this->Rotate(2, -mAngleChange);
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
                ss.push(mState);
                break;
            }
            case LLetter::Type::Pop:
            {
                // 이전 State 복원
                // 위치도 같이 옮겨지는 경우 No draw
                mState = ss.top();
                ss.pop();
                startPos = mState.position;
                break;
            }
            case LLetter::Type::StartingPoint:
            {
                leaf = new std::vector<Vector3>();

                leaf->push_back(mState.position);

                mDrawingLeaf = true;

                leafstack.push(leaf);
                break;
            }
            case LLetter::Type::MarkingPoint:
            {
                leaf->push_back(mState.position);

                break;
            }
            case LLetter::Type::EndingPoint:
            {
                out->push_back(CreateLeaf(leaf, mState.direction));

                if (!leafstack.empty()) {
                    leaf = leafstack.top();
                    leafstack.pop();
                }
                else {
                    leaf = nullptr;
                }

                mDrawingLeaf = false;
                mLeafDirection = { 0.0f, 1.0f, 0.0f };

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

                int index = inp.find("->");
                this->AddRule(inp.substr(0, index), inp.substr(index + 2, inp.size()));
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
            else if (key == "ignore")
            {
                for (const char& c : value)
                {
                    this->AddIgnore(c);
                }
            }
        }
    }

    return;
}

// Private
void LSystem::Move()
{
    // Heading Vector에 distance 곱해서 움직여주기
    mState.position.x += mState.direction.x * mDistance;
    mState.position.y += mState.direction.y * mDistance;
    mState.position.z += mState.direction.z * mDistance;
    mState.thickness *= mDeltaThickness;
}
void LSystem::Move(float distance) // Symbol : G (Leaf)
{
    // Heading Vector에 distance 곱해서 움직여주기
    mState.position.x += mLeafDirection.x * distance;
    mState.position.y += mLeafDirection.y * distance;
    mState.position.z += mLeafDirection.z * distance;
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
    if (mDrawingLeaf)
    {
        x = mLeafDirection.x;
        y = mLeafDirection.y;
        z = mLeafDirection.z;
    }
    else
    {
        x = mState.direction.x;
        y = mState.direction.y;
        z = mState.direction.z;
    }

    float newX = x;
    float newY = y;
    float newZ = z;

    switch (axis)
    {
		case 0:
		{
			if (!mDrawingLeaf)
			{
				// Roll, x
				mState.quaternion = DirectX::XMQuaternionMultiply(
                    mState.quaternion,
                    DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), rad));
			}

			newY = cos * y - sin * z;
			newZ = sin * y + cos * z;
			break;
		}
		case 1:
		{
			if (!mDrawingLeaf)
			{
				// Pitch, y
				mState.quaternion = DirectX::XMQuaternionMultiply(
                    mState.quaternion,
                    DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisY), rad));
			}

			newX = cos * x + sin * z;
			newZ = -sin * x + cos * z;
			break;
		}
		case 2:
		{
			if (!mDrawingLeaf)
			{
				// Turn, z
				mState.quaternion = DirectX::XMQuaternionMultiply(
                    mState.quaternion, 
                    DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisZ), rad));
			}

			newX = cos * x - sin * y;
			newY = sin * x + cos * y;
			break;
		}
    }

    if (mDrawingLeaf)
    {
        mLeafDirection.x = newX;
        mLeafDirection.y = newY;
        mLeafDirection.z = newZ;
    }
    else
    {
        mState.direction.x = newX;
        mState.direction.y = newY;
        mState.direction.z = newZ;
    }

    mState.direction.Normalized();
    mLeafDirection.Normalized();
}

void LSystem::Reset()
{
    this->ClearRule();
    this->ClearState();
    mAngleChange = 90.0f;
    mDistance = 1.0f;
    mDeltaThickness = 1.0f;
    //float mLeafAngleChange = 22.5f;
    //float mLeafDistance = 0.5f;
    //bool mDrawingLeaf = false;
}