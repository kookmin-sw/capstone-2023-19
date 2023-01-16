#include "Stdafx.h"
#include "D3DClass.hpp"
#include "CameraClass.hpp"
#include "ModelClass.hpp"
#include "ColorShaderClass.hpp"
#include "GraphicsClass.hpp"


// Public
GraphicsClass::GraphicsClass()
{

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	// C4316 이슈로 malloc 및 free 사용
	//this->direct3D_ = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	this->direct3D_ = new D3DClass;

	if (!this->direct3D_)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if (!this->direct3D_->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd,
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Camera 객체 생성
	this->camera_ = new CameraClass;
	if (!this->camera_)
	{
		return false;
	}
	this->camera_->SetPosition(0.0f, 0.0f, -5.0f);

	// Model 객체 생성
	this->model_ = new ModelClass;
	if (!this->model_)
	{
		return false;
	}

	// Model 객체 초기화
	if (!this->model_->Initialize(this->direct3D_->GetDevice()))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	}

	// ColorShader 객체 생성
	this->colorShader_ = new ColorShaderClass;
	if (!this->colorShader_)
	{
		return false;
	}

	// ColorShader 객체 초기화
	if (!this->colorShader_->Initialize(this->direct3D_->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (this->colorShader_)
	{
		this->colorShader_->Shutdown();
		delete this->colorShader_;
		this->colorShader_ = nullptr;
	}

	if (this->model_)
	{
		this->model_->Shutdown();
		delete this->model_;
		this->model_ = nullptr;
	}

	if (this->camera_)
	{
		delete this->camera_;
		this->camera_ = nullptr;
	}

	if (this->direct3D_)
	{
		// Direct3D 객체 반환
		this->direct3D_->Shutdown();
		//_aligned_free(this->direct3D_);
		delete this->direct3D_;
		this->direct3D_ = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	return this->Render();
}

// Private
bool GraphicsClass::Render()
{
	// 버퍼 지우기
	this->direct3D_->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치에 따라 뷰 행렬 생성
	this->camera_->Render();

	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	this->direct3D_->GetWorldMatrix(worldMatrix);
	this->camera_->GetViewMatrix(viewMatrix);
	this->direct3D_->GetProjectionMatrix(projectionMatrix);

	// Vertex, Index buffer를 그래픽 파이프라인에 배치
	this->model_->Render(this->direct3D_->GetDeviceContext());

	if (!this->colorShader_->Render(this->direct3D_->GetDeviceContext(),
		this->model_->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		// ColorShader를 통해 렌더링
		return false;
	}

	this->direct3D_->EndScene();

	return true;
}
