#include <iostream>
#include <vector>
#include "LSystem.hpp"

using namespace std;

int main()
{
    LSystem* lsystem = new LSystem();

    // // Test 1
    // lsystem->SetWord("b");
    // lsystem->AddRule("a", "ab");
    // lsystem->AddRule("b", "a");
    // cout << "[before]\n" + lsystem->get_word() << endl;
    // lsystem->Iterate(5);
    // cout << "[after]\n" + lsystem->get_word() << endl;

    // Test 2
    lsystem->SetWord("F-F-F-F");
    lsystem->AddRule("F", "F-F+F+FF-F-F+F");
    cout << "[before]\n" + lsystem->get_word() << endl;
    lsystem->Iterate(3);
    cout << "[after]\n" + lsystem->get_word() << endl;

    delete lsystem;
    lsystem = nullptr;

    return 0;
}