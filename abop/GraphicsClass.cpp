#include <vector>
#include <string>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "D3DClass.hpp"
#include "CameraClass.hpp"
#include "ModelClass.hpp"
#include "ColorShaderClass.hpp"
#include "LSystem.hpp"
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

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, LSystem* lSystem)
{
	// Main의 LSystem 포인터 할당
	if (lSystem)
	{
		this->lSystem_ = lSystem;
	}

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
	this->camera_->SetPosition(0.0f, 30.0f, -150.0f);
	//this->camera_->SetRotation(0.0f, 0.0f, 0.0f);

	// Model 객체 생성
	this->models_ = new std::vector<ModelClass*>();
	if (!this->models_)
	{
		return false;
	}

	std::vector<Model>* models = new std::vector<Model>();
	// vetex 정보를 가져옴
	if (lSystem)
	{
		this->lSystem_->GetResultVertex(models);

		// Model 객체 초기화
		// for (ModelClass* model : *(this->models_))
		for (const Model& model : *models)
		{
			ModelClass* modelClass = new ModelClass;
			modelClass->Initialize(this->direct3D_->GetDevice(), model);

			this->models_->push_back(modelClass);
		}		
	}
	else
	{
		// lSystem이 null인 경우
		// triangle
		//Model model;
		//model.vertexCount = 3;

		//Vector4 black = { 0, 0, 0, 0 };
		//Vector3 position;
		//position = { 0, 0.5, 0 };
		//model.vertexTypes[0] = { position, black };
		//position = { 0.5, -0.5, 0 };
		//model.vertexTypes[1] = { position, black };
		//position = { -0.5, -0.5, 0 };
		//model.vertexTypes[2] = { position, black };
		Model model;
		model.vertexCount = 4;

		Vector4 black = { 0, 0, 0, 0 };
		Vector3 position;
		position = { -0.5, 0.5, 0 };
		model.vertexTypes[0] = { position, black };
		position = { 0.5, 0.5, 0 };
		model.vertexTypes[1] = { position, black };
		position = { 0.5, -0.5, 0 };
		model.vertexTypes[2] = { position, black };
		position = { -0.5, -0.5, 0 };
		model.vertexTypes[3] = { position, black };

		ModelClass* modelClass = new ModelClass;
		modelClass->Initialize(this->direct3D_->GetDevice(), model);
		this->models_->push_back(modelClass);
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

	if (this->models_)
	{
		for (ModelClass* model : *(this->models_))
		{
			model->Shutdown();
			delete model;
			model = nullptr;
		}
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

bool GraphicsClass::Frame(WPARAM key)
{
	return this->Render(key);
}

// Private
bool GraphicsClass::Render(WPARAM key)
{
	// 버퍼 지우기
	this->direct3D_->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// 카메라 위치에 따라 뷰 행렬 생성
	this->camera_->Render(key);

	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	this->direct3D_->GetWorldMatrix(worldMatrix);
	this->camera_->GetViewMatrix(viewMatrix);
	this->direct3D_->GetProjectionMatrix(projectionMatrix);

	// Vertex, Index buffer를 그래픽 파이프라인에 배치
	for (ModelClass* model : *(this->models_))
	{
		model->Render(this->direct3D_->GetDeviceContext());

		if (!this->colorShader_->Render(this->direct3D_->GetDeviceContext(),
			model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		{
			// ColorShader를 통해 렌더링
			return false;
		}
	}

	this->direct3D_->EndScene();

	return true;
}
