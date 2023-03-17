#include "Stdafx.h"
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


void TextureClass::Shutdown()
{
	// Release the texture resource.
	if(texture_)
	{
		texture_->Release();
		texture_ = 0;
	}

	return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return texture_;
}