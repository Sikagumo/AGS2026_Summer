#include "Vector2.h"



Vector2 Vector2::operator=(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	return Vector2{ vec.x, vec.y };
}


Vector2 Vector2::operator+(const Vector2& vec)const
{
	return Vector2{ (x + vec.x), (y + vec.y) };
}

void Vector2::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;
}

Vector2 Vector2::operator-(const Vector2& vec)const
{
	return Vector2{ (x - vec.x), (y - vec.y) };
}

void Vector2::operator-=(const Vector2& vec)
{
	x -= vec.x;
	y -= vec.y;
}

Vector2 Vector2::operator*(const Vector2& vec)const
{
	return Vector2{ (x * vec.x), (y * vec.y) };
}

void Vector2::operator*=(const Vector2& vec)
{
	x *= vec.x;
	y *= vec.y;
}

Vector2 Vector2::operator/(const Vector2& vec)const
{
	return Vector2{ (x / vec.x), (y / vec.y) };
}

void Vector2::operator/=(const Vector2& vec)
{
	x /= vec.x;
	y /= vec.y;
}