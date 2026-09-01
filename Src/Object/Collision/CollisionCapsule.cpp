#include "CollisionCapsule.h"

#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionSphere.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

bool CollisionCapsule::CheckCapsuleVsCapsule(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	const auto* CAPSULE_A = static_cast<const ColliderCapsule*>(_colliderA);
	const auto* CAPSULE_B = static_cast<const ColliderCapsule*>(_colliderB);

	if (!CAPSULE_A || !CAPSULE_B)
	{
		return false;
	}

	// それぞれのカプセルのワールド座標を取得
	const VECTOR START_POS_A = CAPSULE_A->GetWorldStartPos();
	const VECTOR END_POS_A = CAPSULE_A->GetWorldEndPos();

	const VECTOR START_POS_B = CAPSULE_B->GetWorldStartPos();
	const VECTOR END_POS_B = CAPSULE_B->GetWorldEndPos();

	// カプセルAの中点を仮のターゲットにする
	const VECTOR CENTER_A = VScale(VAdd(START_POS_A, END_POS_A), 0.5f);

	VECTOR nearestPosB = UtilityMath::GetNearestPointOnSegment(START_POS_B, END_POS_B,
		CENTER_A);
	VECTOR nearestPosA = UtilityMath::GetNearestPointOnSegment(START_POS_A, END_POS_A,
		nearestPosB);

	nearestPosB = UtilityMath::GetNearestPointOnSegment(START_POS_B, END_POS_B,
		nearestPosA);

	// 割り出した2点間の距離の2乗を計算する
	const VECTOR DISTANCE_VEC = VSub(nearestPosA, nearestPosB);
	const float DIST_SQUARE = static_cast<float>(UtilityMath::SqrMagnitude(DISTANCE_VEC));

	// お互いの半径の合計値
	const float RADIUS_SUM = CAPSULE_A->GetRadius() + CAPSULE_B->GetRadius();

	// 衝突判定
	if (DIST_SQUARE < (RADIUS_SUM * RADIUS_SUM))
	{
		const float DISTANCE = sqrtf(DIST_SQUARE);

		// 衝突情報の設定
		_outInfo.myCollider = _colliderA;
		_outInfo.hitCollider = _colliderB;
		_outInfo.isActive = true;

		// 衝突位置は、お互いの最近接点の中間地点
		_outInfo.hitPosition = VAdd(nearestPosA, VScale(VSub(nearestPosB,
			nearestPosA), 0.5f));

		// 法線ベクトルの計算
		if (DISTANCE > 0.0f)
		{
			_outInfo.hitNormal = VScale(VSub(nearestPosA, nearestPosB), 1.0f / DISTANCE);
		}
		else
		{
			_outInfo.hitNormal = VGet(0.0f, 1.0f, 0.0f);
		}

		// めり込んでいる距離を算出
		_outInfo.penetration = RADIUS_SUM - DISTANCE;

		return true;
	}

	return false;
}

bool CollisionCapsule::CheckCapsuleVsSphere(const ColliderBase* _capsuleCol,
	const ColliderBase* _sphereCol, CollisionInfo& _outInfo)
{
	return CollisionSphere::CheckSphereVsCapsule(_sphereCol, _capsuleCol, _outInfo);
}

bool CollisionCapsule::CheckCapsuleVsModel(const ColliderBase* _capsuleCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_capsuleCol || !_modelCol)
	{
		return false;
	}

	const auto* CAPSULE = static_cast<const ColliderCapsule*>(_capsuleCol);
	const auto* MODEL = static_cast<const ColliderModel*>(_modelCol);

	if (CAPSULE == nullptr || MODEL == nullptr)
	{
		return false;
	}

	const int MODEL_HANDLE = MODEL->GetModelHandle(); 

	if (MODEL_HANDLE == -1)
	{
		return false;
	}

	const VECTOR START_POS = CAPSULE->GetWorldStartPos();
	const VECTOR END_POS = CAPSULE->GetWorldEndPos();
	const float RADIUS = CAPSULE->GetRadius();

	// カプセルとモデル全体の衝突判定
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(MODEL_HANDLE, -1,
		START_POS, END_POS, RADIUS);

	if (hitResult.HitNum > 0)
	{
		float maxPenetration = -1.0f;
		int bestIndex = -1;

		const ColliderBase::TAG MODEL_TAG = _modelCol->GetCollisionTag(); // モデルのタグ

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			if (MODEL->IsExcludedFrame(hitResult.Dim[i].FrameIndex))
			{
				continue;
			}

			const auto& POLY = hitResult.Dim[i]; // ポリゴン情報

			if (MODEL_TAG == ColliderBase::TAG::STAGE && POLY.Normal.y <= 0.5f)
			{
				continue;
			}

			if (MODEL_TAG == ColliderBase::TAG::WALL && POLY.Normal.y > 0.5f)
			{
				continue;
			}

			const VECTOR POLY_POINT = POLY.Position[0];

			const VECTOR NEAREST_ON_AXIS = UtilityMath::GetNearestPointOnSegment(START_POS, END_POS, POLY_POINT);
			const VECTOR TO_HIT = VSub(POLY_POINT, NEAREST_ON_AXIS);
			const float DIST_ALONG_NORMAL = VDot(TO_HIT, POLY.Normal);
			const float POLY_PENETRATION = RADIUS - DIST_ALONG_NORMAL;

			if (POLY_PENETRATION > maxPenetration)
			{
				maxPenetration = POLY_PENETRATION;
				bestIndex = i;
			}
		}

		if (bestIndex == -1)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		const auto& BEST_HIT = hitResult.Dim[bestIndex];

		_outInfo.myCollider = _capsuleCol;
		_outInfo.hitCollider = _modelCol;

		const VECTOR BEST_POLY_POINT = VScale(
			VAdd(VAdd(BEST_HIT.Position[0], BEST_HIT.Position[1]), BEST_HIT.Position[2]),
			1.0f / 3.0f);

		_outInfo.hitPosition = BEST_POLY_POINT;
		_outInfo.hitNormal = BEST_HIT.Normal;
		_outInfo.isActive = true;

		const VECTOR NEAREST_ON_AXIS = UtilityMath::GetNearestPointOnSegment(START_POS, END_POS, BEST_POLY_POINT);
		const VECTOR TO_HIT = VSub(BEST_POLY_POINT, NEAREST_ON_AXIS);
		const float DIST_ALONG_NORMAL = VDot(TO_HIT, BEST_HIT.Normal);

		_outInfo.penetration = RADIUS - DIST_ALONG_NORMAL;

		if (_outInfo.penetration < 0.0f)
		{
			_outInfo.penetration = 0.0f;
		}

		MV1CollResultPolyDimTerminate(hitResult);
		return true;
	}

	MV1CollResultPolyDimTerminate(hitResult);
	return false;
}