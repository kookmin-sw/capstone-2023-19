#include <iostream>
#include <vector>
#include "CommonStructure.hpp"
#include "LSystem.hpp"

using namespace std;

void PrintVector(const Vector3& vec, std::string key)
{
    std::cout << key << ": " << vec.x << " " << vec.y << " " << vec.z << std::endl;
}

void PrintVector(const Vector4& vec, std::string key)
{
    std::cout << key << ": " << vec.w << " " <<  vec.x << " " << vec.y << " " << vec.z << std::endl;
}

int main()
{
    LSystem* lSystem = new LSystem();

    //lSystem->SetWord("+(50)\\(333)^(111)F");
    //lSystem->SetWord("F+(45)\\(45)FF");
    lSystem->SetWord("F");
    lSystem->AddRule("F", "F[-&\\F][\\++&F]F[--&/F][+&F]");
    //lSystem->AddRule("X", "F-[[X]+X]+F[+FX]-X");
    //lSystem->AddRule("F", "FF");

    // lSystem->AddRule("A", "B-F+CFC+F-D&F^D-F+&&CFC+F+B//");
    // lSystem->AddRule("B", "A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//");
    // lSystem->AddRule("C", "|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//");
    // lSystem->AddRule("A", "|CFB-F+B|FA&F^A&&FB-F+B|FC//");
    lSystem->SetAngleChange(90.0);
    lSystem->Iterate(2);

    cout << lSystem->GetWord() << endl;

    std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
    lSystem->GetResultUEInfos(models);

    for (const LSystem::UEModel& model : *models)
    {
        std::cout << model.type << std::endl;
        PrintVector(model.position, "position");
        PrintVector(model.rotation, "rotation");
        PrintVector(model.scale, "scale");
        std::cout << std::endl;
    }

    delete lSystem;
    lSystem = nullptr;

    return 0;
}