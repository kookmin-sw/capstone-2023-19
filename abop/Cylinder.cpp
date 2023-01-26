#include <cmath>

#include "Stdafx.h"
#include "CommonVariable.hpp"
#include "ModelClass.hpp"
#include "Cylinder.hpp"

// 반지름이 radius일 때 n번째 segment 꼭지점 좌표 (높이는 z)
Vector3 CalCylinderVertexPos(float radius, int n, int segment, float z = 0.0f)
{
    float ratio = (float)n / (segment - 1);
    float rad = ratio * PI * 2;
    
    float x = std::cos(rad) * radius;
    float y = std::sin(rad) * radius;

    return Vector3 { x, y, z };
}

bool GenerateCylinderCap(Vector3 pos, DirectX::XMFLOAT4 color, float radius, int n, int segment, float top, float bottom)
{
    this->vertexCount_ = segment * 2 + 2;
    this->indexCount_ = segment * 6;
    // lighting이 어색하지 않도록 별도의 cap 모델을 생성하여 buffer 생성
    // 헷갈리지 않게 CalCylinderVertexPos()와 매개변수를 비슷하게 사용
    
    // 정적 배열 생성
    VertexType* vertices = new VertexType[this->vertexCount_];
    if (!vertices)
    {
        return false;
    }

    unsigned long* indices = new unsigned long[this->indexCount_];
    if (!indices)
    {
        return false;
    }

    // Vertex
    // cap mid - 중앙 (top, bottom)
    // 현재 pos에서 top, bottom만 다름
    vertices[0] = DirectX::XMFLOAT3
    (
        pos.x,
        pos.y,
        top
    );
    vertices[segment + 1] = DirectX::XMFLOAT3
    (
        pos.x,
        pos.y,
        bottom
    );
    vertices[0].color = color;
    vertices[1].color = color;

    for (int i = 1; i < segment + 1; i++)
    {
        // ex. vertex[1]은 top cap 우측 기준 반 시계 방향 0번째(i - 1)
        Vector3 vt = CalCylinderVertexPos(radius, i - 1, segment);

        // top cap 
        // cap mid (0) + cap side ( 1 ~ segment )  
        vertices[i] = DirectX::XMFLOAT3
        (
            pos.x + vt.x,
            pos.y + vt.y,
            top,
        );
        
        // bottom cap
        // cap mid (segment + 1) + cap side ( segment + 2 ~ segment * 2 + 1)
        vertices[i + segment + 1] = DirectX::XMFLOAT3
        (
            pos.x + vt.x,
            pos.y + vt.y,
            bottom
        );

        vertices[i].color = color;
        vertices[i + segment + 1].color = color;
    }
    
    // Index
    for (int i = 1; i < segment; i++)
    {
        int n = (i - 1) * 3;

        // top
        indices[n] = 0;
        indices[n + 1] = i + 1;
        indices[n + 2] = i

        // bottom
        indices[n + segment + 1] = segment + 1;
        indices[n + segment + 2] = i + segment + 2;
        indices[n + segment + 3] = i + segment + 1;
    }

    // 정적 vertex buffer 생성
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->vertexCount_;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 vertex buffer data에 대한 포인터를 제공
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Vertex Buffer 생성
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(this->vertexBuffer_))))
    {
        return false;
    }

    // 정적 Index Buffer 설정
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount_;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 index buffer data에 대한 포인터를 제공
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(this->indexBuffer_))))
    {
        // Index Buffer 생성
        return false;
    }

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

bool Cylinder::Initialize(ID3D11Device* device)
{
    return this->InitializeBuffers(device);
}

bool Cylinder::InitializeBuffers(ID3D11Device* device)
{
    float top = this->position_ - this->height_ * 0.5f;
    float bottom = this->position_ + this->height_ * 0.5f;
    if (GenerateCylinderCap())
    {
        // Cap 먼저 생성
        return false;
    }

    // Cylinder buffer, index count 설정
    this->vertexCount_ = this->segment_ * 2;
    this->indexCount_ = this->segment_ * 6;
    
    // 정적 배열 생성
    VertexType* vertices = new VertexType[this->vertexCount_];
    if (!vertices)
    {
        return false;
    }

    unsigned long* indices = new unsigned long[this->indexCount_];
    if (!indices)
    {
        return false;
    }

    for (int i = 0; i < this->segment_ * 2; i += 2)
    {
        // vertex
        Vector3 vt = CalCylinderVertexPos(this->radius_, i, this->segment_);

        vertices[i].position = DirectX::XMFLOAT3
        (
            this->position_.x + vt.x,
            this->position_.y + vt.y,
            this->position_.z + top
        );

        vertices[i + 1].position = DirectX::XMFLOAT3
        (
            this->position_.x + vt.x,
            this->position_.y + vt.y,
            this->position_.z + bottom
        );

        // color
        vertices[i].color = this->color_;
        vertices[i + 1].color = this->color_;
    }

    // Index
    for (int i = 0; i < this->segment_; i++)
    {
        int n = i * 6;  // i번째 측면 index 6개

        int a = i;
        int b = i + 2;
        int c = i + 3;
        int d = i + 1;

        indices[n] = a;
        indices[n + 1] = b;
        indices[n + 2] = c;

        indices[n + 3] = c;
        indices[n + 4] = d;
        indices[n + 5] = a;
    }

    // 정적 vertex buffer 생성
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->vertexCount_;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 vertex buffer data에 대한 포인터를 제공
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Vertex Buffer 생성
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(this->vertexBuffer_))))
    {
        return false;
    }

    // 정적 Index Buffer 설정
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount_;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 index buffer data에 대한 포인터를 제공
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(this->indexBuffer_))))
    {
        // Index Buffer 생성
        return false;
    }

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void Cylinder::SetSegment(int segment)
{
    this->segment_ = segment;
}

void Cylinder::Setheight(float height)
{
    this->height_ = height;
}

void Cylinder::SetRadius(float radius)
{
    this->radius_ = radius;
}