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
private:
	bool Render();

private:
	// DirectX
	D3DClass* d3d_ = nullptr;
	CameraClass* camera_ = nullptr;

	LightClass* light_ = nullptr;

	// Shader
	ColorShaderClass* colorShader_ = nullptr;

	// LSystem, Model
	LSystem* lSystem_ = nullptr;
	std::vector<ModelClass*>* models_ = nullptr;
};