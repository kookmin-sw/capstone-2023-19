#include <iostream>
#include <vector>
#include "CommonStructure.hpp"
#include "LSystem.hpp"

using namespace std;

int main()
{
    LSystem* lSystem = new LSystem();

    lSystem->SetWord("X");
    lSystem->AddRule("X", "F-[[X]+X]+F[+FX]-X");
    lSystem->AddRule("F", "FF");

    // lSystem->AddRule("A", "B-F+CFC+F-D&F^D-F+&&CFC+F+B//");
    // lSystem->AddRule("B", "A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//");
    // lSystem->AddRule("C", "|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//");
    // lSystem->AddRule("A", "|CFB-F+B|FA&F^A&&FB-F+B|FC//");
    lSystem->SetAngleChange(90.0);
    lSystem->Iterate(5);

    cout << lSystem->GetWord() << endl;

    delete lSystem;
    lSystem = nullptr;

    return 0;
}