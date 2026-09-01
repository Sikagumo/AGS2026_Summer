#include "ColliderBase.h"

ColliderBase::ColliderBase(SHAPE _shapeType, TAG _collisionTag, const Transform* _followTarget)
	: shapeType_(_shapeType)
	, collisionTag_(_collisionTag)
	, followTarget_(_followTarget)
	, isActive_(true)
	, isTrigger_(true)
{
}

void ColliderBase::Draw(void) const
{
	int color = DEBUG_COLOR_INACTIVE;

	// 有効なコライダなら色を変更
	if (isActive_)
	{
		color = DEBUG_COLOR_ACTIVE;
	}

	DrawDebug(color);
}

VECTOR ColliderBase::TransformLocalToWorld(const VECTOR& _localPos) const
{
	// ローカル座標を回転させてワールド座標へ変換
	VECTOR localRotPos = followTarget_->quaRot.PosAxis(_localPos);

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

void ColliderBase::SetActive(bool _isActive)
{
	isActive_ = _isActive;
}

void ColliderBase::SetTriger(bool _isTrigger)
{
	isTrigger_ = _isTrigger;
}