#pragma once

class Triangle : public ModelClass
{
public:
    void SetWidth(const float&);
    void SetHeight(const float*);

    // override
    bool Initialize(ID3D11Device*);
protected:
    // override
    bool InitializeBuffers(ID3D11Device*);

private:
    float width_ = 1;
    float height_ = 1;
};