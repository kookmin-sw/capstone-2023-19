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

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
    this->diffuseColor_ = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(float x, float y, float z)
{
    this->direction_ = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT4 LightClass::GetDiffuseColor()
{
    return this->diffuseColor_;
}

DirectX::XMFLOAT3 LightClass::GetDirection()
{
    return this->direction_;
}

