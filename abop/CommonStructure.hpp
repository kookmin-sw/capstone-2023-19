#pragma once

enum ModelType
{
    // Same as Model.hpp
    Custom,
    Square,
    Plane,
    Cube,
    Cylinder
};

struct Vector3
{
    float x;
    float y;
    float z;
};

struct Vector4
{
    float w;
    float x;
    float y;
    float z;
};

struct State
{
    Vector3 position;
    Vector3 heading;
    // Vector3 up;             // 기준 좌표 heading과 수직
};

struct VertexType
{
    Vector3 position;
    Vector4 color;
};

struct Model
{
    ModelType modelType;
    int vertexCount;
    int indexCount;
    VertexType* vertexTypes;
    int* indices;

    // temp
    int dataCount;
    float* datas;
};
