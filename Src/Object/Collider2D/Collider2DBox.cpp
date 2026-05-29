#include "Collider2DBox.h"

Collider2DBox::Collider2DBox(const Vector2F& _centerPos, float _width, float _height, TAG_2D _collisionTag)
	: Collider2DBase(Collider2DBase::SHAPE_2D::BOX, _collisionTag)
	, width_(_width)
	, height_(_height)
	, centerPos_(_centerPos)
{
}

Vector2F Collider2DBox::GetCenterPos(void) const
{
	return GetWorldCenterPos();
}

void Collider2DBox::SetWidth(float _width)
{
	width_ = _width;
}

void Collider2DBox::SetHeight(float _height)
{
	height_ = _height;
}

Vector2F Collider2DBox::GetWorldCenterPos(void) const
{
	return centerPos_;
}

float Collider2DBox::GetWidth(void) const
{
	return width_;
}

float Collider2DBox::GetHeight(void) const
{
	return height_;
}

Vector2F Collider2DBox::GetLeftTop(void) const
{
	Vector2F center = GetWorldCenterPos();
	float halfWidth = width_ / 2.0f;
	float halfHeight = height_ / 2.0f;

	return Vector2F(center.x - halfWidth, center.y - halfHeight);
}

Vector2F Collider2DBox::GetRightBottom(void) const
{
	Vector2F center = GetWorldCenterPos();
	float halfWidth = width_ / 2.0f;
	float halfHeight = height_ / 2.0f;

	return Vector2F(center.x + halfWidth, center.y + halfHeight);
}

void Collider2DBox::DrawDebug(int _color) const
{
	Vector2F leftTop = GetLeftTop();
	Vector2F rightBottom = GetRightBottom();

	DrawBox(
		static_cast<int>(leftTop.x), static_cast<int>(leftTop.y),
		static_cast<int>(rightBottom.x), static_cast<int>(rightBottom.y),
		_color, FALSE
	);
}