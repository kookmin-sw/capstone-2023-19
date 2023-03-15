#pragma once

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);

    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT3 GetDirection();

private:
    DirectX::XMFLOAT4 diffuseColor_;
    DirectX::XMFLOAT3 direction_;
};