#pragma once

class D3DClass;
class CameraClass;
class ModelClass;
class LSystem;
class ColorShaderClass;
//class LightClass;

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
	D3DClass* d3d_ = nullptr;
	CameraClass* camera_ = nullptr;

	LSystem* lSystem_ = nullptr;
	std::vector<ModelClass*>* models_ = nullptr;

	ColorShaderClass* colorShader_ = nullptr;
};