#include <string>
#include <vector>
#include <iostream>
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include "ModelClass.hpp"
#include "ModelVariation.hpp"
#include "D3DClass.hpp"
#include "CameraClass.hpp"
#include "LightClass.hpp"
#include "ColorShaderClass.hpp"
#include "LSystem.hpp"

#include "Graphics.hpp"

bool Graphics::Initialize(HWND hwnd, D3DClass* d3d, LSystem* lSystem)
{
	// D3DClass
	this->d3d_ = d3d;

	// LSystem
	this->lSystem_ = lSystem;

	// Camera
	this->camera_ = new CameraClass;
	if (!this->camera_)
	{
		return false;
	}

	DirectX::XMMATRIX baseViewMatrix;
	this->camera_->SetPosition(0.0f, 0.0f, -100.0f);
	this->camera_->Render();
	baseViewMatrix = this->camera_->View();

	// Models
	this->models_ = new std::vector<ModelClass*>();
	if (!this->models_)
	{
		return false;
	}

	// Light
	this->light_ = new LightClass;
	if (!this->light_)
	{
		return false;
	}

	// Default Light Setting
	this->light_->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	this->light_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->light_->SetDirection(0.0f, 0.0f, 1.0f);
	this->light_->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->light_->SetSpecularPower(30.0f);

	// Shader
	this->colorShader_ = new ColorShaderClass;
	if (!this->colorShader_)
	{
		return false;
	}

	if (!this->colorShader_->Initialize(this->d3d_->GetDevice(), hwnd))
	{
		std::cout << "Could not initialize the color shader object" << std::endl;
		return false;
	}

	return false;
}

void Graphics::Shutdown()
{
	if (this->light_)
	{
		delete this->light_;
		this->light_ = nullptr;
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

	if (this->d3d_)
	{
		this->d3d_->Shutdown();
		delete this->d3d_;
		this->d3d_ = nullptr;
	}
}

bool Graphics::Frame(int forward, int right, int pitchUp, int rotationRight, int up)
{
	float tempCameraSpeed = 0.3f;
	this->camera_->Walk(forward * tempCameraSpeed);
	this->camera_->Strafe(right * tempCameraSpeed);

	float tempRotationSpeed = 0.01f;
	this->camera_->Pitch(pitchUp * tempRotationSpeed);
	this->camera_->RotateY(rotationRight * tempRotationSpeed);

	float tempUpSpeed = 0.1f;
	this->camera_->Up(up * tempUpSpeed);

	return this->Render();
}

void Graphics::UpdateModels()
{
	// 모델 초기화
	std::vector<Model>* models = new std::vector<Model>();
	if (!models)
	{
		return;
	}

	// 임시 Plane 추가
	Plane* plane = new Plane();
	plane->SetWidth(1000.0f);
	plane->SetHeight(1000.0f);
	plane->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
	// 임시 회전
	DirectX::XMFLOAT3 axisX = DirectX::XMFLOAT3(1, 0, 0);
	DirectX::XMVECTOR v = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axisX), 90.0f * 3.14f / 180.0f);
	plane->SetQuaternion
	(
		DirectX::XMVectorGetX(v),
		DirectX::XMVectorGetY(v),
		DirectX::XMVectorGetZ(v),
		DirectX::XMVectorGetW(v)
	);
	plane->Initialize(this->d3d_->GetDevice());
	this->models_->push_back((ModelClass*)plane);

	this->lSystem_->GetResultVertex(models);
	for (const Model& model : *models)
	{
		if (model.modelType == ModelType::Custom)
		{
			ModelClass* modelClass = new ModelClass;
			modelClass->Initialize(this->d3d_->GetDevice(), model);

			this->models_->push_back(modelClass);
		}
		else
		{

			// !!! TEMP
			Cube* cube = new Cube;

			cube->SetPosition(model.data[0], model.data[1], model.data[2]);
			cube->SetQuaternion(model.data[3], model.data[4], model.data[5], model.data[6]);
			cube->SetSize(model.data[7], model.data[8], model.data[9]);

			cube->Initialize(this->d3d_->GetDevice());

			delete[] model.data;

			this->models_->push_back((ModelClass*)cube);
		}
	}
}

bool Graphics::Render()
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	// 카메라 위치에 따라 뷰 행렬 생성
	this->camera_->Render();
	
	viewMatrix = this->camera_->View();
	this->d3d_->GetWorldMatrix(worldMatrix);
	this->d3d_->GetProjectionMatrix(projectionMatrix);
	this->d3d_->GetOrthoMatrix(orthoMatrix);

	// Vertex, Index buffer를 그래픽 파이프라인에 배치
	for (ModelClass* model : *(this->models_))
	{
		this->d3d_->GetWorldMatrix(worldMatrix);
		//this->camera_->GetViewMatrix(viewMatrix);
		//viewMatrix = this->camera_->View();
		//this->direct3D_->GetProjectionMatrix(projectionMatrix);

		model->Render(this->d3d_->GetDeviceContext());

		// 스케일 변환
		// !!! to be update

		// 회전 변환
		DirectX::XMFLOAT4 quat = model->GetQuaternion();
		DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&quat);
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(quaternion);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, rotMatrix);

		// 당장 짐벌락 발생하지 않는 X->Y->Z 순으로 회전변환 적용
		// TODO - 짐벌락 해결 위해서는 Quaternion으로 변환 필요
		// worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, xMatrix);
		// worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, yMatrix);
		// worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, zMatrix);

		// 이동 변환
		Vector3 translation = model->GetPosition();
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation
		(
			translation.x,
			translation.y,
			translation.z
		);
		worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translationMatrix);

		if (!model->GetTexture())
		{
			// ColorShader를 통해 렌더링
			bool result = this->colorShader_->Render(
				this->d3d_->GetDeviceContext(), model->GetIndexCount(),
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
			/*if (!this->lightShader_->Render(this->direct3D_->GetDeviceContext(), model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(),
				this->light_->GetDirection(), this->light_->GetAmbientColor(), this->light_->GetDiffuseColor()))
			{
				return false;
			}*/
		}
	}

	//this->direct3D_->GetWorldMatrix(worldMatrix);
	//this->direct3D_->GetOrthoMatrix(orthoMatrix);

	return false;
}