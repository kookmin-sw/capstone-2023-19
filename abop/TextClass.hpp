#pragma once

class FontClass;
class FontShaderClass;

class TextClass : public AlignedAllocationPolicy<16>
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, DirectX::XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX);

	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetFps(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, DirectX::XMMATRIX, DirectX::XMMATRIX);

private:
	FontClass* font_ = nullptr;
	FontShaderClass* fontShader_ = nullptr;
	int screenWidth_ = 0;
	int screenHeight_ = 0;
	DirectX::XMMATRIX baseViewMatrix_;
	SentenceType* sentence1_intro_ = nullptr;
	SentenceType* sentence2_fps_ = nullptr;
	SentenceType* sentence3_mouseX_ = nullptr;
	SentenceType* sentence4_mouseY_ = nullptr;
};