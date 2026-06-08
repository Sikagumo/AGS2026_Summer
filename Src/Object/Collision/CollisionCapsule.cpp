#include "CollisionCapsule.h"

#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionSphere.h"
#include "../Manager/CollisionController.h"
#include "../../Utility/UtilityMath.h"

bool CollisionCapsule::CheckCapsuleVsCapsule(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	const auto* capsuleA = dynamic_cast<const ColliderCapsule*>(_colliderA);
	const auto* capsuleB = dynamic_cast<const ColliderCapsule*>(_colliderB);

	if (!capsuleA || !capsuleB) { return false; }

	// それぞれのカプセルのワールド座標（線分）を取得
	VECTOR startPositionA = capsuleA->GetWorldStartPos();
	VECTOR endPositionA = capsuleA->GetWorldEndPos();

	VECTOR startPositionB = capsuleB->GetWorldStartPos();
	VECTOR endPositionB = capsuleB->GetWorldEndPos();

	// カプセルAの中点を仮のターゲットにする
	VECTOR centerA = VScale(VAdd(startPositionA, endPositionA), 0.5f);

	VECTOR nearestPositionB = UtilityMath::GetNearestPointOnSegment(startPositionB, endPositionB, centerA);
	VECTOR nearestPositionA = UtilityMath::GetNearestPointOnSegment(startPositionA, endPositionA,
		nearestPositionB);
	nearestPositionB = UtilityMath::GetNearestPointOnSegment(startPositionB, endPositionB, nearestPositionA);

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
		_outInfo.hitPosition = VAdd(nearestPositionA, VScale(VSub(nearestPositionB, nearestPositionA), 0.5f));

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

bool CollisionCapsule::CheckCapsuleVsSphere(const ColliderBase* _capsuleCol, const ColliderBase* _sphereCol, CollisionInfo& _outInfo)
{
	return CollisionSphere::CheckSphereVsCapsule(_sphereCol, _capsuleCol, _outInfo);
}

bool CollisionCapsule::CheckCapsuleVsModel(const ColliderBase* _capsuleCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_capsuleCol || !_modelCol) { return false; }

	const auto* capsule = dynamic_cast<const ColliderCapsule*>(_capsuleCol);
	const auto* model = dynamic_cast<const ColliderModel*>(_modelCol);

	if (capsule == nullptr || model == nullptr)
	{
		return false;
	}

	// モデルハンドル取得
	int modelHandle = model->GetModelHandle();
	if (modelHandle == -1) { return false; }

	// 判定用パラメータ取得
	VECTOR startPos = capsule->GetWorldStartPos();
	VECTOR endPos = capsule->GetWorldEndPos();
	float radius = capsule->GetRadius();

	// カプセルとモデル全体の衝突判定（触れている全ポリゴンが格納される）
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(modelHandle, -1,
		startPos, endPos, radius);

	// 衝突結果の解析
	if (hitResult.HitNum > 0)
	{
		float maxUpward = -2.0f;
		int bestIndex = -1;

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			// 除外対象のフレームチェック
			if (model->IsExcludedFrame(hitResult.Dim[i].FrameIndex)) { continue; }

			// 法線の Y 成分（どれだけ真上を向いているか）を比較
			// 坂道や壁（Yが0に近い）よりも、平らな床（Yが1に近い）を最優先する
			if (hitResult.Dim[i].Normal.y > maxUpward)
			{
				maxUpward = hitResult.Dim[i].Normal.y;
				bestIndex = i;
			}
		}

		// 有効なポリゴンが1つも見つからなかった場合
		if (bestIndex == -1)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		// 最も「床」として適切なポリゴン情報を抽出
		const auto& bestHit = hitResult.Dim[bestIndex];

		// 衝突情報の設定
		_outInfo.myCollider = _capsuleCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.hitPosition = bestHit.HitPosition;
		_outInfo.hitNormal = bestHit.Normal;
		_outInfo.isActive = true;

		if (bestHit.Normal.y > 0.5f)
		{
			float capsuleBottomY = (startPos.y < endPos.y ? startPos.y : endPos.y) - radius;

			// 床の高さ（HitPosition.y）よりも、カプセルの底がどれだけ下にあるか
			_outInfo.penetration = bestHit.HitPosition.y - capsuleBottomY;
		}
		else
		{
			// 壁や急斜面の場合は、元の線分最短距離を使う
			VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(startPos, endPos, bestHit.HitPosition);
			float distance = UtilityMath::MagnitudeF(VSub(bestHit.HitPosition, nearestPos));
			_outInfo.penetration = radius - distance;
		}

		// めり込み量が極端にマイナスにならないように安全弁をかける
		if (_outInfo.penetration < 0.0f)
		{
			_outInfo.penetration = 0.0f;
		}

		// メモリ解放
		MV1CollResultPolyDimTerminate(hitResult);
		return true;

		return true;
	}

	// 衝突しなかった場合のメモリ解放
	MV1CollResultPolyDimTerminate(hitResult);

	return false;
}