#include "Stdafx.h"
#include "D3DClass.hpp"

D3DClass::D3DClass()
{

}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	// 수직동기화 상태 저장
	this->vsyncEnabled_ = vsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
	{
		return false;
	}

	// 팩토리 객체를 사용하여 첫번째 그래픽카드 인터페이스 어댑터 생성
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	// 출력 모니터에 대한 첫번째 어댑터 지정
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	// 출력에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
	{
		return false;
	}

	// 가능한 모니터와 그래픽 카드 조합을 저장할 리스트 생성
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 디스플레이 모드에 대한 리스트 업데이트
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
	{
		return false;
	}

	// 화면 width/height에 맞는 디스플레이 모드를 찾고, 새로고침 비율을 저장
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 비디오카드 구조체
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	// 비디오카드 메모리 용량 저장 (MB)
	this->videoCardMemory_ = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, this->videoCardDescription_, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;									// 백버퍼를 1개만 사용하도록 지정
	swapChainDesc.BufferDesc.Width = screenWidth;					// 백버퍼 width, height 지정
	swapChainDesc.BufferDesc.Width = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 32bit surface 지정

	if (this->vsyncEnabled_)
	{
		// 새로고침 비율 지정
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;		// 렌더링에 사용될 윈도우 핸들 지정

	// 멀티 샘플링 끄기
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// scanline 순서 및 크기를 지정하지 않음으로 설정
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 다음 백버퍼 비우도록 설정
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그를 사용하지 않음
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// swap chain, directX device, device context 생성
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &(this->swapChain_), &(this->device_), NULL, &(this->deviceContext_)))) 
	{
		return false;
	}

	// 백버퍼 포인터
	ID3D11Texture2D* backBufferPtr = nullptr;
	if (FAILED(this->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
	{
		return false;
	}

	// 백버퍼 포인터로 렌더 타겟 뷰 생성
	if (FAILED(this->device_->CreateRenderTargetView(backBufferPtr, NULL, &(this->renderTargetView_))))
	{
		return false;
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;

	// 깊이 버퍼 구조체 초기화
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼 작성
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 설정된 깊이버퍼 구조체를 사용하여 깊이 버퍼 텍스쳐를 생성합니다
	if (FAILED(this->device_->CreateTexture2D(&depthBufferDesc, NULL, &(this->depthStencilBuffer_))))
	{
		return false;
	}

	// 스텐실 상태 구조체를 초기화합니다
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태 구조체를 작성합니다
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀 정면의 스텐실 설정입니다
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀 뒷면의 스텐실 설정입니다
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 생성합니다
	if (FAILED(this->device_->CreateDepthStencilState(&depthStencilDesc, &(this->depthStencilState_))))
	{
		return false;
	}

	// 깊이 스텐실 상태를 설정합니다
	this->deviceContext_->OMSetDepthStencilState(this->depthStencilState_, 1);

	// 깊이 스텐실 뷰의 구조체를 초기화합니다
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정합니다
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 생성합니다
	if (FAILED(this->device_->CreateDepthStencilView(this->depthStencilBuffer_, &depthStencilViewDesc, &(this->depthStencilView_))))
	{
		return false;
	}

	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩합니다
	this->deviceContext_->OMSetRenderTargets(1, &(this->renderTargetView_), this->depthStencilView_);

	// 그려지는 폴리곤과 방법을 결정할 래스터 구조체를 설정합니다
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	//rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 방금 작성한 구조체에서 래스터 라이저 상태를 만듭니다
	if (FAILED(this->device_->CreateRasterizerState(&rasterDesc, &(this->rasterState_))))
	{
		return false;
	}

	// 이제 래스터 라이저 상태를 설정합니다
	this->deviceContext_->RSSetState(this->rasterState_);

	// 렌더링을 위해 뷰포트를 설정합니다
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 뷰포트를 생성합니다
	this->deviceContext_->RSSetViewports(1, &viewport);

	// 투영 행렬을 설정합니다
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을위한 투영 행렬을 만듭니다
	this->projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 세계 행렬을 항등 행렬로 초기화합니다
	this->worldMatrix_ = DirectX::XMMatrixIdentity();

	// 2D 렌더링을위한 직교 투영 행렬을 만듭니다
	this->orthoMatrix_ = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// Clear the second depth stencil state before setting the parameters.
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// 2D렌더링을 위해 z버퍼를 끄는 두번째 깊이의 스텐실(stenciㅣ) 상태를 만듭니다
	// 유일한 차이점은 DepthEnable이 false로 설정되고, 다른 모든 매개변수는 다른 깊이 스텐실 상태와 동일한 점입니다
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 디바이스를 사용하여 상태를 만듭니다
	if (FAILED(device_->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState_)))
	{
		return false;
	}

	// 블렌딩 상태의 description을 초기화합니다
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// 알파값이 적용된 블렌딩 상태를 만들기 위해 상태를 수정합니다
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// description을 이용하여 알파값이 켜진 블렌딩 상태를 생성합니다
	if (FAILED(device_->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState_)))
	{
		return false;
	}

	// 알파값을 사용하지 않는 상태를 만듭니다
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// 바뀐 description을 사용하여 알파값이 꺼진 블렌딩 상태를 생성합니다
	if (FAILED(device_->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState_)))
	{
		return false;
	}

	return true;
}

void D3DClass::Shutdown()
{
	if (this->swapChain_)
	{
		// 종료 전 윈도우 모드로 설정(swap chain 해제 시 예외 발생)
		this->swapChain_->SetFullscreenState(false, NULL);
	}

	if (this->depthDisabledStencilState_)
	{
		this->depthDisabledStencilState_->Release();
		this->depthDisabledStencilState_ = nullptr;
	}
	////블렌딩 상태 해제
	//if (alphaEnableBlendingState_)
	//{
	//	alphaEnableBlendingState_->Release();
	//	alphaEnableBlendingState_ = nullptr;
	//}
	////블렌딩 상태 해제
	//if (alphaDisableBlendingState_)
	//{
	//	alphaDisableBlendingState_->Release();
	//	alphaDisableBlendingState_ = nullptr;
	//}

	if (this->rasterState_)
	{
		this->rasterState_->Release();
		this->rasterState_ = nullptr;
	}

	if (this->depthStencilView_)
	{
		this->depthStencilView_->Release();
		this->depthStencilView_ = nullptr;
	}

	if (this->depthStencilState_)
	{
		this->depthStencilState_->Release();
		this->depthStencilState_ = nullptr;
	}
	
	if (this->depthStencilBuffer_)
	{
		this->depthStencilBuffer_->Release();
		this->depthStencilBuffer_ = nullptr;
	}

	if (this->renderTargetView_)
	{
		this->renderTargetView_->Release();
		this->renderTargetView_ = nullptr;
	}

	if (this->deviceContext_)
	{
		this->deviceContext_->Release();
		this->deviceContext_ = nullptr;
	}

	if (this->device_)
	{
		this->device_->Release();
		this->device_ = nullptr;
	}

	if (this->swapChain_)
	{
		this->swapChain_->Release();
		this->swapChain_ = nullptr;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	// 버퍼를 지울 색 설정
	float color[4] = { red, green, blue, alpha };

	// 백버퍼를 지웁니다.
	deviceContext_->ClearRenderTargetView(this->renderTargetView_, color);

	// 깊이 버퍼를 지웁니다.
	this->deviceContext_->ClearDepthStencilView(this->depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// 렌더링이 완료되었으므로 화면에 백 버퍼를 표시
	if (this->vsyncEnabled_)
	{
		this->swapChain_->Present(1, 0);
	}
	else
	{
		// 가능한 빠르게 출력
		this->swapChain_->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return this->device_;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return this->deviceContext_;
}

ID3D11RenderTargetView* D3DClass::GetRenderTargetView()
{
	return this->renderTargetView_;
}

void D3DClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = this->projectionMatrix_;
}


void D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix_;
}


void D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = this->orthoMatrix_;
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, this->videoCardDescription_);
	memory = this->videoCardMemory_;
}

void D3DClass::TurnZBufferOn()
{
	deviceContext_->OMSetDepthStencilState(depthStencilState_, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	deviceContext_->OMSetDepthStencilState(depthDisabledStencilState_, 1);
	return;
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// 알파 블렌딩을 켭니다
	deviceContext_->OMSetBlendState(alphaEnableBlendingState_, blendFactor, 0xffffffff);

	return;
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// 알파 블렌딩을 끕니다
	deviceContext_->OMSetBlendState(alphaDisableBlendingState_, blendFactor, 0xffffffff);

	return;
}