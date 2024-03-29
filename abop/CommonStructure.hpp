#pragma once

#include "Vector3.hpp"
#include "DirectXMath.h"

struct Vector4      // !!! Vector4는 아직 struct임
{
    float w;
    float x;
    float y;
    float z;
};

struct State
{
    Vector3 position;
    Vector3 direction;    // 방향 단위 벡터
	DirectX::XMVECTOR quaternion;
    float thickness;
};

struct StateInfo
{
    State treeState;
    Vector3 leafDirection;
};


struct VertexType
{
    Vector3 position;
    Vector4 color;          
};

struct TextureVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texture;
    DirectX::XMFLOAT3 normalVector;
};

enum ModelType
{
    Custom,
    // Same as DX/Models/ModelVariation.cpp
    LeafModel,
    PresetLeafModel,
    TriangleModel,
    SquareModel,
    PlaneModel,
    CubeModel,
    CylinderModel
};

struct Model
{
    ModelType modelType = ModelType::Custom;

    // For custom
    int vertexCount;
    int indexCount;
    VertexType* vertexTypes = nullptr;
    TextureVertex* texVertexTypes = nullptr;
    int* indices = nullptr;

    // For LeafModel
    float angle;

    // For others
    int dataCount;
    float* data;
};