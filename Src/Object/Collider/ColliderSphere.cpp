#define NOMINMAX
#include "ColliderSphere.h"
#include "../Common/Transform.h"

ColliderSphere::ColliderSphere(TAG _collisionTag, const Transform* _followTarget, const VECTOR& _localPosition, float _radius)
    : ColliderBase(SHAPE::SPHERE, _collisionTag, _followTarget)
    , localPosition_(_localPosition)
    , radius_(_radius)
{
}

void ColliderSphere::SetLocalPosition(const VECTOR& _position)
{
    localPosition_ = _position;
}

void ColliderSphere::SetRadius(float _radius)
{
    // 半径が負にならないように制限
    radius_ = std::max(_radius, 0.0f);
}

const VECTOR& ColliderSphere::GetLocalPosition(void) const
{
    return localPosition_;
}

VECTOR ColliderSphere::GetWorldPosition(void) const
{
    // ローカル座標をワールド座標へ変換
    return TransformLocalToWorld(localPosition_);
}

float ColliderSphere::GetRadius(void) const
{
    return radius_;
}

void ColliderSphere::DrawDebug(int _debugColor) const
{
    const VECTOR worldPosition = GetWorldPosition();

    // デバッグ用に球体を描画
    DrawSphere3D(worldPosition, radius_, DEBUG_SEGMENT_COUNT, _debugColor, _debugColor, false);
}