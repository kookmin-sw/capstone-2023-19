#pragma once

class Vector3
{
public:
    Vector3();
    Vector3(const float&, const float&, const float&);

public:
    void SetX(const float&);
    void SetY(const float&);
    void SetZ(const float&);

    void Normalized();
    float SqrtMagnitude();
    Vector3& operator+(const Vector3&);
    Vector3& operator-(const Vector3&);
    Vector3& operator*(const float&);
    Vector3& operator/(const float&);

public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};