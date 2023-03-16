#include <vector>
#include <string>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "D3DClass.hpp"
#include "CameraClass.hpp"
#include "ModelClass.hpp"
#include "ModelVariation.hpp"
#include "ColorShaderClass.hpp"		// !!! TEMP
#include "TextureShaderClass.hpp"	// !!! TEMP
#include "LightShaderClass.hpp"
#include "LightClass.hpp"
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
	this->camera_->SetPosition(0.0f, 0.0f, -20.0f);

	// Model 객체 생성
	this->models_ = new std::vector<ModelClass*>();
	if (!this->models_)
	{
		return false;
	}

	// vetex 정보를 가져옴
	if (lSystem)
	{
		// !!! 렌더링할 L-System을 가져온 상태여야 함
		this->lSystem_ = lSystem;

		std::vector<Model>* models = new std::vector<Model>();
		this->lSystem_->GetResultVertex(models);

		// Model 객체 초기화
		for (const Model& model : *models)
		{
			// !!! 임시 마음에 안듦
			// Model -> ModelClass

			if (model.modelType == ModelType::Custom)
			{
				ModelClass* modelClass = new ModelClass;
				modelClass->Initialize(this->direct3D_->GetDevice(), model);

				this->models_->push_back(modelClass);
			}
			else
			{
				// !!! TEMP
				Cube* cube = new Cube;
				cube->SetPosition(model.data[0], model.data[1], model.data[2]);
				cube->SetRotation(model.data[3], model.data[4], model.data[5]);
				cube->SetSize(model.data[6], model.data[7], model.data[8]);

				cube->Initialize(this->direct3D_->GetDevice());

				delete[] model.data;

				this->models_->push_back((ModelClass*)cube);
			}
		}		
	}
	else
	{
		// !!! FOR TEST
		ModelClass* model = new ModelClass;
		model->Initialize(this->direct3D_->GetDevice(), this->direct3D_->GetDeviceContext(),
			(char*)"data/stone01.tga");

		this->models_->push_back(model);

		//Triangle* cube = new Triangle;

		//cube->Initialize(this->direct3D_->GetDevice());
		//this->models_->push_back((ModelClass*)cube);

		// ---------------------
	}

	// !!! TEMP  ---------------------------------------
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

	// TextureShader 객체 생성
	this->textureShader_ = new TextureShaderClass;
	if (!this->textureShader_)
	{
		return false;
	}

	// TextureShader 객체 초기화
	if (!this->textureShader_->Initialize(this->direct3D_->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}
	// !!! ---------------------------------------------

	// LightShader 초기화
	this->lightShader_ = new LightShaderClass;
	if (!this->lightShader_)
	{
		return false;
	}

	if (!this->lightShader_->Initialize(this->direct3D_->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	// Light 객체 초기화
	this->light_ = new LightClass;
	if (!this->light_)
	{
		return false;
	}

	this->light_->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	this->light_->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	if (this->light_)
	{
		delete this->light_;
		this->light_ = nullptr;
	}

	if (this->lightShader_)
	{
		this->lightShader_->Shutdown();
		delete this->lightShader_;
		this->lightShader_ = nullptr;
	}

	if (this->textureShader_)
	{
		this->textureShader_->Shutdown();
		delete this->textureShader_;
		this->textureShader_ = nullptr;
	}

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

bool GraphicsClass::Frame(int mouseX, int mouseY, int forward, int right, int pitchUp, int rotationRight)
{
	// light rotation 업데이트
	this->rotation_ += (float)DirectX::XM_PI * 0.001f;
	if (this->rotation_ > 360.0f)
	{
		this->rotation_ -= 360.0f;
	}

	// !!! mouse 위치 text 업데이트

	// !!! 키 입력 여부 수정 예정
	float tempCameraSpeed = 0.3f;
	this->camera_->Walk(forward * tempCameraSpeed);
	this->camera_->Strafe(right * tempCameraSpeed);

	float tempRotationSpeed = 0.01f;
	this->camera_->Pitch(pitchUp * tempRotationSpeed);
	this->camera_->RotateY(rotationRight * tempRotationSpeed);

	return true;
}

// Private
bool GraphicsClass::Render()
{
	// 버퍼 지우기
	this->direct3D_->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// 카메라 위치에 따라 뷰 행렬 생성
	this->camera_->Render();

	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Vertex, Index buffer를 그래픽 파이프라인에 배치
	for (ModelClass* model : *(this->models_))
	{
		this->direct3D_->GetWorldMatrix(worldMatrix);
		//this->camera_->GetViewMatrix(viewMatrix);
		viewMatrix = this->camera_->View();
		this->direct3D_->GetProjectionMatrix(projectionMatrix);

		model->Render(this->direct3D_->GetDeviceContext());

		// 스케일 변환
		// !!! to be update

		// 회전 변환
		Vector3 rotation = model->GetRotation();
		//DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw
		//(
		//	rotation.x,
		//	rotation.y,
		//	rotation.z
		//);
		DirectX::XMMATRIX xMatrix = DirectX::XMMatrixRotationX(rotation.x);
		DirectX::XMMATRIX yMatrix = DirectX::XMMatrixRotationY(rotation.y);
		DirectX::XMMATRIX zMatrix = DirectX::XMMatrixRotationZ(rotation.z);

		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, zMatrix);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, yMatrix);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, xMatrix);
		
		// 이동 변환
		Vector3 translation = model->GetPosition();
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation
		(
			translation.x,
			translation.y,
			translation.z
		);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translationMatrix);

		// world 회전
		worldMatrix = DirectX::XMMatrixRotationY(this->rotation_);

		// 텍스쳐 셰이더를 사용하여 모델을 렌더링합니다.
		//if (!this->colorShader_->Render(this->direct3D_->GetDeviceContext(),
		//	model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		//{
		//	// ColorShader를 통해 렌더링
		//	return false;
		//}

		//if (!this->textureShader_->Render(this->direct3D_->GetDeviceContext(), model->GetIndexCount(), 
		//	worldMatrix, viewMatrix, projectionMatrix, model->GetTexture()))
		//{
		//	return false;
		//}
		// Light 셰이더를 사용해서 모델 렌더링
		if (!this->lightShader_->Render(this->direct3D_->GetDeviceContext(), model->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(),
			this->light_->GetDirection(), this->light_->GetDiffuseColor()))
		{
			return false;
		}
	}

	this->direct3D_->EndScene();

	return true;
}
