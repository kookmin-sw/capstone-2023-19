#pragma once

class Leaf : public ModelClass
{
public:
    bool Initialize(ID3D11Device*, Model model);

protected:
    bool InitializeBuffers(ID3D11Device*, Model model);

private:
    float angle_ = 22.5f;
};