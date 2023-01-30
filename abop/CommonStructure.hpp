#pragma once

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
    //  a
    // b  c

    // a  b
    // c  d
    int vertexCount;
    VertexType vertexTypes[4];
};
