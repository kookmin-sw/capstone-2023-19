#include "Stdafx.h"
#include "TextureClass.hpp"
#include "BitmapClass.hpp"

BitmapClass::BitmapClass()
{

}

BitmapClass::BitmapClass(const BitmapClass& other)
{

}

BitmapClass::~BitmapClass()
{

}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
	WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	// 화면 크기 저장
	this->screenWidth_ = screenWidth;
	this->screenHeight_ = screenHeight;

	// 렌더링할 비트맵 픽셀 크기 저장
	this->bitmapWidth_ = bitmapWidth;
	this->bitmapHeight_ = bitmapHeight;

	// 이전 렌더링 위치를 초기화
	this->previousPosX_ = -1;
	this->previousPosY_ = -1;

	// vertex, index 버퍼 초기화
	if (!this->InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스처 로드
	return this->LoadTexture(device, textureFilename);
}

void BitmapClass::Shutdown()
{
	this->ReleaseTexture();

	this->ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// 화면의 다른 위치로 렌더링하기 위해 동적 vertex 버퍼를 다시 빌드
	if (!this->UpdateBuffers(deviceContext, positionX, positionY))
	{
		return false;
	}

	this->RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return this->indexCount_;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return this->texture_->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수와 인덱스 배열의 인덱스 수를 지정합니다.
	this->indexCount_ = 6;
	this->vertexCount_ = 6;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[this->vertexCount_];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * this->vertexCount_));

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[this->indexCount_];
	if (!indices)
	{
		return false;
	}

	// 데이터로 인덱스 배열을 로드합니다.
	for (int i = 0; i < this->indexCount_; i++)
	{
		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->vertexCount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer_)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount_;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer_)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
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

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// 렌더링 위치가 변경된 경우에만 vertex buffer 업데이트
	if ((positionX == this->previousPosX_) && (positionY == this->previousPosY_))
	{
		return true;
	}

	// 렌더링 위치 업데이트
	this->previousPosX_ = positionX;
	this->previousPosY_ = positionY;

	left = (float)((this->screenWidth_ * 0.5) * -1) + (float)positionX;
	right = left + (float)this->bitmapWidth_;

	top = (float)(this->screenHeight_ * 0.5) - (float)positionY;
	bottom = top - (float)this->bitmapHeight_;

	vertices = new VertexType[this->vertexCount_];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 데이터를로드합니다.
	// 첫 번째 삼각형
	vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형
	vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다.
	result = deviceContext->Map(this->vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * this->vertexCount_));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(this->vertexBuffer_, 0);

	// 더 이상 필요하지 않은 꼭지점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// vertex buffer 단위와 offset 설정
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer_, &stride, &offset);
	deviceContext->IASetIndexBuffer(this->indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	this->texture_ = new TextureClass;
	if (!this->texture_)
	{
		return false;
	}

	return this->texture_->Initialize(device, filename);
}

void BitmapClass::ReleaseTexture()
{
	if (this->texture_)
	{
		this->texture_->Shutdown();
		delete this->texture_;
		this->texture_ = nullptr;
	}
}
