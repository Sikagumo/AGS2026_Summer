#include "CollisionCapsule.h"

#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionSphere.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"

bool CollisionCapsule::CheckCapsuleVsCapsule(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	const auto* capsuleA =  static_cast<const ColliderCapsule*>(_colliderA);
	const auto* capsuleB =  static_cast<const ColliderCapsule*>(_colliderB);

	if (!capsuleA || !capsuleB) { return false; }

	// それぞれのカプセルのワールド座標（線分）を取得
	VECTOR startPositionA = capsuleA->GetWorldStartPos();
	VECTOR endPositionA = capsuleA->GetWorldEndPos();

	VECTOR startPositionB = capsuleB->GetWorldStartPos();
	VECTOR endPositionB = capsuleB->GetWorldEndPos();

	// カプセルAの中点を仮のターゲットにする
	VECTOR centerA = VScale(VAdd(startPositionA, endPositionA), 0.5f);

	VECTOR nearestPositionB = UtilityMath::GetNearestPointOnSegment(startPositionB, endPositionB, 
		centerA);
	VECTOR nearestPositionA = UtilityMath::GetNearestPointOnSegment(startPositionA, endPositionA,
		nearestPositionB);

	nearestPositionB = UtilityMath::GetNearestPointOnSegment(startPositionB, endPositionB, 
		nearestPositionA);

	// 割り出した2点間の距離の2乗を計算する
	VECTOR distanceVec = VSub(nearestPositionA, nearestPositionB);
	float distSquare = static_cast<float>(UtilityMath::SqrMagnitude(distanceVec));

	// お互いの半径の合計値と比較
	float radiusSum = capsuleA->GetRadius() + capsuleB->GetRadius();
	float radiusSumSq = radiusSum * radiusSum;

	// 衝突判定
	if (distSquare < radiusSumSq)
	{
		float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _colliderA;
		_outInfo.hitCollider = _colliderB;
		_outInfo.isActive = true;

		// 衝突位置は、お互いの最近接点の中間地点
		_outInfo.hitPosition = VAdd(nearestPositionA, VScale(VSub(nearestPositionB, 
			nearestPositionA), 0.5f));

		// 法線ベクトル（AからBへ向かう方向）の計算
		if (distance > 0.0f)
		{
			_outInfo.hitNormal = VScale(VSub(nearestPositionA, nearestPositionB), 1.0f / distance);
		}
		else
		{
			_outInfo.hitNormal = VGet(0.0f, 1.0f, 0.0f);
		}

		// めり込んでいる距離を算出
		_outInfo.penetration = radiusSum - distance;

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

	const auto* capsule =  static_cast<const ColliderCapsule*>(_capsuleCol);
	const auto* model =  static_cast<const ColliderModel*>(_modelCol);

	if (capsule == nullptr || model == nullptr)
	{
		return false;
	}

	int modelHandle = model->GetModelHandle();
	if (modelHandle == -1)
	{
		return false;
	}

	VECTOR startPos = capsule->GetWorldStartPos();
	VECTOR endPos = capsule->GetWorldEndPos();
	float radius = capsule->GetRadius();

	// カプセルとモデル全体の衝突判定
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(modelHandle, -1,
		startPos, endPos, radius);

	if (hitResult.HitNum > 0)
	{
		float maxPenetration = -1.0f;
		int bestIndex = -1;

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			if (model->IsExcludedFrame(hitResult.Dim[i].FrameIndex))
			{
				continue;
			}

			// 各ポリゴンに対する正しいめり込み量をループ内で仮計算する
			const auto& poly = hitResult.Dim[i];
			float polyPenetration = 0.0f;

			if (poly.Normal.y > 0.5f)
			{
				// 床の場合は、沈み込みを防ぐための高さを計算
				float capsuleBottomY = (startPos.y < endPos.y ? startPos.y : endPos.y) - radius;
				polyPenetration = poly.HitPosition.y - capsuleBottomY;
			}
			else
			{
				// 壁や急斜面の場合は、芯からの最短距離でめり込みを計算
				VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(startPos, endPos, poly.HitPosition);
				float distance = UtilityMath::MagnitudeF(VSub(poly.HitPosition, nearestPos));
				polyPenetration = radius - distance;
			}

			if (polyPenetration > maxPenetration)
			{
				maxPenetration = polyPenetration;
				bestIndex = i;
			}
		}

		if (bestIndex == -1)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		const auto& bestHit = hitResult.Dim[bestIndex];

		// 選択された最も適切な衝突情報を格納
		_outInfo.myCollider = _capsuleCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.hitPosition = bestHit.HitPosition;
		_outInfo.hitNormal = bestHit.Normal;
		_outInfo.isActive = true;

		// 決定されたポリゴンのタイプに応じて、正しいめり込み量を確定させる
		if (bestHit.Normal.y > 0.5f)
		{
			float capsuleBottomY = (startPos.y < endPos.y ? startPos.y : endPos.y) - radius;
			_outInfo.penetration = bestHit.HitPosition.y - capsuleBottomY;
		}
		else
		{
			VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(startPos, endPos, bestHit.HitPosition);
			float distance = UtilityMath::MagnitudeF(VSub(bestHit.HitPosition, nearestPos));
			_outInfo.penetration = radius - distance;
		}

		// めり込み量が極端にマイナスにならないように安全弁をかける
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