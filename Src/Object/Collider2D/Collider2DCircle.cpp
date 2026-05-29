#include "Collider2DCircle.h"

Collider2DCircle::Collider2DCircle(const Vector2F& _centerPos, float _radius, Collider2DBase::TAG_2D _collisionTag)
	: Collider2DBase(Collider2DBase::SHAPE_2D::CIRCLE, _collisionTag)
	, radius_(_radius)
	, centerPos_(_centerPos)
{
}

void Collider2DCircle::SetCenterPos(const Vector2F& _centerPos)
{
	centerPos_ = _centerPos;
}

void Collider2DCircle::SetRadius(float _radius)
{
	radius_ = _radius;
}

Vector2F Collider2DCircle::GetWorldCenterPos(void) const
{
	return centerPos_;
}

float Collider2DCircle::GetRadius(void) const
{
	return radius_;
}

void Collider2DCircle::DrawDebug(int _color) const
{
	Vector2F currentPos = GetWorldCenterPos();
	DrawCircle(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(radius_), _color, FALSE);
}