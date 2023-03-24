#pragma once

// Global
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class ModelClass;
class ColorShaderClass;		// !!! TEMP
class TextureShaderClass;	// !!! TEMP
class LightShaderClass;
class LightClass;
class LSystem;
class TextClass;
//class TextConsoleClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, LSystem* = nullptr);
	void Shutdown();
	bool Frame(int, int, int, int, int, int, int);		// !!! 나중에 키 입력 파라미터 조정
	bool Render();
	
private:
	D3DClass* direct3D_ = nullptr;
	CameraClass* camera_ = nullptr;
	TextClass* text_ = nullptr;
	LightClass* light_ = nullptr;

	std::vector<ModelClass*>* models_ = nullptr;
	ColorShaderClass* colorShader_ = nullptr;		// !!! TEMP
	TextureShaderClass* textureShader_ = nullptr;	// !!! TEMP
	LightShaderClass* lightShader_ = nullptr;

	//TextConsoleClass* console_;

	LSystem* lSystem_ = nullptr;

	float rotation_ = 0.0f;
};