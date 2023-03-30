#pragma once

class D3DClass;
class CameraClass;
class ModelClass;
class LSystem;
class ColorShaderClass;
class LightClass;

class Graphics
{
public:
	bool Initialize(HWND, D3DClass*, LSystem*);
	void Shutdown();
	bool Frame(int, int, int, int, int);

	void UpdateModels();

	void SetCameraPosition(float, float, float);
	void SetCameraRotation(float, float, float);
	void SetCameraSensitivity(float&);
	void SetCameraSpeed(float&);
private:
	bool Render();

private:
	// DirectX
	D3DClass* d3d_ = nullptr;

	// Camera
	CameraClass* camera_ = nullptr;
	float cameraSensitivity_ = 0.01f;
	float cameraSpeed_ = 0.3f;

	LightClass* light_ = nullptr;

	// Shader
	ColorShaderClass* colorShader_ = nullptr;

	// LSystem, Model
	LSystem* lSystem_ = nullptr;
	std::vector<ModelClass*>* models_ = nullptr;
};