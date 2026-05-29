#include "Collider2DBase.h"
#include "../Common/Transform.h"

Collider2DBase::Collider2DBase(SHAPE_2D _shapeType, TAG_2D _collisionTag)
	: shapeType_(_shapeType)
	, collisionTag_(_collisionTag)
{
}

Collider2DBase::SHAPE_2D Collider2DBase::GetShapeType(void) const
{
	return shapeType_;
}

Collider2DBase::TAG_2D Collider2DBase::GetCollisionTag(void) const
{
	return collisionTag_;
}