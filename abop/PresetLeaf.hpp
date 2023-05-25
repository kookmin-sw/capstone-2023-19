#pragma once

class PresetLeaf : public ModelClass
{
public:
    bool Initialize(ID3D11Device*, Model model);
    void SetType(const int&);
    void SetScale(const float&);

protected:
    bool InitializeBuffers(ID3D11Device*, Model model);

private:
    int mType = 1;
    float mScale = 1.0f;
};