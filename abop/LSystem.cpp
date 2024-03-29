#include <fstream>
#include <vector>
#include <iostream>
#include <stack>
#include <cmath>
#include <map>
#include <string>
#include <fbxsdk.h>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "Utils.hpp"
#include "Constant.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "LSystem.hpp"

#pragma region Model

const char* GetModelPath(int type)
{
	switch (type)
	{
		case 1:
			return "./data/preset_flower/whiteFlower_A.fbx";
		case 2:
            return "./data/preset_flower/whiteFlower_B.fbx";
		case 3:
			return "./data/preset_flower/whiteFlower_C.fbx";
        case 4:
            return "./data/preset_flower/flower.fbx";
	}

    return "";
}

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

Model LSystem::CreateLeafSegment(std::vector<Vector3>* leaf)
{
    int size = leaf->size();

    Vector4 green{ 0.19f, 0.35f, 0.15f, 0.0f };

    Model model;

    model.modelType = ModelType::LeafModel;
    model.vertexCount = size;
    model.vertexTypes = new VertexType[size];
    model.indexCount = 6;
    model.indices = new int[model.indexCount];

    // TEMP
    for (int i = 0; i < size; i++)
        model.vertexTypes[i] = VertexType{ leaf->at(i), green };

    int i = 0;
    int vertex = 1;
    model.indices[0] = 0;
    model.indices[1] = 1;
    model.indices[2] = 2;
    model.indices[3] = 0;
    model.indices[4] = 2;
    model.indices[5] = 1;

    return model;
}

Model LSystem::CreateLeafPreset(Vector3& position, DirectX::XMVECTOR& quaternion, const int& type, const float& scale)
{
    Model model;

    model.modelType = ModelType::PresetLeafModel;

    model.dataCount = 9;
    model.data = new float[model.dataCount];

    model.data[0] = position.x;
    model.data[1] = position.y;
    model.data[2] = position.z;
    model.data[3] = DirectX::XMVectorGetX(quaternion);
    model.data[4] = DirectX::XMVectorGetY(quaternion);
    model.data[5] = DirectX::XMVectorGetZ(quaternion);
    model.data[6] = DirectX::XMVectorGetW(quaternion);
    model.data[7] = type;
    model.data[8] = scale;

    return model;
}

bool LSystem::LoadModel(const char* filePath, std::vector<Model>* out, float scale, Vector4& color)
{
    std::vector<TextureVertex>* vertices = new std::vector<TextureVertex>();

    FbxManager* manager = FbxManager::Create();

    FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    FbxImporter* importer = FbxImporter::Create(manager, "");
    FbxScene* mFbxScene = FbxScene::Create(manager, "");

    bool status = importer->Initialize(filePath, -1, manager->GetIOSettings());
    if (status == false)
    {
        std::cout << "(initialize)status: false -> return false" << '\n';
        return false;
    }

    status = importer->Import(mFbxScene);
    if (status == false)
    {
        std::cout << "(import)status: false -> return false" << '\n';
        return false;
    }

    importer->Destroy();

    FbxNode* rootNode = mFbxScene->GetRootNode();

    if (rootNode)
    {
        for (int i = 0; i < rootNode->GetChildCount(); i++)
        {
            FbxNode* childNode = rootNode->GetChild(i);

            if (childNode->GetNodeAttribute() == NULL)
                continue;

            FbxNodeAttribute::EType AttributeType = childNode->GetNodeAttribute()->GetAttributeType();

            if (AttributeType != FbxNodeAttribute::eMesh)
                continue;

            FbxMesh* mesh = (FbxMesh*)childNode->GetNodeAttribute();

            FbxVector4* pVertices = mesh->GetControlPoints();

            const int VertexCount = mesh->GetControlPointsCount();
            const int PolygonCount = mesh->GetPolygonCount();
            int vertexCounter = 0;
            FbxGeometryElementNormal* normalEl = mesh->GetElementNormal();

            for (int PolygonIndex = 0; PolygonIndex < PolygonCount; PolygonIndex++)
            {
                const int VerticeCount = mesh->GetPolygonSize(PolygonIndex);
                for (int VerticeIndex = 0; VerticeIndex < VerticeCount; VerticeIndex++)
                {
                    DirectX::XMFLOAT3 position;
                    DirectX::XMFLOAT2 texture;
                    DirectX::XMFLOAT3 normalvector;

                    int index = mesh->GetPolygonVertex(PolygonIndex, VerticeIndex);
                    position.x = (float)pVertices[index].mData[0];
                    position.y = (float)pVertices[index].mData[1] ;
                    position.z = (float)pVertices[index].mData[2];

                    FbxVector4 normal = normalEl->GetDirectArray().GetAt(vertexCounter++);
                    normalvector.x = (float)normal[0];
                    normalvector.y = (float)normal[1];
                    normalvector.z = (float)normal[2];

                    FbxVector2 fbxTexCoord;
                    FbxStringList UVSetNameList;
                    for (int k = 0; k < mesh->GetElementUVCount(); ++k)
                    {
                        FbxGeometryElementUV* eUV = mesh->GetElementUV(k);
                        int TextureUVIndex = mesh->GetTextureUVIndex(PolygonIndex, VerticeIndex);
                        fbxTexCoord = eUV->GetDirectArray().GetAt(TextureUVIndex);
                        texture.x = static_cast<float>(fbxTexCoord[0]);
                        texture.y = static_cast<float>(fbxTexCoord[1]);
                    }

                    //std::cout << "position : " << position.x << " " << position.y << " " << position.z << '\n';
                    //std::cout << "normalvector : " << normalvector.x << " " << normalvector.y << " " << normalvector.z << '\n';
                    //std::cout << "texture : " << texture.x << " " << texture.y << '\n';

                    vertices->push_back({ position, texture, normalvector });

                }
            }
        }
    }
    out->push_back(CreateModel(vertices, scale, color));

    return true;
}

Model LSystem::CreateModel(std::vector<TextureVertex>* vertices, float scale, Vector4& color)
{
    int size = vertices->size();
    Vector3 position;

    Model model;
    model.modelType = ModelType::Custom;
    model.vertexCount = size;
    model.vertexTypes = new VertexType[size];
    model.texVertexTypes = new TextureVertex[size];
    model.indexCount = size * 3;
    model.indices = new int[model.indexCount];

    model.dataCount = 8;
    model.data = new float[8];
    model.data[0] = DirectX::XMVectorGetX(mState.quaternion);
    model.data[1] = DirectX::XMVectorGetY(mState.quaternion);
    model.data[2] = DirectX::XMVectorGetZ(mState.quaternion);
    model.data[3] = DirectX::XMVectorGetW(mState.quaternion);
    model.data[4] = mState.position.x;
    model.data[5] = mState.position.y;
    model.data[6] = mState.position.z;
    model.data[7] = scale;

    for (int i = 0; i < size; i++)
    {
        position.x = (*vertices)[i].position.x, position.y = (*vertices)[i].position.y, position.z = (*vertices)[i].position.z;
        model.vertexTypes[i] = VertexType{ position, color };
        model.texVertexTypes[i] = TextureVertex{ (*vertices)[i].position, (*vertices)[i].texture, (*vertices)[i].normalVector };
    }

    int i = 0;
    int vertex = 0;
    while (i < size * 3 - 1)
    {
        model.indices[i++] = vertex++;
        model.indices[i++] = vertex++;
        model.indices[i++] = vertex;
    }

    return model;
}
#pragma endregion

Vector3 ToEulerAngles(DirectX::XMVECTOR q) {
    DirectX::XMFLOAT4* f4 = new DirectX::XMFLOAT4();
    DirectX::XMStoreFloat4(f4, q);

    Vector3 angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (f4->w * f4->x + f4->y * f4->z);
    double cosr_cosp = 1 - 2 * (f4->x * f4->x + f4->y * f4->y);
    angles.x = DirectX::XMConvertToDegrees(std::atan2(sinr_cosp, cosr_cosp));
    if (angles.x > 360.f)
    {
        angles.x -= 360.f;
    }
    else if (angles.x < 0.f)
    {
        angles.x += 360.f;
    }

    // pitch (y-axis rotation)
    double sinp = std::sqrt(1 + 2 * (f4->w * f4->y - f4->x * f4->z));
    double cosp = std::sqrt(1 - 2 * (f4->w * f4->y - f4->x * f4->z));
    angles.y = DirectX::XMConvertToDegrees(2 * std::atan2(sinp, cosp) - PI / 2);
    if (angles.y > 360.f)
    {
        angles.y -= 360.f;
    }
    else if (angles.y < 0.f)
    {
        angles.y += 360.f;
    }

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (f4->w * f4->z + f4->x * f4->y);
    double cosy_cosp = 1 - 2 * (f4->y * f4->y + f4->z * f4->z);
    angles.z = DirectX::XMConvertToDegrees(std::atan2(siny_cosp, cosy_cosp));
    if (angles.z > 360.f)
    {
        angles.z -= 360.f;
    }
    else if (angles.z < 0.f)
    {
        angles.z += 360.f;
    }

    return angles;
}

Vector4 QuatToF4(DirectX::XMVECTOR q)
{
    DirectX::XMFLOAT4* f4 = new DirectX::XMFLOAT4();
    DirectX::XMStoreFloat4(f4, q);

    return Vector4{ f4->w, f4->x, f4->y, f4->z };
}

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

    ResetState();
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
    return mThickness;
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
    mThickness = val;
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

    // maxDepth 판단 - depthVector 만들기 위함
    int maxDepth = 0, tempDepth = 0;
    for (int i = 0; i < size; i++)
    {
        std::string cur = mWord[i].GetLetter();
        if (cur == "[")
        {
            tempDepth++;
            maxDepth = max(tempDepth, maxDepth);
        }
        if (cur == "]")
            tempDepth--;
    }

    std::vector<std::vector<int>> depthVector(maxDepth + 1, std::vector<int>());
    // index 마다 각 depth, index 바로 찾아가게 해주는 vector
    std::vector<std::pair<int, int>> locations(size, std::make_pair(-1, -1));
    // a < b > c 에서 a 가 위치한 index 를 담은 vector
    std::vector<int> leftContext(size, -1);
    // a < b > c 에서 c 를 전부 담은 vector
    // TODO - 다른 Depth의 After 가 있는 Test Case?
    std::vector<LLetter> rightContext(size);

    // 1. 파싱하면서 Location, LeftContext, RightContext 정보 생성
    int currentDepth = 0, count = 0;
    for (int i = 0; i < size; i++)
    {
        LLetter& cur = mWord[i];
        std::string curString = cur.GetLetter();

        if (curString == "[")
        {
            currentDepth++;
            continue;
        }
        if (curString == "]")
        {
            depthVector[currentDepth].clear();
            currentDepth--;
            continue;
        }

        // TODO - 만약 Parametric L-Letter가 Ignore라면?
        if (mIgnores.find(curString[0]) != mIgnores.end())
            continue;

        count++;

        // 정보 업데이트
        depthVector[currentDepth].emplace_back(i);
        locations[i].first = currentDepth;
        locations[i].second = depthVector[currentDepth].size() - 1;

        // Left-Context 넣어주기
        int leftDepth = currentDepth;
        if (count != 1 && depthVector[0].size() > 0)
        {
            // 각 depth의 0번 index 친구라면 위쪽 depth에서 찾아야 함
            if (locations[i].second == 0)
            {
                do
                {
                    leftDepth--;
                } while (leftDepth >= 0 && depthVector[leftDepth].size() == 0);

                if (leftDepth >= 0)
                    leftContext[i] = depthVector[leftDepth][depthVector[leftDepth].size() - 1];
            }
            // 각 depth의 0번 index 친구가 아니라면 같은 depth에서 찾으면 됨
            else
                leftContext[i] = depthVector[leftDepth][locations[i].second - 1];

            // 역참조로 right Context Info도 채워주기 - 단, 같은 depth 일 때만
            if (locations[i].second != 0)
                rightContext[leftContext[i]] = mWord[i];
        }
    }

    // 2. 파싱하며 LRule 적용
    currentDepth = 0;
    for (int i = 0; i < size; i++)
    {
        LLetter& cur = mWord[i];
        std::string curString = cur.GetLetter();

        // 1. 괄호
        if (curString == "[" || curString == "]")
        {
            iterated.emplace_back(cur);
            continue;
        }

        // 2. mIgnore에 걸리는 모든 문자
        // TODO - 만약 Parametric L-Letter가 Ignore라면?
        if (mIgnores.find(curString[0]) != mIgnores.end())
        {
            auto after = mRules[curString].GetAfter(NULL, NULL, cur);
            if (after.size() > 0)
            {
                for (auto& letter : after)
                    iterated.emplace_back(letter);
            }
            else
                iterated.emplace_back(cur);

            continue;
        }

        // 3. Context-Sensitive, Context-Free 전부
        LLetter left;
        LLetter right;
        std::vector<LLetter> letterAfter;
        if (leftContext[i] != -1)
            left = mWord[leftContext[i]];

        right = rightContext[i];

        letterAfter = mRules[cur.GetFormat()].GetAfter(left, right, cur);

        if (letterAfter.size() > 0)
        {
            for (auto& letter : letterAfter)
                iterated.emplace_back(letter);
        }
        else
            iterated.emplace_back(cur);
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

    ResetState();

    float width = 0.5;
    std::stack<StateInfo> ss;
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
            if (letter.IsParametic())
            {
                std::vector<std::string> params = letter.GetParameters();
                float distance = std::stof(params[0]);
                float thickness = params.size() > 1
                    ? std::stof(params[1])
                    : mState.thickness;

                this->MoveParam(distance);
                endPos = mState.position;
                out->push_back(
                    CreateCylinder(startPos, endPos, mState.quaternion,
                        thickness, distance, 50));
            }
            else
            {
                // Draw + Move forward
                this->Move();
                endPos = mState.position;
                out->push_back(
                    CreateCylinder(startPos, endPos, mState.quaternion,
                        mState.thickness, mDistance, 50));
            }
            startPos = mState.position;
            break;
        }
        case LLetter::Type::NoDrawForward: {
            this->Move();
            startPos = mState.position;
            break;
        }
        case LLetter::Type::NoDrawForward2:
        {
            // No Draw + Move foward
            if (letter.IsParametic())
            {
                std::vector<std::string> params = letter.GetParameters();
                float distance = std::stof(params[0]);
                float thickness = params.size() > 1
                    ? std::stof(params[1])
                    : 0.3f;

                this->MoveParam(distance);
                startPos = mState.position;
            }
            else
            {
                this->Move();
                startPos = mState.position;
                //out->push_back(CreateLineModel(startPos, endPos));
            }

            break;
        }
        case LLetter::Type::MakeLeaf: // Leaf Model Set 1
        {
            Vector4 flowerColor = { 181.0f / 256.0f, 69.0f / 256.0f, 89.0f / 256.0f, 0.0f };
            if (letter.IsParametic())
            {
                std::vector<std::string> params = letter.GetParameters();
                int type = std::stof(params[0]);

                // TODO - 크기 지원
                float scale = params.size() > 1 ?
                    std::stof(params[1])
                    : 1.0f;
                if (1 <= type && type <= 3)
					LoadModel(GetModelPath(type), out, scale, flowerColor);
                else if (type >= 4)
                {
                    // 자체 보정
                    DirectX::XMVECTOR quat = DirectX::XMQuaternionMultiply(
                        mState.quaternion,
                        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f));

                    out->push_back(CreateLeafPreset(startPos, quat, 2, scale * 2.3f));
                }
            }
            else
            {
                LoadModel(GetModelPath(1), out, 1.0f, flowerColor);
            }

            break;
        }
        case LLetter::Type::MakeLeaf2: // Leaf Model Set 2
        {
            Vector4 flowerColor = { 181.0f / 256.0f, 69.0f / 256.0f, 89.0f / 256.0f, 0.0f };
            if (letter.IsParametic())
            {
                std::vector<std::string> params = letter.GetParameters();
                int type = std::stof(params[0]);

                // TODO - 크기 지원
                float scale = params.size() > 1 ?
                    std::stof(params[1])
                    : 1.0f;
                LoadModel(GetModelPath(type), out, scale, flowerColor);
            }

            break;
        }
        case LLetter::Type::UseLeafPreset:
        {
            Vector3 position = startPos;
            if (letter.IsParametic())
            {
                std::vector<std::string> params = letter.GetParameters();
                int type = std::stoi(params[0]);
                float scale = params.size() > 1 ?
                    std::stof(params[1])
                    : 1.0f;

                out->push_back(CreateLeafPreset(position, mState.quaternion, type, scale));
            }
            else
            {
                out->push_back(CreateLeafPreset(position, mState.quaternion, 1, 1.0f));
            }
            break;
        }
        case LLetter::Type::RollLeft:
        {
            if (letter.IsParametic())
            {
                this->Rotate(0, std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // mAngleChange 만큼 x축 회전
                if (mDrawingLeaf) {
                    this->Rotate(0, mLeafAngleChange);
                }
                else {
                    this->Rotate(0, mAngleChange);
                }
            }
            break;
        }
        case LLetter::Type::RollRight:
        {
            if (letter.IsParametic())
            {
                this->Rotate(0, -1 * std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // -mAngleChange 만큼 x축 회전
                if (mDrawingLeaf) {
                    this->Rotate(0, -mLeafAngleChange);
                }
                else {
                    this->Rotate(0, -mAngleChange);
                }
            }
            break;
        }
        case LLetter::Type::PitchUp:
        {
            if (letter.IsParametic())
            {
                this->Rotate(1, std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // mAngleChange 만큼 y축 회전
                if (mDrawingLeaf) {
                    this->Rotate(1, mLeafAngleChange);
                }
                else {
                    this->Rotate(1, mAngleChange);
                }
            }
            break;
        }
        case LLetter::Type::PitchDown:
        {
            if (letter.IsParametic())
            {
                this->Rotate(1, -1 * std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // -mAngleChange 만큼 y축 회전
                if (mDrawingLeaf) {
                    this->Rotate(1, -mLeafAngleChange);
                }
                else {
                    this->Rotate(1, -mAngleChange);
                }
            }
            break;
        }
        case LLetter::Type::TurnLeft:
        {
            if (letter.IsParametic())
            {
                this->Rotate(2, std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // mAngleChange 만큼 z축 회전
                //this->Rotate(2, mAngleChange);

                if (mDrawingLeaf) {
                    this->Rotate(2, mLeafAngleChange);
                }
                else {
                    this->Rotate(2, mAngleChange);
                }
            }
            break;
        }
        case LLetter::Type::TurnRight:
        {
            if (letter.IsParametic())
            {
                this->Rotate(2, -1 * std::stof(letter.GetParameters()[0]));
                break;
            }
            else
            {
                // -mAngleChange 만큼 z축 회전
                //this->Rotate(2, -mAngleChange);

                if (mDrawingLeaf) {
                    this->Rotate(2, -mLeafAngleChange);
                }
                else {
                    this->Rotate(2, -mAngleChange);
                }
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
            ss.push({ mState, mLeafDirection });
            break;
        }
        case LLetter::Type::Pop:
        {
            // 이전 State 복원
            // 위치도 같이 옮겨지는 경우 No draw
            mState = ss.top().treeState;
            mLeafDirection = ss.top().leafDirection;
            ss.pop();
            startPos = mState.position;
            break;
        }
        case LLetter::Type::StartingPoint:
        {
            leaf = new std::vector<Vector3>();

            //leaf->push_back(mState.position);

            //mDrawingLeaf = true;

            leafstack.push(leaf);
            break;
        }
        case LLetter::Type::MarkingPoint:
        {
            if (leaf->size() == 0 || leaf->at(leaf->size() - 1) != mState.position)
                leaf->push_back(mState.position);

            break;
        }
        case LLetter::Type::EndingPoint:
        {
            if (leaf->size() >= 3)
                out->push_back(CreateLeafSegment(leaf));

            if (!leafstack.empty()) {
                leaf = leafstack.top();
                leafstack.pop();
            }
            else {
                leaf = nullptr;
            }

            //mDrawingLeaf = false;
            //mLeafDirection = { 0.0f, 1.0f, 0.0f };

            break;
        }
        case LLetter::Type::Tilt:
        {
            if (letter.IsParametic())
                this->Rotate(3, std::stof(letter.GetParameters()[0]));
            else
                this->Rotate(3, 90.0f);
            break;
        }
        case LLetter::Type::None:
        {
            break;
        }
        }
    }
}

void LSystem::GetResultUEInfos(std::vector<UEModel>* models)
{
    if (mWord.size() < 1)
    {
        return;
    }

    ResetState();

    float forwardDistance = 100.f;
    SetDistance(forwardDistance);
    std::stack<State> s;
    Vector3 startPos;
    Vector3 endPos;

    for (const LLetter& letter : mWord)
    {
        switch (letter.GetType())
        {
            case LLetter::Type::None:
            {
                break;
            }
#pragma region rotate
            // 기존 symbol 동작 유지 ue에 맞게 수정 (기존 모델 재사용)
            // 이전에 원래 pitch up down을 ^, &으로 썼었으나 회전 변환 수정하면서 roll 로 바뀜
            case LLetter::Type::RollLeft:
            {
                // "\"
                // ue 기준 pitch down
                // \(60) -> y(pitch) -60
                if (letter.IsParametic())
                {
                    this->Rotate(2, std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(2, mAngleChange);
                }

                break;
            }
            case LLetter::Type::RollRight:
            {
                // "/"
                // ue 기준 pitch up
                // /(60) -> y(pitch) 60
                if (letter.IsParametic())
                {
                    this->Rotate(2, -1 * std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(2, -mAngleChange);
                }

                break;
            }
            case LLetter::Type::PitchUp:
            {
                // "^"
                // ue 기준 yaw +
                // ^(30) -> z(yaw) 30
                if (letter.IsParametic())
                {
                    this->Rotate(1, -1 * std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(1, -mAngleChange);
                }

                break;
            }
            case LLetter::Type::PitchDown:
            {
                // "&"
                // ue   기준 yaw -
                // &(30) -> z(yaw) -30
                if (letter.IsParametic())
                {
                    this->Rotate(1, std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(1, mAngleChange);
                }

                break;
            }
            case LLetter::Type::TurnLeft:
            {
                // "+"
                // ue 기준 roll
                // -(30) -> x(roll) 30
                if (letter.IsParametic())
                {
                    this->Rotate(0, std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(0, mAngleChange);
                }

                break;
            }
            case LLetter::Type::TurnRight:
            {
                // "-"
                // ue 기준 roll
                // +(30) -> x(roll) -30
                if (letter.IsParametic())
                {
                    this->Rotate(0, -1 * std::stof(letter.GetParameters()[0]));
                }
                else
                {
                    this->Rotate(0, -1 * mAngleChange);
                }

                break;
            }
            case LLetter::Type::TurnAround:
            {
                // !!! DONT USE
                this->Rotate(2, 180.f);

                break;
            }
#pragma endregion
            case LLetter::Type::Push:
            {
                s.push(mState);

                break;
            }
            case LLetter::Type::Pop:
            {
                mState = s.top();
                s.pop();
                startPos = mState.position;

                break;
            }
            case LLetter::Type::Forward:
            {
                float distance;
                float thickness;
                // Trunk
                if (letter.IsParametic())
                {
                    std::vector<std::string> params = letter.GetParameters();
                    distance = std::stof(params[0]) * forwardDistance;
                    thickness = params.size() > 1
                        ? std::stof(params[1])
                        : mState.thickness;

                    MoveParam(distance);
                }
                else
                {
                    thickness = mState.thickness;

                    Move();
                }

                endPos = mState.position;
                
                UEModel model = UEModel();
                model.type = 1;
                model.position = (endPos + startPos) * 0.5f;
                Rotate(0, -90.f);
                model.rotation = QuatToF4(mState.quaternion);
                Rotate(0, 90.f);

                model.scale = Vector3(thickness, thickness, distance);

                models->push_back(model);

                startPos = mState.position;

                break;
            }
            case LLetter::Type::NoDrawForward:
            {
                this->Move();
                endPos = mState.position;

                break;
            }
            case LLetter::Type::MakeLeaf: // Leaf Model Set 1
            {
                // Leaf 이후 이동 X
                float distance;

                Move();     // UE 내 SetDistance()로 offset 맞춤 (고정된 크기)
                endPos = mState.position;

                UEModel model = UEModel();
                model.type = 2;
                model.position = (endPos + startPos) * 0.5f;
                Rotate(0, -90.f);
                model.rotation = QuatToF4(mState.quaternion);
                Rotate(0, 90.f);
                //model.scale = Vector3(thickness, thickness, distance);

                models->push_back(model);

                startPos = mState.position;

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
    ResetConstant();

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
        if (inp == "constant")
        {
            while (ifs >> inp)
            {
                if (inp == "end")
                {
                    break;
                }

                std::vector<std::string> constantInfo = split(inp, ':');
                AddConstant(constantInfo[0], constantInfo[1]);
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

void LSystem::MoveParam(const float& distance)
{
    // Heading Vector에 distance 곱해서 움직여주기
    mState.position.x += mState.direction.x * distance;
    mState.position.y += mState.direction.y * distance;
    mState.position.z += mState.direction.z * distance;
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
    case 3: // only for $ symbol
    {
        DirectX::XMFLOAT3 axisHead;
        axisHead.x = x, axisHead.y = y, axisHead.z = z;
        mState.quaternion = DirectX::XMQuaternionMultiply(
            mState.quaternion,
            DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisHead), rad));
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

void LSystem::ResetState()
{
    mState =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * PI / 180.0f),
        mThickness
    };
}