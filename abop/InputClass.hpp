#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	void GetCameraMove(int&, int&, int&, int&);

	//void KeyDown(const unsigned int&);
	//void KeyUp(const unsigned int&);

	//bool IsKeyDown(const unsigned int&);
private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* directInput_ = nullptr;
	IDirectInputDevice8* keyboard_ = nullptr;
	IDirectInputDevice8* mouse_ = nullptr;

	// 8비트 정수형으로 사용
	unsigned char keyboardState_[256] = { 0, };
	DIMOUSESTATE mouseState_;
	
	int screenWidth_ = 0;
	int screenHeight_ = 0;
	int mouseX_ = 0;
	int mouseY_ = 0;
};
