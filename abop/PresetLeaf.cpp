#include "Stdafx.h"
#include "ModelClass.hpp"
#include "PresetLeaf.hpp"
#include <vector>

bool PresetLeaf::Initialize(ID3D11Device* device, Model model)
{
    return this->InitializeBuffers(device, model);
}

bool PresetLeaf::InitializeBuffers(ID3D11Device* device, Model model)
{
    // vertexCount, indexCount 설정
    this->vertexCount_ = mType * 2 + 2;
    this->indexCount_ = this->vertexCount_ * 6 - 12;

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

    // Type에 따라 vertexCount 설정
    switch (mType)
    {
        case 1:
        {
            vertices[0].position = { 0.0f, 0.0f, 0.0f };
            vertices[0].color = { 0.19f, 0.35f, 0.15f, 0.0f };

            vertices[1].position = { -0.191315f * mScale, 0.0f, 0.46195f * mScale };
            vertices[1].color = { 0.19f, 0.35f, 0.15f, 0.0f };

            vertices[2].position = { 0.0f, 0.0f, 1.0f * mScale };
            vertices[2].color = { 0.19f, 0.35f, 0.15f, 0.0f };

            vertices[3].position = { 0.191315f * mScale, 0.0f, 0.46195f * mScale };
            vertices[3].color = { 0.19f, 0.35f, 0.15f, 0.0f };
            break;
        }
        // TODO : 나머지 Type의 Leaf Preset 추가 
    }
    
    // Type에 따라 indexCount 설정
    switch (mType)
    {
        case 1:
        {
            indices[0] = 0, indices[1] = 1, indices[2] = 2;
            indices[3] = 0, indices[4] = 2, indices[5] = 1;
            indices[6] = 0, indices[7] = 2, indices[8] = 3;
            indices[9] = 0, indices[10] = 3, indices[11] = 2;
            break;
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

void PresetLeaf::SetType(const int& type) 
{
    mType = type;
}

void PresetLeaf::SetScale(const float& scale)
{
    mScale = scale;
}