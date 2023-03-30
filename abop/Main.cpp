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

	// Hilbert Curve 3D
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

	// Leaf - 2D (CANNOT USE)
	//lSystem->SetWord("{[++++G.][++GG.][+GGG.][GGGGG.][-GGG.][--GG.][----G.]}");
	//lSystem->SetAngleChange(30.0f);

	// Simple Tree
	//lSystem->SetWord("F");
	//lSystem->AddRule('F', "F[-&\\F][\\++&F]F[--&/F][+&F]");
	//lSystem->SetAngleChange(22.5f);
	//lSystem->SetDistance(1.0f);
	//lSystem->SetThickness(0.5f);
	//lSystem->SetDeltaThickness(0.9f);
	//lSystem->Iterate(4);
	//lSystem->Iterate(7);


	//lSystem->SetWord("F&FFF^F--F--FFF^F");
	//lSystem->SetAngleChange(45.0f);

	//lSystem->SetWord("{[++++G.][++GG.][+GGG.][GGGGG.][-GGG.][--GG.][----G.][++++G.]}");
	//lSystem->SetAngleChange(30.0f);

	//lSystem->SetWord("[{+.G.{.&G.{.&G.][-G[&G[&G.].}.].}.}]F^F");
	//lSystem->SetLeafAngleChange(25.0f);
	//lSystem->SetAngleChange(90.0f);
	//lSystem->SetLeafDistance(1.0f);
	//lSystem->SetLeafAngleChange(25.0f);

	//lSystem->SetWord("FL");
	//lSystem->AddRule("L", "{++[+G.-G.-G.----G.-G.]}}{----[+G.-G.-G.----G.-G.]}}");
	//lSystem->SetAngleChange(45.0f);
	//lSystem->SetLeafAngleChange(30.f);
	//lSystem->Iterate(1);

	//lSystem->SetWord("A");
	//lSystem->AddRule("A", "[&FLA]/////`[&FLA]///////`[&FLA]");
	//lSystem->AddRule("F", "S/////F");
	//lSystem->AddRule("S", "FL");
	//lSystem->AddRule("L", "[```^^{-G.+G.+G.-|-G.+G.+G.}]");
	//lSystem->SetLeafAngleChange(22.5f);
	//lSystem->SetLeafDistance(0.3f);
	//lSystem->SetDistance(2.0f);
	//lSystem->SetDeltaThickness(0.9f);
	//lSystem->SetAngleChange(22.5f);
	//lSystem->Iterate(7);

	lSystem->SetWord("F");
	lSystem->AddRule('F', "F[-&\\[{-G.+G.+G.-|-G.+G.+G.}]FL][\\++&F[{-G.+G.+G.-|-G.+G.+G.}]L]F[--&/F[{-G.+G.+G.-|-G.+G.+G.}]L][+&F[{-G.+G.+G.-|-G.+G.+G.}]L]");
	lSystem->AddRule('L', "[++{-G.+G.+G.-|-G.+G.+G.}]S");
	lSystem->AddRule('S', "[--{-G.+G.+G.-|-G.+G.+G.}]L");
	lSystem->SetLeafAngleChange(22.5f);
	lSystem->SetLeafDistance(0.3f);
	lSystem->SetAngleChange(22.5f);
	lSystem->SetDistance(1.5f);
	lSystem->SetThickness(0.5f);
	lSystem->SetDeltaThickness(0.9f);
	lSystem->Iterate(4);

	SystemClass* system = new SystemClass;
	if (!system)
	{
		return -1;
	}

	if (system->Initialize(lSystem))
	//if (system->Initialize())
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