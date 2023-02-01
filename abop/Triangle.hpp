#pragma once

class Triangle : public ModelClass
{
public:
    // override
    bool Initialize(ID3D11Device*);

protected:
    // override
    bool InitializeBuffers(ID3D11Device*);

public:
    void SetWidth(const float&);
    void SetHeight(const float&);

private:
    float width_ = 1;
    float height_ = 1;
};