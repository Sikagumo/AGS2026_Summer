#include "ColliderBase.h"

ColliderBase::ColliderBase(SHAPE shapeType, TAG collisionTag, const Transform* followTarget)
	: shapeType_(shapeType)
	, collisionTag_(collisionTag)
	, followTarget_(followTarget)
	, isActive_(true)
{
}

void ColliderBase::Draw(void)
{
	int color = DEBUG_COLOR_INACTIVE;

	// 有効なコライダなら色を変更
	if (isActive_)
	{
		color = DEBUG_COLOR_ACTIVE;
	}

	DrawDebug(color);
}

VECTOR ColliderBase::TransformLocalToWorld(const VECTOR& localPos) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = followTarget_->quaRot.PosAxis(localPos);

	// 位置を加算して最終的なワールド座標にする
	return VAdd(followTarget_->pos, localRotPos);
}

const Transform* ColliderBase::GetFollowTarget(void) const
{
	return followTarget_;
}

void ColliderBase::SetFollowTarget(Transform* followTarget)
{
	followTarget_ = followTarget;
}

ColliderBase::SHAPE ColliderBase::GetShapeType(void) const
{
	return shapeType_;
}

ColliderBase::TAG ColliderBase::GetCollisionTag(void) const
{
	return collisionTag_;
}

bool ColliderBase::IsActive(void) const
{
	return isActive_;
}

void ColliderBase::SetActive(bool isActive)
{
	isActive_ = isActive;
}