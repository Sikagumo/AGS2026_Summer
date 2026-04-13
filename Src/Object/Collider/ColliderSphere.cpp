#include "ColliderSphere.h"
#include "./ColliderBase.h"
#include "../Common/Transform.h"

ColliderSphere::ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius):
	ColliderBase::ColliderBase(SHAPE::SPHERE, tag, follow),
	localPos_(localPos),radius_(radius)
{

}

const VECTOR& ColliderSphere::GetLocalPos(void) const
{
	return localPos_;
}

VECTOR ColliderSphere::GetPos(void) const
{
	return GetRotPos(localPos_);
}

void ColliderSphere::DrawDebug(int color)
{
	DrawSphere3D(GetPos(), radius_, 10, color, color, true);
}

VECTOR ColliderSphere::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly,
	int maxTryCnt, float pushDistance) const
{
	// コピー生成
	Transform tempTransform = *follow_;
	ColliderSphere tmpSphere = *this;
	tmpSphere.SetFollow(&tempTransform);

	// 衝突補正処理
	int tryCnt = 0;
	while (tryCnt < maxTryCnt)
	{
		// カプセルと三角形の当たり判定
		if (!HitCheck_Sphere_Triangle(
			tmpSphere.GetPos(),
			tmpSphere.GetRadius(),
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