#include "CollisionCapsule.h"
#include <cmath>
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionSphere.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr float VALUE_ZERO_F = 0.0f;       // 浮動小数点数のゼロ（初期化や基準値用）
	constexpr float VALUE_HALF_F = 0.5f;       // 半分の値を計算するための係数
	constexpr float VALUE_ONE_F = 1.0f;        // 浮動小数点数の1.0（乗算や割合の基準用）
	constexpr float VALUE_THREE_F = 3.0f;      // 浮動小数点数の3.0
	constexpr float NORMAL_Y_THRESHOLD = 0.5f; // 法線のY軸方向の判定に用いる閾値
	constexpr int INVALID_INDEX = -1;          // 配列等の無効なインデックスを示す値
}

bool CollisionCapsule::CheckCapsuleVsCapsule(const ColliderBase* _colliderA, const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	const auto* capsuleA = static_cast<const ColliderCapsule*>(_colliderA);
	const auto* capsuleB = static_cast<const ColliderCapsule*>(_colliderB);

	if (!capsuleA || !capsuleB)
	{
		return false;
	}

	// それぞれのカプセルのワールド座標を取得
	const VECTOR startPosA = capsuleA->GetWorldStartPos();
	const VECTOR endPosA = capsuleA->GetWorldEndPos();

	const VECTOR startPosB = capsuleB->GetWorldStartPos();
	const VECTOR endPosB = capsuleB->GetWorldEndPos();

	// カプセルAの中点を仮のターゲットにする
	const VECTOR centerA = VScale(VAdd(startPosA, endPosA), VALUE_HALF_F);

	VECTOR nearestPosB = UtilityMath::GetNearestPointOnSegment(startPosB, endPosB, centerA);
	VECTOR nearestPosA = UtilityMath::GetNearestPointOnSegment(startPosA, endPosA, nearestPosB);

	nearestPosB = UtilityMath::GetNearestPointOnSegment(startPosB, endPosB, nearestPosA);

	// 割り出した2点間の距離の2乗を計算する
	const VECTOR distanceVec = VSub(nearestPosA, nearestPosB);
	const float distSquare = static_cast<float>(UtilityMath::SqrMagnitude(distanceVec));

	// お互いの半径の合計値
	const float radiusSum = capsuleA->GetRadius() + capsuleB->GetRadius();

	// 衝突判定
	if (distSquare < (radiusSum * radiusSum))
	{
		const float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _colliderA;
		_outInfo.hitCollider = _colliderB;
		_outInfo.isActive = true;

		// 衝突位置は、お互いの最近接点の中間地点
		_outInfo.hitPosition = VAdd(nearestPosA, VScale(VSub(nearestPosB, nearestPosA), VALUE_HALF_F));

		// 法線ベクトルの計算
		if (distance > VALUE_ZERO_F)
		{
			_outInfo.hitNormal = VScale(VSub(nearestPosA, nearestPosB), VALUE_ONE_F / distance);
		}
		else
		{
			_outInfo.hitNormal = VGet(VALUE_ZERO_F, VALUE_ONE_F, VALUE_ZERO_F);
		}

		// めり込んでいる距離を算出
		_outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionCapsule::CheckCapsuleVsSphere(const ColliderBase* _capsuleCol, const ColliderBase* _sphereCol, CollisionInfo& _outInfo)
{
	return CollisionSphere::CheckSphereVsCapsule(_sphereCol, _capsuleCol, _outInfo);
}

bool CollisionCapsule::CheckCapsuleVsModel(const ColliderBase* _capsuleCol, const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_capsuleCol || !_modelCol)
	{
		return false;
	}

	const auto* capsule = static_cast<const ColliderCapsule*>(_capsuleCol);
	const auto* model = static_cast<const ColliderModel*>(_modelCol);

	if (capsule == nullptr || model == nullptr)
	{
		return false;
	}

	const int modelHandle = model->GetModelHandle();

	if (modelHandle == INVALID_INDEX)
	{
		return false;
	}

	const VECTOR startPos = capsule->GetWorldStartPos();
	const VECTOR endPos = capsule->GetWorldEndPos();
	const float radius = capsule->GetRadius();

	// カプセルとモデル全体の衝突判定
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(modelHandle, INVALID_INDEX, startPos, endPos, radius);

	if (hitResult.HitNum > 0)
	{
		float maxPenetration = -1.0f;
		int bestIndex = INVALID_INDEX;

		const ColliderBase::TAG modelTag = _modelCol->GetCollisionTag(); 

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			if (model->IsExcludedFrame(hitResult.Dim[i].FrameIndex))
			{
				continue;
			}

			const auto& poly = hitResult.Dim[i];

			if (modelTag == ColliderBase::TAG::STAGE && poly.Normal.y <= NORMAL_Y_THRESHOLD)
			{
				continue;
			}

			if (modelTag == ColliderBase::TAG::WALL && poly.Normal.y > NORMAL_Y_THRESHOLD)
			{
				continue;
			}

			const VECTOR polyPoint = poly.Position[0];

			const VECTOR nearestOnAxis = UtilityMath::GetNearestPointOnSegment(startPos, endPos, polyPoint);
			const VECTOR toHit = VSub(polyPoint, nearestOnAxis);
			const float distAlongNormal = VDot(toHit, poly.Normal);
			const float polyPenetration = radius - distAlongNormal;

			if (polyPenetration > maxPenetration)
			{
				maxPenetration = polyPenetration;
				bestIndex = i;
			}
		}

		if (bestIndex == INVALID_INDEX)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		const auto& bestHit = hitResult.Dim[bestIndex];

		_outInfo.myCollider = _capsuleCol;
		_outInfo.hitCollider = _modelCol;

		const VECTOR bestPolyPoint = VScale(
			VAdd(VAdd(bestHit.Position[0], bestHit.Position[1]), bestHit.Position[2]),
			VALUE_ONE_F / VALUE_THREE_F);

		_outInfo.hitPosition = bestPolyPoint;
		_outInfo.hitNormal = bestHit.Normal;
		_outInfo.isActive = true;

		const VECTOR nearestOnAxis = UtilityMath::GetNearestPointOnSegment(startPos, endPos, bestPolyPoint);
		const VECTOR toHit = VSub(bestPolyPoint, nearestOnAxis);
		const float distAlongNormal = VDot(toHit, bestHit.Normal);

		_outInfo.penetration = radius - distAlongNormal;

		if (_outInfo.penetration < VALUE_ZERO_F)
		{
			_outInfo.penetration = VALUE_ZERO_F;
		}

		MV1CollResultPolyDimTerminate(hitResult);
		return true;
	}

	MV1CollResultPolyDimTerminate(hitResult);
	return false;
}