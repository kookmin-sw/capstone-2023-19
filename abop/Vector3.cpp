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
