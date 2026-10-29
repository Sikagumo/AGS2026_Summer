#include "Collider2DBase.h"
#include "../Common/Transform.h"

Collider2DBase::Collider2DBase(SHAPE_2D _shapeType, TAG_2D _collisionTag)
	: shapeType_(_shapeType)
	, collisionTag_(_collisionTag)
{
}