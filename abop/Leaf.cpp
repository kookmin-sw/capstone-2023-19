#include "Stdafx.h"
#include "ModelClass.hpp"
#include "Leaf.hpp"

bool Leaf::Initialize(ID3D11Device* device, Model model)
{
    return this->InitializeBuffers(device, model);
}

bool Leaf::InitializeBuffers(ID3D11Device* device, Model model)
{
    // Leaf buffer, index count 설정
    this->vertexCount_ = model.vertexCount;
    this->indexCount_ = model.indexCount;

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
    for (int i = 0; i < model.vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3
        (
            model.vertexTypes[i].position.x,
            model.vertexTypes[i].position.y,
            model.vertexTypes[i].position.z
        );

        vertices[i].color = DirectX::XMFLOAT4
        (
            model.vertexTypes[i].color.w,
            model.vertexTypes[i].color.x,
            model.vertexTypes[i].color.y,
            model.vertexTypes[i].color.z
        );
    }

    // Index
    indices = (unsigned long*)model.indices;

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