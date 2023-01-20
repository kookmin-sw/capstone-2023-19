#pragma

//Global
const float CAMERA_MOVING = 0.2f;
const float CAMERA_ROTATION = 0.0174532925f;

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();

	void Render(WPARAM);
	void GetViewMatrix(DirectX::XMMATRIX&);

private:
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 rotation_;
	DirectX::XMMATRIX viewMatrix_;
};