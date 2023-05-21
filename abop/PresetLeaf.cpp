#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Stdafx.h"
#include "ModelClass.hpp"
#include "Utils.hpp"
#include "PresetLeaf.hpp"

#include <iostream>

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

    // Vertex, Index 정보 담긴 file 불러오기
    std::string fileName = "data/preset_leaf/" + std::to_string(mType) + ".txt";
    std::ifstream file(fileName);

    // vertex 정보, index 정보 설정
    std::string line;
    float x, y, z;
    int a, b, c;
    if (file.is_open())
    {
        for (int i = 0; i < this->vertexCount_; i++)
        {
	        if (std::getline(file, line))
	        {
                std::vector<std::string> temp = split(line, ' ');
                x = std::stof(temp[0]) * mScale;
                y = std::stof(temp[1]) * mScale;
                z = std::stof(temp[2]) * mScale;

                vertices[i].position = { x, y, z };
                vertices[i].color = { 0.19f, 0.35f, 0.15f, 0.0f };
	        }
        }

        for (int i = 0; i < this->indexCount_; i += 3)
        {
	        if (std::getline(file, line))
	        {
                std::vector<std::string> temp = split(line, ' ');
                a = std::stoi(temp[0]);
                b = std::stoi(temp[1]);
                c = std::stoi(temp[2]);

                indices[i] = a;
                indices[i + 1] = b;
                indices[i + 2] = c;
	        }
        }

        file.close();
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