#include "ColliderCapsule.h"
#include <DxLib.h>
#include "../Common/Transform.h"
#include "./ColliderModel.h"


ColliderCapsule::ColliderCapsule(TAG tag, const Transform* follow, const VECTOR& localPosTop, const VECTOR& localPosDown, float radius):
	ColliderBase(SHAPE::CAPSULE, tag, follow),
	localPosTop_(localPosTop),
	localPosDown_(localPosDown),
	radius_(radius)
{
}

VECTOR ColliderCapsule::GetPosTop(void) const
{
	return GetRotPos(localPosTop_);
}
VECTOR ColliderCapsule::GetPosDown(void) const
{
	return GetRotPos(localPosDown_);
}

VECTOR ColliderCapsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();
	VECTOR diff = VSub(top, down);

	return VAdd(down, VScale(diff, 0.5f));
}

void ColliderCapsule::DrawDebug(int color)
{
	// 上の球体
	VECTOR pos1 = GetPosTop();
	DrawSphere3D(pos1, radius_, 5, color, color, false);

	// 下の球体
	VECTOR pos2 = GetPosDown();
	DrawSphere3D(pos2, radius_, 5, color, color, false);
	
	VECTOR dir;
	VECTOR s, e;

	// 球体を繋ぐ線(X+)
	dir = follow_->GetRight();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// 球体を繋ぐ線(X-)
	dir = follow_->GetLeft();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// 球体を繋ぐ線(Z+)
	dir = follow_->GetForward();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// 球体を繋ぐ線(Z-)
	dir = follow_->GetBack();
	s = VAdd(pos1, VScale(dir, radius_));
	e = VAdd(pos2, VScale(dir, radius_));
	DrawLine3D(s, e, color);

	// カプセルの中心
	DrawSphere3D(GetCenter(), radius_, 10, color, color, true);
}

VECTOR ColliderCapsule::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly,
												  int maxTryCnt, float pushDistance) const
{
	// コピー生成
	Transform tempTransform = *follow_;
	ColliderCapsule tmpCapsule = *this;
	tmpCapsule.SetFollow(&tempTransform);

	// 衝突補正処理
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// カプセルと三角形の当たり判定
		if (!HitCheck_Capsule_Triangle(
			tmpCapsule.GetPosTop(), tmpCapsule.GetPosDown(),
			tmpCapsule.GetRadius(),
			hitColPoly.Position[0], hitColPoly.Position[1],
			hitColPoly.Position[2]))
		{
			break;
		}
		// 衝突していたら法線方向に押し戻し
		tempTransform.pos = VAdd(tempTransform.pos, VScale(hitColPoly.Normal, pushDistance));
		tryCnt++;
	}
	return tempTransform.pos;
}

void ColliderCapsule::PushBackAlongNormal(const ColliderModel* _colliderModel, Transform* _transform, int _maxTryCnt, float _pushDistance, bool isExclude, bool isTarget) const
{
	auto hits = MV1CollCheck_Capsule(_colliderModel->GetFollow()->modelId, -1,
									 GetPosTop(), GetPosDown(), GetRadius());

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hitPoly = hits.Dim[i];

		// 除外フレームは無視
		/*if (isExclude && colliderModel->IsExcludeFrame(hitPoly.FrameIndex))
		{
			continue;
		}*/
		// 対象フレーム以外は無視
		if (isTarget && !_colliderModel->IsTargetFrame(hitPoly.FrameIndex))
		{
			continue;
		}

		// 指定回数と距離で三角形の法線方向に押し戻す
		_transform->pos = GetPosPushBackAlongNormal(hitPoly, _maxTryCnt, _pushDistance);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

bool ColliderCapsule::IsHit(const ColliderModel* colliderModel, bool isExcude, bool isTarget) const
{
	bool ret = false;

	// ステージモデル(地面)との衝突
	auto hits = MV1CollCheck_LineDim(
		colliderModel->GetFollow()->modelId, -1,
		GetPosTop(), GetPosDown());

	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];

		// 除外フレームは無視
		/*if (isExclude && colliderModel->IsExcludeFrame(hit.FrameIndex))
		{
			continue;
		}*/
		// 対象フレーム以外は無視
		if (isTarget && !colliderModel->IsTargetFrame(hit.FrameIndex))
		{
			continue;
		}

		// 衝突！
		ret = true;
		break;
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	return ret;
}
