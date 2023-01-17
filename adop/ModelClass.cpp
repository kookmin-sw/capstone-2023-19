#include "Stdafx.h"
#include "ModelClass.hpp"

ModelClass::ModelClass()
{

}

ModelClass::ModelClass(const ModelClass& other)
{
    
}

ModelClass::~ModelClass()
{
    
}

bool ModelClass::Initialize(ID3D11Device* device, const Model& model)
{
    // vertex 및 인덱스 버퍼 초기화
    return this->InitializeBuffers(device, model);
}

void ModelClass::Shutdown()
{
    this->ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Render를 위해 그래픽 파이프 라인에 vertex와 index buffer 준비
    this->RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
    return this->indexCount_;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    // !!! TEMP
    this->vertexCount_ = 3;
    // 인덱스 배열의 인덱스 수를 설정
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

    // 정점 배열 데이터 설정
    vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

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

    delete [] vertices;
    vertices = nullptr;

    delete [] indices;
    indices = nullptr;

    return true;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device, const Model& model)
{
    // !!! 추후 개별적인 상속 클래스로 구현해도 좋을듯
    // vertexArr은 왼쪽 상단부터 시계 방향

    if (model.vertexCount == 3)
    {
        // Triple
        this->vertexCount_ = 3;
        this->indexCount_ = 3;
    }
    else if (model.vertexCount == 4)
    {
        // Quad
        // 인덱스 배열의 인덱스 수를 설정
        this->vertexCount_ = 4;
        this->indexCount_ = 6;
    }
    else
    {
        throw "Not supported vertices";
    }


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

    // 정점 배열 데이터 설정
    for (int i = 0; i < model.vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(
            model.vertexTypes[i].position.x,
            model.vertexTypes[i].position.y,
            model.vertexTypes[i].position.z
        );
        vertices[i].color = DirectX::XMFLOAT4(
            model.vertexTypes[i].color.w,
            model.vertexTypes[i].color.x,
            model.vertexTypes[i].color.y,
            model.vertexTypes[i].color.z
        );
    }

    // default5
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    if (model.vertexCount == 4)
    {
        // 직사각형인 경우
        indices[3] = 2;
        indices[4] = 3;
        indices[5] = 0;
    }

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

    delete [] vertices;
    vertices = nullptr;

    delete [] indices;
    indices = nullptr;

    return true;
}


void ModelClass::ShutdownBuffers()
{
    if (this->indexBuffer_)
    {
        this->indexBuffer_->Release();
        this->indexBuffer_ = nullptr;
    }

    if (this->vertexBuffer_)
    {
        this->vertexBuffer_->Release();
        this->vertexBuffer_ = nullptr;
    }
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // 정점 버퍼의 단위와 오프셋 설정
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // !!! Check
    deviceContext->IASetVertexBuffers(0, 1, &(this->vertexBuffer_), &stride, &offset);
    deviceContext->IASetIndexBuffer(this->indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
