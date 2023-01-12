#pragma once

class InputClass;
class GraphicsClass;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName_;
	HINSTANCE hInstance_;
	HWND hwnd_;

	InputClass* input_ = nullptr;
	GraphicsClass* graphics_ = nullptr;
};

static LRESULT CALLBACK SWndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* sApplicationHandle = nullptr;
