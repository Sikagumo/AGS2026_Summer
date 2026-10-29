#include "ColliderCapsule.h"
#include "../Common/Transform.h"

ColliderCapsule::ColliderCapsule(TAG _collisionTag, const Transform* _followTarget, const VECTOR& _localStartPos, const VECTOR& _localEndPos, float _radius)
	: ColliderBase(SHAPE::CAPSULE, _collisionTag, _followTarget)
	, localStartPos_(_localStartPos)
	, localEndPos_(_localEndPos)
	, radius_(_radius)
{
}

VECTOR ColliderCapsule::GetWorldStartPos(void) const
{
	return TransformLocalToWorld(localStartPos_);
}

VECTOR ColliderCapsule::GetWorldEndPos(void) const
{
	return TransformLocalToWorld(localEndPos_);
}

void ColliderCapsule::DrawDebug(int _debugColor) const
{
	const VECTOR WORLD_START_POS = GetWorldStartPos();
	const VECTOR WORLD_END_POS = GetWorldEndPos();

	// ƒJƒvƒZƒ‹‚ð•`‰æ
	DrawCapsule3D(WORLD_START_POS, WORLD_END_POS, radius_, DEBUG_SEGMENT_COUNT, _debugColor, _debugColor, false);
}