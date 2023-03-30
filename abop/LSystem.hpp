#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    float angleChange_ = 90;
    float distance_ = 1.0f;
    float deltaThickness_ = 1.0f;
    State state_;

    std::vector<LRule> rules_;
    std::vector<LLetter>* word_;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string GetWord() const;       // Word text
    char* GetWordIntoChar() const;
    std::string GetRules() const;      // Rules text

    std::string GetRuleText() const;
    std::vector<LRule>* GetRules();

    float GetAngleChange() const;
    float GetDistance() const;

    void SetAngleChange(const float&);
    void SetDistance(const float&);
    void SetWord(const std::string&);
    void SetThickness(const float&);
    void SetDeltaThickness(const float&);
    
    // Rule
    void AddRule(const std::string&);
    void AddRule(const char&, const std::string&);
    void AddRule(const std::string&, const std::string&);

    void ClearState();

    // Run
    void Iterate();
    void Iterate(int);

    // Result
    void GetResultVertex(std::vector<Model>*);

private:
    void Move();
    void Rotate(const unsigned short&, const float&);

    DirectX::XMFLOAT3 axisX;
    DirectX::XMFLOAT3 axisY;
    DirectX::XMFLOAT3 axisZ;
};
