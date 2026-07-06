#include "ColliderCapsule.h"
#include "../Common/Transform.h"

ColliderCapsule::ColliderCapsule(TAG _collisionTag, const Transform* _followTarget, const VECTOR& _localStartPos, const VECTOR& _localEndPos, float _radius)
    : ColliderBase(SHAPE::CAPSULE, _collisionTag, _followTarget)
    , localStartPos_(_localStartPos)
    , localEndPos_(_localEndPos)
    , radius_(_radius)
{
}

void ColliderCapsule::SetLocalStartPos(const VECTOR& _position)
{
    localStartPos_ = _position;
}

void ColliderCapsule::SetLocalEndPos(const VECTOR& _position)
{
    localEndPos_ = _position;
}

void ColliderCapsule::SetRadius(float _radius)
{
    radius_ = _radius;
}

const VECTOR& ColliderCapsule::GetLocalStartPos(void) const
{
    return localStartPos_;
}

const VECTOR& ColliderCapsule::GetLocalEndPos(void) const
{
    return localEndPos_;
}

VECTOR ColliderCapsule::GetWorldStartPos(void) const
{
    return TransformLocalToWorld(localStartPos_);
}

VECTOR ColliderCapsule::GetWorldEndPos(void) const
{
    return TransformLocalToWorld(localEndPos_);
}

float ColliderCapsule::GetRadius(void) const
{
    return radius_;
}

void ColliderCapsule::SetShape(const VECTOR& _localStartPos, const VECTOR& _localEndPos, 
    float _radius)
{
    localStartPos_ = _localStartPos;
    localEndPos_ = _localEndPos;
    radius_ = _radius;
}

void ColliderCapsule::DrawDebug(int debugColor) const
{
    const VECTOR startPos = GetWorldStartPos();
    const VECTOR endPos = GetWorldEndPos();

    // ƒJƒvƒZƒ‹‚ð•`‰æ
    DrawCapsule3D(startPos, endPos, radius_, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}