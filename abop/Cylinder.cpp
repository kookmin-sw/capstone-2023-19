#include <cmath>

#include "Stdafx.h"
#include "CommonVariable.hpp"
#include "ModelClass.hpp"
#include "Cylinder.hpp"

// 반지름이 radius일 때 n번째 segment 꼭지점 좌표 (높이는 z)
Vector3 CalCylinderVertexPos(float radius, int n, int segment, float z = 0.0f)
{
    float ratio = (float)n / segment;
    float rad = ratio * PI * 2;
    
    float x = std::cos(rad) * radius;
    float y = std::sin(rad) * radius;

    return Vector3 { x, y, z };
}

// !!! 테스트용 나중에 지우기
bool Cylinder::GenerateCylinderCap(ID3D11Device* device)
{
    float top = -this->height_ * 0.5f;
    float bottom = this->height_ * 0.5f;

    return this->GenerateCylinderCap(device, top, bottom);
}

bool Cylinder::GenerateCylinderCap(ID3D11Device* device, float top, float bottom)
{
    this->vertexCount_ = this->segment_ * 2 + 2;
    this->indexCount_ = this->segment_ * 6;
    // lighting이 어색하지 않도록 별도의 cap 모델을 생성하여 buffer 생성
    
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
    vertices[0].position = DirectX::XMFLOAT3
    (
        0.0f,
        0.0f,
        top
    );
    vertices[this->segment_ + 1].position = DirectX::XMFLOAT3
    (
        0.0f,
        0.0f,
        bottom
    );
    vertices[0].color = this->color_;
    vertices[this->segment_ + 1].color = this->color_;

    for (int i = 1; i < this->segment_ + 1; i++)
    {
        // ex. vertex[1]은 top cap 우측 기준 반 시계 방향 0번째(i - 1)
        Vector3 vt = CalCylinderVertexPos(this->radius_, i - 1, this->segment_);

        // top cap 
        // cap mid (0) + cap side ( 1 ~ segment )  
        vertices[i].position = DirectX::XMFLOAT3
        (
			vt.x,
            vt.y,
            top
        );

        // bottom cap
        // cap mid (segment + 1) + cap side ( segment + 2 ~ segment * 2 + 1)
        vertices[i + this->segment_ + 1].position = DirectX::XMFLOAT3
        (
			vt.x,
            vt.y,
            bottom
        );

        vertices[i].color = this->color_;
        vertices[i + this->segment_ + 1].color = this->color_;
    }
    
    // Index
    int it = 0;     // top mid
    int ib = this->segment_ + 1;
    int diff = this->segment_ * 3;

    for (int i = 0; i < this->segment_; i++)
    {
        int n = i * 3;

        // top (시계)
        indices[n] = it;
        indices[n + 1] = i + 2 > this->segment_
            ? 1
            : i + 2;
        indices[n + 2] = i + 1;

        // bottom (반시계)
        indices[diff + n] = ib;
        indices[diff + n + 1] = this->segment_ + i + 2;
        indices[diff + n + 2] = this->segment_ + i + 3 > this->segment_ * 2 + 1
            ? ib + 1
            : this->segment_ + i + 3;
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
    float top = -this->height_ * 0.5f;
    float bottom = this->height_ * 0.5f;
    //if (!this->GenerateCylinderCap(device, top, bottom))
    //{
    //    // Cap 먼저 생성
    //    // !!! 임시로 따로 처리
    //    return false;
    //}
    
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

    for (int i = 0; i < this->segment_; i++)
    {
        // vertex
        Vector3 vt = CalCylinderVertexPos(this->radius_, i, this->segment_);

        vertices[i * 2].position = DirectX::XMFLOAT3
        (
            vt.x,
            vt.y,
            top
        );

        vertices[i * 2 + 1].position = DirectX::XMFLOAT3
        (
            vt.x,
            vt.y,
            bottom
        );

        // color
        vertices[i * 2].color = this->color_;
        vertices[i * 2 + 1].color = this->color_;
    }

    // Index
    for (int i = 0; i < this->segment_; i++)
    {
        int n = i * 6;

        int a = i * 2;
        int b = (i * 2 + 2) % (this->segment_ * 2);
        int c = (i * 2 + 3) % (this->segment_ * 2);
        int d = i * 2 + 1;

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

void Cylinder::SetHeight(float height)
{
    this->height_ = height;
}

void Cylinder::SetRadius(float radius)
{
    this->radius_ = radius;
}