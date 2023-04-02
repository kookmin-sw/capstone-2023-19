#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    float angleChange_ = 90;
    float distance_ = 1.0f;
    float deltaThickness_ = 1.0f;
    float leafAngleChange_ = 22.5f;
    float leafDistance_ = 0.5f;
    bool drawingLeaf_ = false;
    State state_;

    std::vector<LRule> rules_;
    std::vector<LLetter>* word_;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string GetWord() const;       // Word text
    void GetWord(char* out);

    std::vector<LRule> GetRules() const;
    std::string GetRuleText() const;

    float GetAngleChange() const;
    float GetDistance() const;
    float GetLeafAngleChange() const;
    float GetLeafDistance() const;

    void SetAngleChange(const float&);
    void SetDistance(const float&);
    void SetWord(const std::string&);
    void SetThickness(const float&);
    void SetDeltaThickness(const float&);
    void SetLeafAngleChange(const float&);
    void SetLeafDistance(const float&);
    
    // Rule
    void AddRule(const std::string&);
    void AddRule(const char&, const std::string&);
    void AddRule(const std::string&, const std::string&);
    void DeleteRule(const char&);
    void ClearRule();

    void ClearState();

    // Run
    void Iterate();
    void Iterate(int);

    // Result
    void GetResultVertex(std::vector<Model>*);

private:
    void Move();
    void Move(float distance);
    void Rotate(const unsigned short&, const float&);

    DirectX::XMFLOAT3 axisX;
    DirectX::XMFLOAT3 axisY;
    DirectX::XMFLOAT3 axisZ;
};
