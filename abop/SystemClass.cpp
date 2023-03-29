#include <vector>
#include <string>
#include "Stdafx.h"
#include "InputClass.hpp"
#include "GraphicsClass.hpp"
#include "FpsClass.hpp"
#include "SystemClass.hpp"
#include "LSystem.hpp"

// Public
SystemClass::SystemClass()
{

}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}


bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	this->InitializeWindows(screenWidth, screenHeight);

	this->input_ = new InputClass;
	if (!this->input_)
	{
		return false;
	}

	if (!this->input_->Initialize(this->hInstance_, this->hwnd_, screenWidth, screenHeight))
	{
		MessageBox(this->hwnd_, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	this->graphics_ = new GraphicsClass;
	if (!this->graphics_)
	{
		return false;
	}

	this->fps_ = new FpsClass;
	if (!this->fps_)
	{
		return false;
	}

	this->fps_->Initialize();

	return this->graphics_->Initialize(screenWidth, screenHeight, this->hwnd_);
}

bool SystemClass::Initialize(LSystem* lSystem)
{
	// Main의 LSystem 포인터 할당
	if (!lSystem)
	{
		return false;
	}
	this->lSystem_ = lSystem;
	
	int screenWidth = 0;
	int screenHeight = 0;

	this->InitializeWindows(screenWidth, screenHeight);

	this->input_ = new InputClass;
	if (!this->input_)
	{
		return false;
	}

	if (!this->input_->Initialize(this->hInstance_, this->hwnd_, screenWidth, screenHeight))
	{
		MessageBox(this->hwnd_, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	this->fps_ = new FpsClass;
	if (!this->fps_)
	{
		return false;
	}

	this->fps_->Initialize();
	
	this->graphics_ = new GraphicsClass;
	if (!this->graphics_)
	{
		return false;
	}

	// GraphicsClass에도 LSystem 포인터 할당
	return this->graphics_->Initialize(screenWidth, screenHeight, this->hwnd_, this->lSystem_);
}

void SystemClass::Shutdown()
{
	if (this->fps_)
	{
		delete this->fps_;
		this->fps_ = 0;
	}
	
	if (this->graphics_)
	{
		this->graphics_->Shutdown();
		delete this->graphics_;
		this->graphics_ = nullptr;
	}

	if (this->input_)
	{
		delete this->input_;
		this->input_ = nullptr;
	}

	this->ShutdownWindows();
}
void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		// 메시지 입력 루프
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
			{
				// 메시지 입력 외에는 Frame 함수 처리
				MessageBox(this->hwnd_, L"Frame Processing Failed.", L"Error", MB_OK);
				break;
			}
		}

		if (this->input_->IsEscapePressed())
		{
			// ESC 누르면 종료
			break;
		}
	}
}

LRESULT CALLBACK SystemClass::MessageHandler(
	HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

// Private
bool SystemClass::Frame()
{
	int mouseX = 0, mouseY = 0;
	int forward = 0, right = 0;
	int pitchUp = 0, rotationRight = 0;
	int up = 0;
	
	// fps 통계 업데이트
	this->fps_->Frame();

	if (!this->input_->Frame())
	{
		return false;
	}

	// mouseX, mouseY로 마우스 위치 가져옴
	this->input_->GetMouseLocation(mouseX, mouseY);

	this->input_->GetCameraMove(forward, right, pitchUp, rotationRight, up);

	// 그래픽 Frame 처리
	if (!this->graphics_->Frame(
		mouseX, mouseY, 
		forward, right, pitchUp, rotationRight, up, 
		this->fps_->GetFps()))
	{
		return false;
	}

	return this->graphics_->Render();
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// 외부 static 포인터를 이 객체로 지정
	sApplicationHandle = this;

	// 이 프로그램의 인스턴스
	this->hInstance_ = GetModuleHandle(NULL);

	this->applicationName_ = L"abop";

	// window 클래스 설정
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = SWndProc;		// SystemClass.hpp에 static 선언
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance_;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->applicationName_;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// FULL SCREEN 모드인 경우, 모니터 화면 해상도를 데스트콥 해상도로 지정하고 색상을 32bit로 지정
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// Window 모드인 경우 800 * 600 해상도 지정
		screenWidth = 800;
		screenHeight = 600;

		// 윈도우 창을 가운데 배치
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 윈도우 생성 후 핸들 저장
	//this->hwnd_ = CreateWindowEx(
	//	WS_EX_APPWINDOW, this->applicationName_, this->applicationName_,
	//	WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	//	posX, posY, screenWidth, screenHeight, NULL, NULL, this->hInstance_, NULL);

	this->hwnd_ = CreateWindowEx(
		WS_EX_APPWINDOW, this->applicationName_, this->applicationName_,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		posX, posY, screenWidth, screenHeight, NULL, NULL, this->hInstance_, NULL);

	// 윈도우 표시 및 포커스 지정
	ShowWindow(this->hwnd_, SW_SHOW);
	SetForegroundWindow(this->hwnd_);
	SetFocus(this->hwnd_);
}

void SystemClass::ShutdownWindows()
{
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창 제거
	DestroyWindow(this->hwnd_);
	this->hwnd_ = nullptr;

	// 프로그램 인스턴스 제거
	UnregisterClass(this->applicationName_, this->hInstance_);
	this->hInstance_ = nullptr;

	// static 포인터 참조 제거
	sApplicationHandle = nullptr;
}

LRESULT CALLBACK SWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	char szText[100];
	static HWND hEditbox;

	switch (umsg)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			// 윈도우 종료 및 닫기 확인
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			// 이 외 메시지는 SystemClass 메시지 처리로 전달
			return sApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}