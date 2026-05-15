#include "Vector2.h"

/* intå^Vector2 */

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



/* floatå^Vector2 */

Vector2F Vector2F::operator=(const Vector2F& vec)
{
	x = vec.x;
	y = vec.y;
	return Vector2F{ vec.x, vec.y };
}


Vector2F Vector2F::operator+(const Vector2F& vec)const
{
	return Vector2F{ (x + vec.x), (y + vec.y) };
}

void Vector2F::operator+=(const Vector2F& vec)
{
	x += vec.x;
	y += vec.y;
}

Vector2F Vector2F::operator-(const Vector2F& vec)const
{
	return Vector2F{ (x - vec.x), (y - vec.y) };
}

void Vector2F::operator-=(const Vector2F& vec)
{
	x -= vec.x;
	y -= vec.y;
}

Vector2F Vector2F::operator*(const Vector2F& vec)const
{
	return Vector2F{ (x * vec.x), (y * vec.y) };
}

void Vector2F::operator*=(const Vector2F& vec)
{
	x *= vec.x;
	y *= vec.y;
}

Vector2F Vector2F::operator/(const Vector2F& vec)const
{
	return Vector2F{ (x / vec.x), (y / vec.y) };
}

void Vector2F::operator/=(const Vector2F& vec)
{
	x /= vec.x;
	y /= vec.y;
}