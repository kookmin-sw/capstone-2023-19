#pragma once

// Global
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class ColorShaderClass;
class LSystem;
class TextClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, LSystem* = nullptr);
	void Shutdown();
	bool Frame(int, int, int, int, int, int);		// !!! ���߿� Ű �Է� �Ķ���� ����
	bool Render();
	
private:
	D3DClass* direct3D_ = nullptr;
	CameraClass* camera_ = nullptr;
	TextClass* text_ = nullptr;

	std::vector<ModelClass*>* models_ = nullptr;
	ColorShaderClass* colorShader_ = nullptr;

	LSystem* lSystem_ = nullptr;
};