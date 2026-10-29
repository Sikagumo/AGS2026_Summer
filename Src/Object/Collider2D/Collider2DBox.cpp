#include "Collider2DBox.h"

namespace
{
	// îºï™ÇÃílÇåvéZÇ∑ÇÈÇΩÇﬂÇÃåWêîíËêî
	constexpr float VALUE_HALF_F = 0.5f;
}

Collider2DBox::Collider2DBox(const Vector2F& _centerPos, float _width, float _height, TAG_2D _collisionTag)
	: Collider2DBase(SHAPE_2D::BOX, _collisionTag)
	, width_(_width)
	, height_(_height)
	, centerPos_(_centerPos)
{
}

Vector2F Collider2DBox::GetLeftTop(void) const
{
	float halfWidth = width_ * VALUE_HALF_F;
	float halfHeight = height_ * VALUE_HALF_F;

	return Vector2F(centerPos_.x - halfWidth, centerPos_.y - halfHeight);
}

Vector2F Collider2DBox::GetRightBottom(void) const
{
	float halfWidth = width_ * VALUE_HALF_F;
	float halfHeight = height_ * VALUE_HALF_F;

	return Vector2F(centerPos_.x + halfWidth, centerPos_.y + halfHeight);
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