#pragma once

// !!! Àü¹æ¼±¾ð ¿¡·¯ ¶§¹®¿¡ ÀÓ½Ã·Î Ãß°¡
#include <map>

class LRule;
class LLetter;

struct info
{
    char letter;
    int origIndex;
};

class LSystem
{
public:
    //struct UEState
    //{
    //    Vector3 position;
    //    Vector3 direction;    // ë°©í–¥ ë‹¨ìœ„ ë²¡í„°
    //    DirectX::XMVECTOR quaternion;
    //    float thickness;
    //};

    struct UEModel
    {
        int type;
        Vector3 position;
        Vector4 rotation;
        Vector3 scale;
    };

private:
    float mAngleChange = 90;
    float mDistance = 1.0f;
    float mDeltaThickness = 1.0f;
    float mLeafAngleChange = 22.5f;
    float mLeafDistance = 0.5f;
    float mThickness = 0.3f;
    bool mDrawingLeaf = false;
    Vector3 mLeafDirection;
    State mState;

    DirectX::XMFLOAT3 axisX;
    DirectX::XMFLOAT3 axisY;
    DirectX::XMFLOAT3 axisZ;

    std::map<std::string, LRule> mRules;
    std::vector<LLetter> mWord;

    std::map<char, bool> mIgnores;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string GetWord() const;       // Word text
    void GetWord(char* out);

    // { before, LRule } pair
    std::map<std::string, LRule> GetRules();

    float GetAngleChange() const;
    float GetDistance() const;
    float GetThickness() const;
    float GetDeltaThickness() const;
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
    void AddRule(std::string, const std::string&);
    void DeleteRule(const std::string& key, const int& afterId);
    void ClearRule();

    void ClearState();

    // Ignore
    std::map<char, bool> GetIgnores();
    void AddIgnore(char symbol);
    void DeleteIgnore(char symbol);
    
    // Run
    void Iterate();
    void Iterate(int);

    // Result
    void GetResultVertex(std::vector<Model>*);
    void GetResultUEInfos(std::vector<UEModel>*);

    // Preset
    void LoadPreset(std::string&);

    bool LoadModel(const char*, std::vector<Model>*, float, Vector4&);

private:
    void Move();
    void MoveParam(const float& distance);
    void Move(float distance);
    void Rotate(const unsigned short&, const float&);

    void Reset();
    void ResetState();

    // Model °ü·Ã
    Model CreateTrunk(Vector3&, Vector3&, DirectX::XMVECTOR&, const float&, const float&);
    Model CreateCylinder(Vector3&, Vector3&, DirectX::XMVECTOR&, const float&, const float&, const int&);
	Model CreateLeaf(std::vector<Vector3>*, Vector3&);
    Model CreateLeafSegment(std::vector<Vector3>*);
    Model CreateLeafPreset(Vector3&, DirectX::XMVECTOR&, const int&, const float&); 
    Model CreateModel(std::vector<TextureVertex>*, float, Vector4&);
};
