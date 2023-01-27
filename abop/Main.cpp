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
	
	// 프랙탈
	// Case 1
	//lSystem->SetWord("F-F-F-F");
 //   lSystem->AddRule('F', "F-F+F+FF-F-F+F");
 //   lSystem->SetAngleChange(90.0);
 //   lSystem->Iterate(3);
	
	// Case 2
	//lSystem->SetWord("-F");
 //   lSystem->AddRule('F', "F+F-F-F+F");
 //   lSystem->SetAngleChange(90.0);
 //   lSystem->Iterate(4);

	// Case 3
	//lSystem->SetWord("F-F-F-F");
 //   lSystem->AddRule('F', "F-FF--F-F");
 //   lSystem->SetAngleChange(90.0);
 //   lSystem->Iterate(5);

	// 브랜치
	// Case 1
	//lSystem->SetWord("F");
 //   lSystem->AddRule('F', "FF-[-F+F+F]+[+F-F-F]");
 //   lSystem->SetAngleChange(22.5);
 //   lSystem->Iterate(4);

	// Case 2
	//lSystem->SetWord("F");
 //   lSystem->AddRule('F', "F[+F]F[-F][F]");
 //   lSystem->SetAngleChange(20.0);
 //   lSystem->Iterate(5);

	// 3D Test
	// Case 1
	//lSystem->SetWord("P");
	//lSystem->AddRule('P', "I+[P+L]--//[--L]I[++L]-[PL]++PF");
	//lSystem->AddRule('I', "Fs[//&&L][//^^L]Fs");
	//lSystem->AddRule('s', "sFs");
	//lSystem->AddRule('L', "['{+f-ff-f+|+f-ff}']");
	//lSystem->AddRule('F', "[&&&P'/W////W////W////W////W");
	//lSystem->AddRule('P', "FF");
	//lSystem->AddRule('W', "['^F][{&&&&-f+f|-f+f}]");
	//lSystem->SetAngleChange(18);
	//lSystem->Iterate(5);

	// Case 2
	//lSystem->SetWord("A");
	//lSystem->AddRule('A', " B-F+CFC+F-D&F^D-F + && CFC + F + B//");
	//lSystem->AddRule('B', " A & F^CFB^F^D^^ - F - D^ | F^B | FC^F^A//");
	//lSystem->AddRule('C', " | D^ | F^B - F + C^F^A && FA & F^C + F + B^F^D//");
	//lSystem->AddRule('D', " | CFB - F + B | FA & F^A && FB - F + B | FC//");
	//lSystem->SetAngleChange(90.0f);
	//lSystem->Iterate(2);

	// TEMP 2D TEST
	//lSystem->SetWord("F-G-G");
	//lSystem->AddRule('F', "F-G+F+G-F");
	//lSystem->AddRule('G', "GG");
	//lSystem->SetAngleChange(120.0f);
	//lSystem->Iterate(6);

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