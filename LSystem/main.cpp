#include <iostream>
#include <vector>
#include "LSystem.hpp"

using namespace std;

int main()
{
    LSystem* lsystem = new LSystem();
    lsystem->SetWord("F-F-F-F");
    lsystem->AddRule("F", "F-F+F+FF-F-F+F");
    cout << lsystem->get_word() << endl;
    lsystem->Iterate();
    cout << lsystem->get_word() << endl;
    lsystem->Iterate();
    cout << lsystem->get_word() << endl;
    lsystem->Iterate();
    cout << lsystem->get_word() << endl;

    delete lsystem;
    lsystem = nullptr;

    return 0;
}