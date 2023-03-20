#pragma once

class TextureClass;

class BitmapClass
{
private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer* vertexBuffer_ = nullptr;
	ID3D11Buffer* indexBuffer_ = nullptr;

	int vertexCount_ = 0;
	int indexCount_ = 0;

	TextureClass* texture_ = nullptr;
	int screenWidth_ = 0;
	int screenHeight_ = 0;
	int bitmapWidth_ = 0;
	int bitmapHeight_ = 0;
	int previousPosX_ = 0;
	int previousPosY_ = 0;
};