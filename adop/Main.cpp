#include <vector>
#include <string>
#include "Stdafx.h"
#include "SystemClass.hpp"
#include "LSystem.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	SystemClass* system = new SystemClass;
	if (!system)
	{
		return -1;
	}

	if (system->Initialize())
	{
		system->Run();
	}

	LSystem* temp = new LSystem;

	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}