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
	LSystem* lSystem = new LSystem();
	if (!lSystem)
	{
		return -1;
	}

	lSystem->SetWord("FF-FF+FF+FFFF+FF");
	lSystem->SetDistance(3.0f);
	//lSystem->SetWord("{[++++G.][++GG.][+GGG.][GGGGG.][-GGG.][--GG.][----G.]}");

	SystemClass* system = new SystemClass;
	if (!system)
	{
		return -1;
	}

	if (system->Initialize(lSystem))
	{
		system->Run();
	}

    delete lSystem;
    lSystem = nullptr;

	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}