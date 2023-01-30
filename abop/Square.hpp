#pragma once

class Square : public ModelClass
{
public:
    bool Initialize(ID3D11Device*);

protected:
    bool InitializeBuffers(ID3D11Device*);

public:
    void SetWidth(const float&);
    void SetHeight(const float&);

private:
    float width_ = 1;
    float height_ = 1;
};