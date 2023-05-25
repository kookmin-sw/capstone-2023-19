#include <cmath>
#include "Vector3.hpp"

Vector3::Vector3()
{

}

Vector3::Vector3(const float& x, const float& y, const float& z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::SetX(const float& x)
{
	this->x = x;
}

void Vector3::SetY(const float& y)
{
	this->y = y;
}

void Vector3::SetZ(const float& z)
{
	this->z = z;
}

void Vector3::Normalized()
{
	float length = this->SqrtMagnitude();
	this->operator/(length);
}

float Vector3::SqrtMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

float Vector3::Dot(const Vector3& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

Vector3 Vector3::Cross(const Vector3& vec)
{
	float newX = y * vec.z - z * vec.y;
	float newY = z * vec.x - x * vec.z;
	float newZ = x * vec.y - y * vec.x;
	x = newX; y = newY; z = newZ;
	return { newX, newY, newZ };
}


Vector3& Vector3::operator+(const Vector3& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;

	return *this;
}

Vector3& Vector3::operator-(const Vector3& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;

	return *this;
}

Vector3& Vector3::operator*(const float& op)
{
	this->x *= op;
	this->y *= op;
	this->z *= op;

	return *this;
}

Vector3& Vector3::operator/(const float& op)
{
	this->x /= op;
	this->y /= op;
	this->z /= op;

	return *this;
}

bool Vector3::operator==(const Vector3& vec)
{
	return (this->x == vec.x && this->y == vec.y && this->z == vec.z);
}

bool Vector3::operator!=(const Vector3& vec)
{
	return !(*this == vec);
}

Vector3& operator%(const Vector3& vec1, const Vector3& vec2)
{
	Vector3 v = vec1;
	v.Cross(vec2);
	return v;
}
