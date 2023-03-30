#pragma once

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

    void SetAmbientColor(float r, float g, float b, float a);
    void SetDiffuseColor(float r, float g, float b, float a);
    void SetDirection(float x, float y, float z);
    void SetSpecularColor(float r, float g, float b, float a);
    void SetSpecularPower(float power);

    DirectX::XMFLOAT4 GetAmbientColor();
    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT3 GetDirection();
    DirectX::XMFLOAT4 GetSpecularColor();
    float GetSpecularPower();

private:
    DirectX::XMFLOAT4 ambientColor_;
    DirectX::XMFLOAT4 diffuseColor_;
    DirectX::XMFLOAT3 direction_;
    DirectX::XMFLOAT4 specularColor_;
    float specularPower_;
};