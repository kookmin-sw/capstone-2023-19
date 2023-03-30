#include "Stdafx.h"
#include "LightClass.hpp"

LightClass::LightClass()
{

}

LightClass::LightClass(const LightClass& other)
{

}

LightClass::~LightClass()
{

}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
    this->ambientColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
    this->diffuseColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(float x, float y, float z)
{
    this->direction_ = DirectX::XMFLOAT3(x, y, z);
}

void LightClass::SetSpecularColor(float r, float g, float b, float a)
{
    this->specularColor_ = DirectX::XMFLOAT4(r, g, b, a);
}

void LightClass::SetSpecularPower(float power)
{
    this->specularPower_ = power;
}

DirectX::XMFLOAT4 LightClass::GetAmbientColor()
{
    return this->ambientColor_;
}

DirectX::XMFLOAT4 LightClass::GetDiffuseColor()
{
    return this->diffuseColor_;
}

DirectX::XMFLOAT3 LightClass::GetDirection()
{
    return this->direction_;
}

DirectX::XMFLOAT4 LightClass::GetSpecularColor()
{
    return this->specularColor_;
}

float LightClass::GetSpecularPower()
{
    return this->specularPower_;
}

