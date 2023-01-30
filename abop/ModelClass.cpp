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

bool ModelClass::Initialize(ID3D11Device* device, float* modelData)
{
    // vertex 및 인덱스 버퍼 초기화
    if (modelData)
    {
        // Only ModelClass
        return this->InitializeBuffers(device, modelData);
    }

    return this->InitializeBuffers(device);
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

// Get, Set
int ModelClass::GetIndexCount()
{
    return this->indexCount_;
}

void ModelClass::SetPosition(const float& x, const float& y, const float& z)
{
    this->position_ = { x, y, z };
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    // 자식 클래스에서 오버라이드

    return false;
}

bool ModelClass::InitializeBuffers(ID3D11Device*, float*)
{
    // to be update

    return false;
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
