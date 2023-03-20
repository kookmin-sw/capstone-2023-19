#pragma once

class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char* targaData_ = nullptr;
	ID3D11Texture2D* texture_ = nullptr; 
	ID3D11ShaderResourceView* textureView_ = nullptr;
};