#include "ColliderCapsule.h"
#include "../Common/Transform.h"

ColliderCapsule::ColliderCapsule(TAG collisionTag, const Transform* followTarget, const VECTOR& localStartPos, const VECTOR& localEndPos, float radius)
    : ColliderBase(SHAPE::CAPSULE, collisionTag, followTarget)
    , localStartPos_(localStartPos)
    , radius_(radius)
{
}

void ColliderCapsule::SetLocalStartPos(const VECTOR& position)
{
    localStartPos_ = position;
}

void ColliderCapsule::SetLocalEndPos(const VECTOR& position)
{
    localEndPos_ = position;
}

void ColliderCapsule::SetRadius(float radius)
{
    radius_ = radius;
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

void ColliderCapsule::DrawDebug(int debugColor) const
{
    const VECTOR startPos = GetWorldStartPos();
    const VECTOR endPos = GetWorldEndPos();

    // ƒJƒvƒZƒ‹‚ð•`‰æ
    DrawCapsule3D(startPos, endPos, radius_, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}