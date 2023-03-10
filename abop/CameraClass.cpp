#include "Stdafx.h"
#include "CameraClass.hpp"

CameraClass::CameraClass()
{
	this->position_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	//this->rotation_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->right_ = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->up_ = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->look_ = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

// 카메라 위치 get, set
DirectX::XMVECTOR CameraClass::GetPositionXM() const
{
	return DirectX::XMLoadFloat3(&(this->position_));
}

DirectX::XMFLOAT3 CameraClass::GetPosition() const
{
	return this->position_;
}

void SetPosition(const DirectX::XMFLOAT3&);


void CameraClass::SetPosition(float x, float y, float z)
{
	this->position_ = DirectX::XMFLOAT3(x, y, z);
}


void CameraClass::SetPosition(const DirectX::XMFLOAT3& v)
{
	this->position_ = v;
}

// 카메라 기저벡터
DirectX::XMVECTOR CameraClass::GetRightXM() const
{
	return DirectX::XMLoadFloat3(&(this->right_));
}

DirectX::XMFLOAT3 CameraClass::GetRight() const
{
	return this->right_;
}

DirectX::XMVECTOR CameraClass::GetUpXM() const
{
	return DirectX::XMLoadFloat3(&(this->up_));
}

DirectX::XMFLOAT3 CameraClass::GetUp() const
{
	return this->up_;
}

DirectX::XMVECTOR CameraClass::GetLookXM() const
{
	return DirectX::XMLoadFloat3(&(this->look_));
}

DirectX::XMFLOAT3 CameraClass::GetLook() const
{
	return this->look_;
}


// 절두체 속성
float CameraClass::GetNearZ() const
{
	return this->nearZ_;
}

float CameraClass::GetFarZ() const
{
	return this->farZ_;
}

float CameraClass::GetAspect() const
{
	return this->aspect_;
}

float CameraClass::GetFovY() const
{
	return this->fovY_;
}

float CameraClass::GetFovX() const
{
	float halfWidth = 0.5f * this->GetNearWindowWidth();

	return 2.0f * atan(halfWidth / this->nearZ_);
}

// 시야 공간 기준 near, far 평면 크기
float CameraClass::GetNearWindowWidth() const
{
	return this->aspect_ * this->nearWindowHeight_;
}

float CameraClass::GetNearWindowHeight() const
{
	return this->nearWindowHeight_;
}

float CameraClass::GetFarWindowWidth() const
{
	return this->aspect_ * this->farWindowHeight_;
}

float CameraClass::GeFarWindowHeight() const
{
	return this->farWindowHeight_;
}


// 절두체 설정
void CameraClass::SetLens(float fovY, float aspect, float zn, float zf)
{
	this->fovY_ = fovY;
	this->aspect_ = aspect;
	this->nearZ_ = zn;
	this->farZ_ = zf;

	this->nearWindowHeight_ = 2.0f * this->nearZ_ * tanf(0.5f * this->fovY_);
	this->farWindowHeight_ = 2.0f * this->farZ_ * tanf(0.5f * this->fovY_);
}

// 카메라 위치와 시선 방향으로 카메라 공간 설정
void CameraClass::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
	// look (target - pos)
	DirectX::XMVECTOR L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, pos));
	// right (world up, look과 직교)
	DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
	// up (camera look, right와 직교)
	DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

	DirectX::XMStoreFloat3(&(this->position_), pos);
	DirectX::XMStoreFloat3(&(this->look_), L);
	DirectX::XMStoreFloat3(&(this->right_), R);
	DirectX::XMStoreFloat3(&(this->up_), U);
}

void CameraClass::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	DirectX::XMVECTOR P = XMLoadFloat3(&pos);
	DirectX::XMVECTOR T = XMLoadFloat3(&target);
	DirectX::XMVECTOR U = XMLoadFloat3(&up);

	this->LookAt(P, T, U);
}

// 시야, 투영 행렬
//DirectX::XMMATRIX CameraClass::GetViewMatrix() const
//{
//	return this->viewMatrix_;
//}
//
//DirectX::XMMATRIX CameraClass::GetProjMatrix() const
//{
//	return this->projMatrix_;
//}
//
//DirectX::XMMATRIX CameraClass::GetViewProjMatrix() const
//{
//	return DirectX::XMMatrixMultiply(this->GetViewMatrix(), this->GetProjMatrix());
//}

DirectX::XMMATRIX CameraClass::View() const
{
	return DirectX::XMLoadFloat4x4(&(this->view_));
}

DirectX::XMMATRIX CameraClass::Proj() const
{
	return DirectX::XMLoadFloat4x4(&(this->proj_));
}

DirectX::XMMATRIX CameraClass::ViewProj() const
{
	return DirectX::XMMatrixMultiply(this->View(), this->Proj());
}


// 카메라 이동
void CameraClass::Strafe(float d)
{
	// 시선 벡터를 기준으로 횡이동
	//this->position_ += d * this->right_;
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(this->right_));
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&(this->position_));

	DirectX::XMStoreFloat3(&(this->position_), DirectX::XMVectorMultiplyAdd(s, r, p));
}
void CameraClass::Walk(float d)
{
	// 축이동
	//this->position_ += d * this->look_;
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(this->look_));
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&(this->position_));

	DirectX::XMStoreFloat3(&(this->position_), DirectX::XMVectorMultiplyAdd(s, l, p));
}

// 카메라 회전
void CameraClass::Pitch(float angle)
{
	// 시선 벡터를 기준으로 상하회전
	// right vector(x축)을 기준으로 angle만큼 회전
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&(this->right_)), angle);

	DirectX::XMStoreFloat3(&(this->up_), DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(this->up_)), R));
	DirectX::XMStoreFloat3(&(this->look_), DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(this->look_)), R));
}

void CameraClass::RotateY(float angle)
{
	// 좌우회전
	// 세계 공간 y축을 기준으로 angle만큼 회전
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	DirectX::XMStoreFloat3(&(this->right_), DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(this->right_)), R));
	DirectX::XMStoreFloat3(&(this->up_), DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(this->up_)), R));
	DirectX::XMStoreFloat3(&(this->look_), DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&(this->look_)), R));
}

void CameraClass::Render()
{
	// vector를 다시 정규직교화 (오차 누적 방지)
	DirectX::XMVECTOR R = DirectX::XMLoadFloat3(&(this->right_));
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&(this->up_));
	DirectX::XMVECTOR L = DirectX::XMLoadFloat3(&(this->look_));
	DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&(this->position_));

	L = DirectX::XMVector3Normalize(L);
	U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

	R = DirectX::XMVector3Cross(U, L);

	float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
	float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
	float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

	XMStoreFloat3(&(this->right_), R);
	XMStoreFloat3(&(this->up_), U);
	XMStoreFloat3(&(this->look_), L);

	this->view_(0, 0) = (this->right_).x;
	this->view_(1, 0) = (this->right_).y;
	this->view_(2, 0) = (this->right_).z;
	this->view_(3, 0) = x;

	this->view_(0, 1) = (this->up_).x;
	this->view_(1, 1) = (this->up_).y;
	this->view_(2, 1) = (this->up_).z;
	this->view_(3, 1) = y;

	this->view_(0, 2) = (this->look_).x;
	this->view_(1, 2) = (this->look_).y;
	this->view_(2, 2) = (this->look_).z;
	this->view_(3, 2) = z;

	this->view_(0, 3) = 0.0f;
	this->view_(1, 3) = 0.0f;
	this->view_(2, 3) = 0.0f;
	this->view_(3, 3) = 1.0f;

#pragma region before
	//DirectX::XMFLOAT3 up, position, lookAt;
	//DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	//float yaw, pitch, roll;
	//DirectX::XMMATRIX rotationMatrix;

	//// Up
	//up.x = 0.0f;
	//up.y = 1.0f;
	//up.z = 0.0f;
	//upVector = DirectX::XMLoadFloat3(&up);

	//// Position
	//position = this->position_;
	//positionVector = DirectX::XMLoadFloat3(&position);

	//// Look at
	//lookAt.x = 0.0f;
	//lookAt.y = 0.0f;
	//lookAt.z = 1.0f;
	//lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	//// 회전값을 라디안 단위로 설정
	//// pitch (x), yaw (y), roll (z)
	//pitch = this->rotation_.x * 0.0174532925f;
	//yaw = this->rotation_.y * 0.0174532925f;
	//roll = this->rotation_.z * 0.0174532925f;
	//// 회전 행렬 생성
	//rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//// lookAt과 up vector를 회전 행렬로 변환
	//lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	//upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	//// 뷰어를 위치 이동을 적용
	//lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	//// view 행렬 생성
	//this->viewMatrix_ = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector
#pragma endregion before
}
