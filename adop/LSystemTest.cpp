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
    // lsystem->SetWord("F-F-F-F");
    // lsystem->AddRule("F", "F-F+F+FF-F-F+F");
    // cout << "[before]\n" + lsystem->GetWord() << endl;
    // lsystem->Iterate(3);
    // cout << "[after]\n" + lsystem->GetWord() << endl;

    // Test 3
    // lsystem->SetWord("F[+F][-F[-F]F]F[+F][-F]");
    // lsystem->SetAngleChange(45.0);
    // vector<LSystem::State>* result = new vector<LSystem::State>();
    // lsystem->GetResultVertex(result);

    // for (const LSystem::State& state : *result)
    // {
    //     cout << "(" << state.position.x << ", " << state.position.y << ") " << state.angle << endl;
    // }

    // Test 4
    lsystem->SetWord("F-F-F-F");
    lsystem->AddRule('F', "F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F");
    lsystem->SetAngleChange(90.0);
    lsystem->Iterate(2);

    vector<LSystem::State>* result = new vector<LSystem::State>();
    lsystem->GetResultVertex(result);
    for (const LSystem::State& state : *result)
    {
        cout << "(" << state.position.x << ", " << state.position.y << ") " << state.angle << endl;
    }

    delete lsystem;
    lsystem = nullptr;

    return 0;
}