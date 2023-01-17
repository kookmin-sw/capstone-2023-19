#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    const double pi = 3.14159265358979;

    float angleChange_ = 90;
    float distance_ = 1.0;
    State state_;
    // Vector3 position_ = Vector3 { 0, 0, 0 };
    // Vector3 forward = Vector3 { 0, 1.0, 0 };

    std::vector<LRule> rules_;
    std::vector<LLetter>* word_;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string GetWord() const;       // Word text
    std::string GetRules() const;      // Rules text
    float GetAngleChange() const;
    float GetDistance() const;

    void SetAngleChange(const float&);
    void SetDistance(const float&);
    void SetWord(const std::string&);
    
    // Rule
    void AddRule(const std::string&);
    void AddRule(const char&, const std::string&);
    void AddRule(const std::string&, const std::string&);

    // Run
    void Iterate();
    void Iterate(int);

    // Result
    void GetResultVertex(std::vector<Model>*);

private:
    void Move();
    void Turn(const bool& isLeft = true);
    // void SetPosition(const Vector3&);
};
