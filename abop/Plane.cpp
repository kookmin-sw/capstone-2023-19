#include "Stdafx.h"
#include "ModelClass.hpp"
#include "Plane.hpp"

bool Plane::Initialize(ID3D11Device* device)
{
    return this->InitializeBuffers(device);
}

bool Plane::InitializeBuffers(ID3D11Device* device)
{
    if (this->nWidthSegments_ < 2 || this->nHeightSegments_ < 2)
    {
        throw "Invalid nSegments";
    }

    // Square buffer index 수 설정
    this->vertexCount_ = this->nWidthSegments_ * this->nHeightSegments_;
    this->indexCount_ = (this->nWidthSegments_ - 1) * (this->nHeightSegments_ - 1) * 6;
    
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

    float hWidth = this->width_ / 2;
    float hHeight = this->height_ / 2;

    float winv = 1.0 / (this->nWidthSegments_ - 1);
    float hinv = 1.0 / (this->nHeightSegments_ - 1);

    for (int y = 0; y < nHeightSegments_; y++)
    {
        for (int x = 0; x < nWidthSegments_; x++)
        {
            int index = y * this->nWidthSegments_ + x;

            vertices[index].position = DirectX::XMFLOAT3
            {
                this->position_.x - hWidth + winv * x * this->width_,
                this->position_.y + hHeight - hinv * y * this->height_,
                this->position_.z
            };

            vertices[index].color = this->color_;
        }
    }

    int iBufferIndex = 0;
    for (int y = 0; y < nHeightSegments_ - 1; y++)
    {
        for (int x = 0; x < nWidthSegments_ - 1; x++)
        {
            int index = y * this->nWidthSegments_ + x;

            int a = index;
            int b = index + 1;
            int c = index + 1 + this->nWidthSegments_;
            int d = index + this->nWidthSegments_;

            indices[iBufferIndex++] = a;
            indices[iBufferIndex++] = b;
            indices[iBufferIndex++] = c;
            indices[iBufferIndex++] = c;
            indices[iBufferIndex++] = d;
            indices[iBufferIndex++] = a;
        }
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

void Plane::SetWidth(const float& width)
{
    this->width_ = width;
}

void Plane::SetHeight(const float& height)
{
    this->height_ = height;
}

void Plane::SetWidthSegments(const int& n)
{
    this->nWidthSegments_ = n;
}

void Plane::SetHeightSegments(const int& n)
{
    this->nHeightSegments_ = n;
}