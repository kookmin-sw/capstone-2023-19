#pragma once

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(DirectX::XMMATRIX&);
	void GetWorldMatrix(DirectX::XMMATRIX&);
	void GetOrthoMatrix(DirectX::XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool vsyncEnabled_ = false;
	int videoCardMemory_ = 0;
	char videoCardDescription_[128] = { 0, };
	IDXGISwapChain* swapChain_ = nullptr;
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* deviceContext_ = nullptr;
	ID3D11RenderTargetView* renderTargetView_ = nullptr;
	ID3D11Texture2D* depthStencilBuffer_ = nullptr;
	ID3D11DepthStencilState* depthStencilState_ = nullptr;
	ID3D11DepthStencilView* depthStencilView_ = nullptr;
	ID3D11RasterizerState* rasterState_ = nullptr;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX orthoMatrix_;
};