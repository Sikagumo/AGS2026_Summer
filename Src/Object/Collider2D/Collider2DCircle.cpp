#include "Collider2DCircle.h"

Collider2DCircle::Collider2DCircle(const Vector2F& _centerPos, float _radius, TAG_2D _collisionTag)
	: Collider2DBase(SHAPE_2D::CIRCLE, _collisionTag)
	, radius_(_radius)
	, centerPos_(_centerPos)
{
}

void Collider2DCircle::DrawDebug(int _color) const
{
	DrawCircle(
		static_cast<int>(centerPos_.x),
		static_cast<int>(centerPos_.y),
		static_cast<int>(radius_),
		_color, FALSE
	);
}