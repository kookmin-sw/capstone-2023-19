#include <vector>
#include <string>
#include "Stdafx.h"
#include "DirectXMath.h"
#include "CommonStructure.hpp"
#include "CommonVariable.hpp"
#include "D3DClass.hpp"
#include "CameraClass.hpp"
#include "ModelClass.hpp"
#include "ModelVariation.hpp"
#include "BitmapClass.hpp"
#include "ColorShaderClass.hpp"		// !!! TEMP
#include "TextureShaderClass.hpp"	// !!! TEMP
#include "LightShaderClass.hpp"
#include "LightClass.hpp"
#include "LSystem.hpp"
#include "GraphicsClass.hpp"
#include "TextClass.hpp"

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

	DirectX::XMMATRIX baseViewMatrix;
	this->camera_->SetPosition(0.0f, 0.0f, -1.0f);
	this->camera_->Render();
	baseViewMatrix = this->camera_->View();

	// Text 객체 생성
	this->text_ = new TextClass;
	if (!this->text_)
	{
		return false;
	}

	// Text 객체 초기화
	if (!this->text_->Initialize(this->direct3D_->GetDevice(), this->direct3D_->GetDeviceContext(), 
		hwnd, screenWidth, screenHeight, baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

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
			else if (model.modelType == ModelType::LeafModel) {
				Leaf* leaf = new Leaf;
				leaf->Initialize(this->direct3D_->GetDevice(), model);

				this->models_->push_back((ModelClass*)leaf);
			}
			else
			{
				// !!! TEMP
				Cube* cube = new Cube;
				cube->SetPosition(model.data[0], model.data[1], model.data[2]);
				cube->SetQuaternion(model.data[3], model.data[4], model.data[5], model.data[6]);
				cube->SetSize(model.data[7], model.data[8], model.data[9]);

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
		if (!model)
		{
			return false;
		}

		model->Initialize(this->direct3D_->GetDevice(), this->direct3D_->GetDeviceContext(),
			(char*)"data/cube.txt", (char*)"data/stone01.tga");

		this->models_->push_back(model);
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
		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
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
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}

	// Light 객체 초기화
	this->light_ = new LightClass;
	if (!this->light_)
	{
		return false;
	}

	this->light_->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	this->light_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->light_->SetDirection(0.0f, 0.0f, 1.0f);

	this->bitmap_ = new BitmapClass;
	if (!this->bitmap_)
	{
		return false;
	}

	WCHAR tex[] = L"./data/seafloor.dds";
	if (!this->bitmap_->Initialize(this->direct3D_->GetDevice(), screenWidth, screenHeight,
		tex, 256, 256))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (this->bitmap_)
	{
		this->bitmap_->Shutdown();
		delete this->bitmap_;
		this->bitmap_ = nullptr;
	}

	if (this->light_)
	{
		delete this->light_;
		this->light_ = nullptr;
	}

	if (this->text_)
	{
		this->text_->Shutdown();
		delete this->text_;
		this->text_ = nullptr;
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

bool GraphicsClass::Frame(int mouseX, int mouseY, int forward, int right, int pitchUp, int rotationRight, int up, int fps)
{
	// light rotation 업데이트
	//this->rotation_ += (float)DirectX::XM_PI * 0.001f;
	//if (this->rotation_ > 360.0f)
	//{
	//	this->rotation_ -= 360.0f;
	//}

	bool result;
	
	// fps 업데이트
	result = this->text_->SetFps(fps, this->direct3D_->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	
	// mouse 위치 text 업데이트
	result = this->text_->SetMousePosition(mouseX, mouseY, this->direct3D_->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	

	// !!! 키 입력 여부 수정 예정
	float tempCameraSpeed = 0.3f;
	this->camera_->Walk(forward * tempCameraSpeed);
	this->camera_->Strafe(right * tempCameraSpeed);

	float tempRotationSpeed = 0.01f;
	this->camera_->Pitch(pitchUp * tempRotationSpeed);
	this->camera_->RotateY(rotationRight * tempRotationSpeed);

	float tempUpSpeed = 0.1f;
	this->camera_->Up(up * tempUpSpeed);

	return true;
}

// Private
bool GraphicsClass::Render()
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	// 버퍼 지우기
	this->direct3D_->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// 카메라 위치에 따라 뷰 행렬 생성
	this->camera_->Render();

	viewMatrix = this->camera_->View();
	this->direct3D_->GetWorldMatrix(worldMatrix);
	this->direct3D_->GetProjectionMatrix(projectionMatrix);
	this->direct3D_->GetOrthoMatrix(orthoMatrix);

	// Vertex, Index buffer를 그래픽 파이프라인에 배치
	for (ModelClass* model : *(this->models_))
	{
		this->direct3D_->GetWorldMatrix(worldMatrix);
		//this->camera_->GetViewMatrix(viewMatrix);
		//viewMatrix = this->camera_->View();
		//this->direct3D_->GetProjectionMatrix(projectionMatrix);

		model->Render(this->direct3D_->GetDeviceContext());

		// 스케일 변환
		// !!! to be update

		// 회전 변환
		DirectX::XMFLOAT4 quat = model->GetQuaternion();
		DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&quat);
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(quaternion);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, rotMatrix);

		//DirectX::XMMATRIX xMatrix = DirectX::XMMatrixRotationX(rotation.x);
		//DirectX::XMMATRIX yMatrix = DirectX::XMMatrixRotationY(rotation.y);
		//DirectX::XMMATRIX zMatrix = DirectX::XMMatrixRotationZ(rotation.z);

		//// 당장 짐벌락 발생하지 않는 X->Y->Z 순으로 회전변환 적용
		//// TODO - 짐벌락 해결 위해서는 Quaternion으로 변환 필요
		//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, xMatrix);
		//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, yMatrix);
		//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, zMatrix);

		// 이동 변환
		Vector3 translation = model->GetPosition();
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation
		(
			translation.x,
			translation.y,
			translation.z
		);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translationMatrix);

		// !!! world 회전
		//worldMatrix = DirectX::XMMatrixRotationY(this->rotation_);

		if (!model->GetTexture())
		{
			// ColorShader를 통해 렌더링
			bool result = this->colorShader_->Render(
				this->direct3D_->GetDeviceContext(), model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix,
				this->light_->GetDirection(), this->light_->GetAmbientColor(),
				this->light_->GetDiffuseColor(), this->camera_->GetPosition(),
				this->light_->GetSpecularColor(), this->light_->GetSpecularPower());

			if (!result)
			{
				return false;
			}
		}
		else
		{
			// 텍스처가 있는 경우
			// 텍스쳐 셰이더를 사용하여 모델을 렌더링합니다. (Texture only)
			//if (!this->textureShader_->Render(this->direct3D_->GetDeviceContext(), model->GetIndexCount(),
			//	worldMatrix, viewMatrix, projectionMatrix, model->GetTexture()))
			//{
			//	return false;
			//}

			// Light 셰이더를 사용해서 모델 렌더링 (Texture + Light)
			if (!this->lightShader_->Render(this->direct3D_->GetDeviceContext(), model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(),
				this->light_->GetDirection(), this->light_->GetAmbientColor(), this->light_->GetDiffuseColor()))
			{
				return false;
			}
		}
	}

	this->direct3D_->GetWorldMatrix(worldMatrix);
	this->direct3D_->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링을 시작하기 전에 Z 버퍼 끄기
	this->direct3D_->TurnZBufferOff();

	// 2D 렌더링
	//if (!this->bitmap_->Render(this->direct3D_->GetDeviceContext(), 100, 100))
	//{
	//	return false;
	//}

	//if (!this->textureShader_->Render(this->direct3D_->GetDeviceContext(),
	//	this->bitmap_->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
	//	this->bitmap_->GetTexture()))
	//{
	//	return false;
	//}
	// 
	this->direct3D_->TurnOnAlphaBlending();

	// 텍스트 문자열을 렌더링합니다
	if (!this->text_->Render(direct3D_->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	this->direct3D_->TurnOffAlphaBlending();

	this->direct3D_->TurnZBufferOn();

	this->direct3D_->EndScene();

	return true;
}
