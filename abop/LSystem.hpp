#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    float angleChange_ = 90;
    float distance_ = 1.0;
    State state_;

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
    void Rotate(const unsigned short&, const float&);
    // void SetPosition(const Vector3&);
};
