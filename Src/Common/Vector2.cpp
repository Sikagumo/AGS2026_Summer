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
void Vector2::operator*=(int _value)
{
	x *= _value;
	y *= _value;
}

Vector2 Vector2::operator/(const Vector2& _vec)const
{
	Vector2 ret = _vec;

	// É[ÉçèúéZëŒçÙ
	ret.x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	ret.y = ((_vec.y != 0) ? (y / _vec.y) : 0);

	return ret;
}

void Vector2::operator/=(const Vector2& _vec)
{
	// É[ÉçèúéZëŒçÙ
	x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	y = ((_vec.y != 0) ? (y / _vec.y) : 0);
}

void Vector2::operator/=(int _value)
{
	// É[ÉçèúéZëŒçÙ
	x = ((_value != 0) ? (x / _value) : 0);
	y = ((_value != 0) ? (y / _value) : 0);
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
void Vector2F::operator*=(float _value)
{
	x *= _value;
	y *= _value;
}

Vector2F Vector2F::operator/(const Vector2F& _vec)const
{
	Vector2F ret = _vec;

	// É[ÉçèúéZëŒçÙ
	ret.x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	ret.y = ((_vec.y != 0) ? (y / _vec.y) : 0);

	return ret;
}
void Vector2F::operator/=(const Vector2F& _vec)
{
	x /= _vec.x;
	y /= _vec.y;
}
void Vector2F::operator/=(float _value)
{
	// É[ÉçèúéZëŒçÙ
	x = ((_value != 0) ? (x / _value) : 0);
	y = ((_value != 0) ? (y / _value) : 0);
}
