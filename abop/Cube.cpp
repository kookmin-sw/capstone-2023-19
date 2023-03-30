#include "Stdafx.h"
#include "ModelClass.hpp"
#include "Cube.hpp"

bool Cube::Initialize(ID3D11Device* device)
{
    return this->InitializeBuffers(device);
}

bool Cube::InitializeBuffers(ID3D11Device* device)
{
    // Cube buffer, index count 설정
    this->vertexCount_ = 8;
    this->indexCount_ = 36;
    
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

    float hWidth = this->size_.x / 2;
    float hDepth = this->size_.y / 2;
    float hHeight = this->size_.z / 2;

    // Vertex
    vertices[0].position = DirectX::XMFLOAT3(-hWidth, hDepth, -hHeight);
    vertices[1].position = DirectX::XMFLOAT3(hWidth, hDepth, -hHeight);
    vertices[2].position = DirectX::XMFLOAT3(hWidth, -hDepth, -hHeight);
    vertices[3].position = DirectX::XMFLOAT3(-hWidth, -hDepth, -hHeight);
    vertices[4].position = DirectX::XMFLOAT3(hWidth, hDepth, hHeight);
    vertices[5].position = DirectX::XMFLOAT3(-hWidth, hDepth, hHeight);
    vertices[6].position = DirectX::XMFLOAT3(-hWidth, -hDepth, hHeight);
    vertices[7].position = DirectX::XMFLOAT3(hWidth, -hDepth, hHeight);

    // Color
    for (int i = 0; i < 8; i++)
    {
        //vertices[i].color = this->color_;
        vertices[i].color = { 0.5f, 0.25f, 0.0f, 0.0f }; // Temp tree color
    }

    int cubeIndices[36] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        3, 2, 7, 7, 6, 3,
        2, 1, 4, 4, 7, 2,
        1, 0, 5, 5, 4, 1,
        0, 3, 6, 6, 5, 0
    };

    // Index
    for (int i = 0; i < 36; i++)
    {
        indices[i] = cubeIndices[i];
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

Vector3 Cube::GetSize()
{
    return this->size_;
}

void Cube::SetSize(const Vector3& size)
{
    this->size_ = size;
}

void Cube::SetSize(const float& width, const float& depth, const float& height)
{
    this->size_.x = width;
    this->size_.y = depth;
    this->size_.z = height;
}
