#include "Stdafx.h"
#include <stdio.h>
#include "TextureClass.hpp"

TextureClass::TextureClass()
{

}


TextureClass::TextureClass(const TextureClass& other)
{

}


TextureClass::~TextureClass()
{

}

bool TextureClass::Initialize(ID3D11Device* device, char* filename)
{
	if (FAILED(CreateDDSTextureFromFile(device, (WCHAR*)filename, nullptr, &this->texture_)))
	{
		return false;
	}

	return true;
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	int width = 0;
	int height = 0;

	// targa data 로드
	if (!LoadTarga(filename, width, height))
	{
		return false;
	}
	
	// Texture 구조체 설정
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스처 생성
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &this->texture_);
	if (FAILED(hResult))
	{
		return false;
	}

	// targa data width 사이즈 설정
	unsigned int rowPitch = (width * 4) * sizeof(unsigned char);

	// targa data를 텍스처에 복사
	deviceContext->UpdateSubresource(this->texture_, 0, NULL, this->targaData_, rowPitch, 0);

	// Shader resource view 구조체 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Texture의 Shader resource view 생성
	hResult = device->CreateShaderResourceView(this->texture_, &srvDesc, &this->textureView_);
	if (FAILED(hResult))
	{
		return false;
	}

	// Texture에 대한 mip맵 생성
	deviceContext->GenerateMips(this->textureView_);

	delete[] this->targaData_;
	this->targaData_ = nullptr;

	return true;
}

void TextureClass::Shutdown()
{
	if (this->textureView_)
	{
		this->textureView_->Release();
		this->textureView_ = nullptr;
	}

	if (this->texture_)
	{
		this->texture_->Release();
		this->texture_ = nullptr;
	}

	if (this->targaData_)
	{
		delete[] this->targaData_;
		this->targaData_ = nullptr;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return this->textureView_;
}

bool TextureClass::LoadTarga(char* filename, int& width, int& height)
{
	// targa 파일을 바이너리 모드로 open
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// file header 읽기
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	width = (int)targaFileHeader.width;
	height = (int)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;

	// 32bit 인 경우에만
	if (bpp != 32)
	{
		return false;
	}

	// targa image data 메모리 할당
	int imageSize = width * height * 4;
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일 닫기
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targaData_로 복사
	this->targaData_ = new unsigned char[imageSize];
	if (!this->targaData_)
	{
		return false;
	}

	// 올바른 순서로 targaData_ 배열에 복사 (targa 형식이 거꾸로 저장되어 있음)
	int index = 0;
	int k = (width * height * 4) - (width * 4);
	
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			this->targaData_[index + 0] = targaImage[k + 2];	// R
			this->targaData_[index + 1] = targaImage[k + 1];	// G
			this->targaData_[index + 2] = targaImage[k + 0];	// B
			this->targaData_[index + 3] = targaImage[k + 3];	// A

			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후
		// 열의 시작 부분에서 이전 행으로 다시 설정
		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;

	return true;
}