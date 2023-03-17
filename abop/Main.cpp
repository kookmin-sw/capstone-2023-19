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

	//lSystem->SetWord("A");
	//lSystem->AddRule('A', "B - F + CFC + F - D & F^D - F + && CFC + F + B//");
	//lSystem->AddRule('B', "A & F^CFB^F^D^^ - F - D^ | F^B | FC^F^A//");
	//lSystem->AddRule('C', "D^ | F^B - F + C^F^A && FA & F^C + F + B^F^D//");
	//lSystem->AddRule('D', "CFB - F + B | FA & F^A && FB - F + B | FC//");
	//lSystem->SetDistance(2.0f);
	//lSystem->SetAngleChange(90.0f);
	//lSystem->Iterate(2);
	//lSystem->GetWord();
	//lSystem->SetWord("&F^F^F^^^-F-^|F^|F^F^//-F+^|F^-F+^F^&&F&F^+F+^F^//F^|F^-F+^F^&&F&F^+F+^F^//+F-F-F+|F&F^&&F-F+|F//&F^F-F+|F&F^&&F-F+|F//-F+&&^|F^-F+^F^&&F&F^+F+^F^//F^|F^-F+^F^&&F&F^+F+^F^//+F+&F^F^F^^^-F-^|F^|F^F^////");


	//lSystem->SetDistance(3.0f);
	//lSystem->SetAngleChange(22.5f);
	//lSystem->Iterate(7);


	lSystem->SetWord("F&FFF^F--F--FFF^F");
	lSystem->SetAngleChange(45.0f);

	//lSystem->SetWord("{[++++G.][++GG.][+GGG.][GGGGG.][-GGG.][--GG.][----G.]}");
	//lSystem->SetAngleChange(30.0f);

	SystemClass* system = new SystemClass;
	if (!system)
	{
		return -1;
	}

	//if (system->Initialize(lSystem))
	if (system->Initialize())
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