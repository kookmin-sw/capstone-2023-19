#include "Stdafx.h"
#include "CameraClass.hpp"

CameraClass::CameraClass()
{
	this->position_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotation_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	this->position_.x = x;
	this->position_.y = y;
	this->position_.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	this->rotation_.x = x;
	this->rotation_.y = y;
	this->rotation_.z = z;
}

DirectX::XMFLOAT3 CameraClass::GetPosition()
{
	return this->position_;
}

DirectX::XMFLOAT3 CameraClass::GetRotation()
{
	return this->rotation_;
}

void CameraClass::Render(WPARAM key)
{
	DirectX::XMFLOAT3 up, position, lookAt;
	DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	DirectX::XMMATRIX rotationMatrix;

	// Up
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = DirectX::XMLoadFloat3(&up);

	// Position
	switch (key)
	{
		case VK_LEFT:
			this->position_.x += CAMERA_MOVING;
			break;
		case VK_UP:
			this->position_.y -= CAMERA_MOVING;
			break;
		case VK_RIGHT:
			this->position_.x -= CAMERA_MOVING;
			break;
		case VK_DOWN:
			this->position_.y += CAMERA_MOVING;
			break;
	}

	position = this->position_;
	positionVector = DirectX::XMLoadFloat3(&position);

	// Look at
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	// 회전값을 라디안 단위로 설정
	// pitch (x), yaw (y), roll (z)
	pitch = this->rotation_.x * CAMERA_ROTATION;
	yaw = this->rotation_.y * CAMERA_ROTATION;
	roll = this->rotation_.z * CAMERA_ROTATION;
	// 회전 행렬 생성
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt과 up vector를 회전 행렬로 변환
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// 뷰어를 위치 이동을 적용
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	// view 행렬 생성
	this->viewMatrix_ = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix_;
}
