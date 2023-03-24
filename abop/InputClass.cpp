#include "Stdafx.h"
#include "InputClass.hpp"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	this->screenWidth_ = screenWidth;
	this->screenHeight_ = screenHeight;

	// Direct input 인터페이스 초기화
	HRESULT result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8
		, (void**)&(this->directInput_), NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드 인터페이스 생성
	result = this->directInput_->CreateDevice(GUID_SysKeyboard, &(this->keyboard_), NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식 설정
	result = this->keyboard_->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// 협조 설정
	result = this->keyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = this->keyboard_->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스도 키보드와 동일하게 설정
	result = this->directInput_->CreateDevice(GUID_SysMouse, &(this->mouse_), NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = this->mouse_->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	result = this->mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = this->mouse_->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if (this->mouse_)
	{
		this->mouse_->Unacquire();
		this->mouse_->Release();
		this->mouse_ = nullptr;
	}

	if (this->keyboard_)
	{
		this->keyboard_->Unacquire();
		this->keyboard_->Release();
		this->keyboard_ = nullptr;
	}

	if (this->directInput_)
	{
		this->directInput_->Release();
		this->directInput_ = nullptr;
	}
}

bool InputClass::Frame()
{
	if (!ReadKeyboard())
	{
		return false;
	}

	if (!ReadMouse())
	{
		return false;
	}

	// 키보드와 마우스의 변경상태를 처리
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	// 키보드 디바이스
	HRESULT result = this->keyboard_->GetDeviceState(sizeof(this->keyboardState_)
		, (LPVOID) & (this->keyboardState_));

	if (FAILED(result))
	{
		// 키보드가 포커스를 잃은 경우 다시 가져옴
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			this->keyboard_->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	// 마우스 디바이스를 얻는다.
	HRESULT result = this->mouse_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&(this->mouseState_));
	if (FAILED(result))
	{
		// 마우스 포커스를 잃은 경우 다시 가져옴
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			this->mouse_->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// 마우스 커서 위치 업데이트
	this->mouseX_ += this->mouseState_.lX;
	this->mouseY_ += this->mouseState_.lY;

	if (this->mouseX_ < 0) { this->mouseX_ = 0; }
	if (this->mouseY_ < 0) { this->mouseY_ = 0; }

	if (this->mouseX_ > this->screenWidth_) { this->mouseX_ = this->screenWidth_; }
	if (this->mouseY_ > this->screenHeight_) { this->mouseY_ = this->screenHeight_; }
}

bool InputClass::IsEscapePressed()
{
	if (this->keyboardState_[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = this->mouseX_;
	mouseY = this->mouseY_;
}

void InputClass::GetCameraMove(int& forward, int& right, int& pitchUp, int& rotationRight, int& up)
{
	// forward, right, pitch up, rotate right
	int f = 0, r = 0, pu = 0, rr = 0;
	int u = 0;

	if (this->keyboardState_[DIK_W] & 0x80)
	{
		f += 1;
	}

	if (this->keyboardState_[DIK_S] & 0x80)
	{
		f -= 1;
	}

	if (this->keyboardState_[DIK_A] & 0x80)
	{
		r -= 1;
	}

	if (this->keyboardState_[DIK_D] & 0x80)
	{
		r += 1;
	}

	// !!! TEMP
	if (this->keyboardState_[DIK_UP] & 0x80)
	{
		pu -= 1;
	}

	if (this->keyboardState_[DIK_DOWN] & 0x80)
	{
		pu += 1;
	}
	if (this->keyboardState_[DIK_RIGHT] & 0x80)
	{
		rr += 1;
	}

	if (this->keyboardState_[DIK_LEFT] & 0x80)
	{
		rr -= 1;
	}
	
	// up, down
	if (this->keyboardState_[DIK_LCONTROL] & 0x80)
	{
		u -= 1;
	}

	if (this->keyboardState_[DIK_LSHIFT] & 0x80)
	{
		u += 1;
	}

	// !!! 단위 벡터로 바꿔야 함
	forward = f;
	right = r;
	pitchUp = pu;
	rotationRight = rr;
	up = u;
}

//void InputClass::KeyDown(const unsigned int& input)
//{
//	// 눌릴 때만 true로
//	this->keys_[input] = true;
//}
//
//void InputClass::KeyUp(const unsigned int& input)
//{
//	this->keys_[input] = false;
//}
//
//bool InputClass::IsKeyDown(const unsigned int& input)
//{
//	return this->keys_[input];
//}
