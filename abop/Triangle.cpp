#include "Stdafx.h"
#include "ModelClass.hpp"
#include "Triangle.hpp"

bool Triangle::Initialize(ID3D11Device* device)
{
    return this->InitializeBuffers(device);
}

bool Triangle::InitializeBuffers(ID3D11Device* device)
{
    this->vertexCount_ = 3;
    this->indexCount_ = 3;
    
    // 정점 배열 생성
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

    float hHeight = this->height_ / 2;
    float hWidth = this->width_ / 2;

    // Vertex
    vertices[0].position = DirectX::XMFLOAT3
    (
        this->position_.x,
        this->position_.y + hHeight,
        this->position_.z
    );

    vertices[1].position = DirectX::XMFLOAT3
    (
        this->position_.x + hWidth,
        this->position_.y - hHeight,
        this->position_.z
    );

    vertices[2].position = DirectX::XMFLOAT3
    (
        this->position_.x - hWidth,
        this->position_.y - hHeight,
        this->position_.z
    );

    // Color
    vertices[0].color = this->color_;
    vertices[1].color = this->color_;
    vertices[2].color = this->color_;

    // default5
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    // 정적 정점 버퍼 설정
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->vertexCount_;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource 구조에 vertex data에 대한 포인터를 제공
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(this->vertexBuffer_))))
    {
        // Vertex Buffer 생성
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

    // subresource 구조에 index data에 대한 포인터를 제공
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