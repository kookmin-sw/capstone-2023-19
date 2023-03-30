#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	// 카메라 위치 get, set
	DirectX::XMVECTOR GetPositionXM() const;
	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(float, float, float);
	void SetPosition(const DirectX::XMFLOAT3&);

	// 카메라 기저벡터
	DirectX::XMVECTOR GetRightXM() const;
	DirectX::XMFLOAT3 GetRight() const;
	DirectX::XMVECTOR GetUpXM() const;
	DirectX::XMFLOAT3 GetUp() const;
	DirectX::XMVECTOR GetLookXM() const;
	DirectX::XMFLOAT3 GetLook() const;

	// 절두체 속성
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	// 시야 공간 기준 near, far 평면 크기
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GeFarWindowHeight() const;

	// 절두체 설정
	void SetLens(float, float, float, float);

	// 카메라 위치와 시선 방향으로 카메라 공간 설정
	void LookAt(DirectX::FXMVECTOR, DirectX::FXMVECTOR, DirectX::FXMVECTOR);
	void LookAt(const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3&);

	// 시야, 투영 행렬
	DirectX::XMMATRIX View() const;
	DirectX::XMMATRIX Proj() const;
	DirectX::XMMATRIX ViewProj() const;

	//DirectX::XMMATRIX GetViewMatrix() const;
	//DirectX::XMMATRIX GetProjMatrix() const;
	//DirectX::XMMATRIX GetViewProjMatrix() const;

	// 카메라 이동
	void Strafe(float);		// 시선 벡터를 기준으로 횡이동
	void Walk(float);		// 축이동
	void Up(float);			// z축 이동

	// 카메라 회전
	void Pitch(float);		// 시선 벡터를 기준으로 상하회전
	void RotateY(float);	// 좌우회전
	// roll은 사용하지 않음

	void Render();

private:
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 right_;
	DirectX::XMFLOAT3 up_;
	DirectX::XMFLOAT3 look_;

	float nearZ_;
	float farZ_;
	float aspect_;
	float fovY_;
	float nearWindowHeight_;
	float farWindowHeight_;

	DirectX::XMFLOAT4X4 view_;
	DirectX::XMFLOAT4X4 proj_;
	//DirectX::XMFLOAT3 rotation_;
	//DirectX::XMMATRIX viewMatrix_;
	//DirectX::XMMATRIX projMatrix_;
};