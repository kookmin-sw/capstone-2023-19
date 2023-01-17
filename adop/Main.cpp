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
	
	// !!! TEMP
	lSystem->SetWord("F-F-F-F");
    lSystem->AddRule('F', "F-F+F+FF-F-F+F");
    lSystem->SetAngleChange(90.0);
    lSystem->Iterate(3);

    // std::vector<LSystem::State>* result = new std::vector<LSystem::State>();
    // lSystem->GetResultVertex(result);
    // for (const LSystem::State& state : *result)
    // {
    //     cout << "(" << state.position.x << ", " << state.position.y << ") " << state.angle << endl;
    // }

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