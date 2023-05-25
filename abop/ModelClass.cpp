#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "TextureClass.hpp"
#include "ModelClass.hpp"

#include <fstream>

ModelClass::ModelClass()
{

}

ModelClass::ModelClass(const ModelClass& other)
{
    
}

ModelClass::~ModelClass()
{
    
}

bool ModelClass::Initialize(ID3D11Device* device, Model model)
{
    // vertex 및 인덱스 버퍼 초기화

    // Only ModelClass
    return this->InitializeBuffers(device, model);
}

//bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
//{
//    if (!InitializeBuffers(device))
//    {
//        return false;
//    }
//
//    return LoadTexture(device, deviceContext, textureFilename);
//}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
    if (!this->LoadModel(modelFilename))
    {
        return false;
    }

    if (!this->InitializeBuffers(device))
    {
        return false;
    }

    return this->LoadTexture(device, deviceContext, textureFilename);
}

void ModelClass::Shutdown()
{
    this->ReleaseTexture();

    this->ShutdownBuffers();

    this->ReleaseModel();
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

Vector3 ModelClass::GetPosition()
{
    return this->position_;
}

Vector3 ModelClass::GetRotation()
{
    return this->rotation_;
}

Vector3 ModelClass::GetScale()
{
    return this->scale_;
}

DirectX::XMFLOAT4 ModelClass::GetQuaternion()
{
    return this->quaternion_;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    if (this->texture_)
    {
        return this->texture_->GetTexture();
    }

    return NULL;
}

void ModelClass::SetPosition(const float& x, const float& y, const float& z)
{
    this->position_ = { x, y, z };
}

void ModelClass::SetPosition(const Vector3& position)
{
    this->position_ = position;
}

void ModelClass::SetRotation(const float& x, const float& y, const float& z)
{
    this->rotation_ = { x, y, z };
}

void ModelClass::SetRotation(const Vector3& rotation)
{
    this->rotation_ = rotation;
}

void ModelClass::SetQuaternion(const float& x, const float& y, const float& z, const float& w)
{
    this->quaternion_ = DirectX::XMFLOAT4(x, y, z, w);
}

void ModelClass::SetScale(const float& x, const float& y, const float& z)
{
    scale_.x = x;
    scale_.y = y;
    scale_.z = z;
}

void ModelClass::SetScale(const Vector3& scale)
{
    scale_ = scale;
}

void ModelClass::SetColor(const float& r, const float& g, const float& b, const float& a)
{
    this->color_ = DirectX::XMFLOAT4(r, g, b, a);
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
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
    
    for (int i = 0; i < this->vertexCount_; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(this->model_[i].x, this->model_[i].y, this->model_[i].z);
        vertices[i].texture = DirectX::XMFLOAT2(this->model_[i].tu, this->model_[i].tv);
        vertices[i].normal = DirectX::XMFLOAT3(this->model_[i].nx, this->model_[i].ny, this->model_[i].nz);
        vertices[i].color = this->color_;

        indices[i] = i;
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

bool ModelClass::InitializeBuffers(ID3D11Device* device, Model model)
{
    // buffer, index count 설정
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

        vertices[i].texture = DirectX::XMFLOAT2
        (
            model.texVertexTypes[i].texture.x,
            model.texVertexTypes[i].texture.y
        );

        vertices[i].normal = DirectX::XMFLOAT3
        (
            model.texVertexTypes[i].normalVector.x,
            model.texVertexTypes[i].normalVector.y,
            model.texVertexTypes[i].normalVector.z
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

    delete[] model.vertexTypes;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
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

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    this->texture_ = new TextureClass;
    if (!this->texture_)
    {
        return false;
    }

    return this->texture_->Initialize(device, deviceContext, filename);
}

void ModelClass::ReleaseTexture()
{
    if (this->texture_)
    {
        this->texture_->Shutdown();
        delete this->texture_;
        this->texture_ = nullptr;
    }
}

bool ModelClass::LoadModel(char* filename)
{
    // 파일 형식에 맞게 다시 수정 가능
    std::ifstream fin;
    fin.open(filename);

    if (fin.fail())
    {
        return false;
    }

    char input = 0;
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    fin >> this->vertexCount_;

    this->indexCount_ = this->vertexCount_;

    this->model_ = new ModelType[this->vertexCount_];
    if (!this->model_)
    {
        return false;
    }

    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    for (int i = 0; i < this->vertexCount_; i++)
    {
        fin >> this->model_[i].x >> this->model_[i].y >> this->model_[i].z;
        fin >> this->model_[i].tu >> this->model_[i].tv;
        fin >> this->model_[i].nx >> this->model_[i].ny >> this->model_[i].nz;
    }

    fin.close();

    return true;
}

void ModelClass::ReleaseModel()
{
    if (this->model_)
    {
        delete[] this->model_;
        this->model_ = nullptr;
    }
}