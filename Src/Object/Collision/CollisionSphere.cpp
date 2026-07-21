#include "CollisionSphere.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "../Collision/CollisionController.h"
#include "../../Utility/UtilityMath.h"


bool CollisionSphere::CheckSphereVsSphere(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	if (!_colliderA || !_colliderB) { return false; }

	const auto* sphereA =  static_cast<const ColliderSphere*>(_colliderA);
	const auto* sphereB =  static_cast<const ColliderSphere*>(_colliderB);

	if (sphereA == nullptr || sphereB == nullptr)
	{
		return false;
	}

	// 距離計算
	VECTOR positionA = sphereA->GetWorldPosition();
	VECTOR positionB = sphereB->GetWorldPosition();

	float distanceX = positionA.x - positionB.x;
	float distanceY = positionA.y - positionB.y;
	float distanceZ = positionA.z - positionB.z;

	// 距離の二乗を計算
	float distSquare = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);

	// 半径の合計値と比較
	float radiusSum = sphereA->GetRadius() + sphereB->GetRadius();
	float radiusSumSq = radiusSum * radiusSum;

	// 衝突判定
	if (distSquare < radiusSumSq)
	{
		float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _colliderA;
		_outInfo.hitCollider = _colliderB;
		_outInfo.isActive = true;
		// 衝突位置の計算
		_outInfo.hitPosition = VAdd(positionB, VScale(VSub(positionA, positionB), 0.5f));

		// 法線ベクトルと押し出し量の計算
		if (distance > 0.0f)
		{
			_outInfo.hitNormal = VScale(VSub(positionA, positionB), 1.0f / distance);
		}

		// めり込んでいる距離を算出
		_outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsCapsule(const ColliderBase* _sphereCol,
	const ColliderBase* _capsuleCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_capsuleCol) { return false; }

	const auto* sphereHit =  static_cast<const ColliderSphere*>(_sphereCol);
	const auto* capsuleHit =  static_cast<const ColliderCapsule*>(_capsuleCol);

	if (sphereHit == nullptr || capsuleHit == nullptr)
	{
		return false;
	}

	// 各形状のパラメータ取得
	VECTOR spherePos = sphereHit->GetWorldPosition();
	VECTOR capStartPos = capsuleHit->GetWorldStartPos();
	VECTOR capEndPos = capsuleHit->GetWorldEndPos();
	float sphereRadius = sphereHit->GetRadius();
	float capsuleRadius = capsuleHit->GetRadius();

	// カプセルの線分上で、球体に最も近い点を算出
	VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(capStartPos, capEndPos, spherePos);

	// 最近接点と球体の中心距離による判定
	float distSquare = static_cast<float>(UtilityMath::SqrMagnitude(spherePos, nearestPos));
	float radiusSum = sphereRadius + capsuleRadius;
	float radiusSumSq = radiusSum * radiusSum;

	// 衝突判定
	if (distSquare < radiusSumSq)
	{
		float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _sphereCol;
		_outInfo.hitCollider = _capsuleCol;
		_outInfo.isActive = true;

		// 衝突位置の計算
		_outInfo.hitPosition = UtilityMath::Lerp(nearestPos, spherePos, UtilityMath::HALF_NUM);

		// 法線ベクトルと押し出し量の計算
		if (distance > 0.0f)
		{
			_outInfo.hitNormal = VScale(VSub(spherePos, nearestPos), 1.0f / distance);
		}
		_outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsModel(const ColliderBase* _sphereCol, 
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_modelCol) { return false; }

	const auto* sphere = static_cast<const ColliderSphere*>(_sphereCol);
	if (sphere == nullptr) { return false; }

	bool isHit = CheckSphereVsModelCore(sphere->GetWorldPosition(), sphere->GetRadius(),
		_modelCol, &_outInfo);

	if (isHit)
	{
		_outInfo.myCollider = _sphereCol;
	}

	return isHit;
}

bool CollisionSphere::CheckHitWave(const ColliderBase* _hitCapsuleCol, ColliderBase* _waveCol)
{
	if (!_hitCapsuleCol || !_waveCol)
	{
		return false;
	}

	const auto* capsule =  static_cast <const ColliderCapsule*>(_hitCapsuleCol);
	const auto* wave =  static_cast<const ColliderSphere*>(_waveCol);

	if (capsule == nullptr || wave == nullptr)
	{
		return false;
	}

	VECTOR wavePos = wave->GetWorldPosition();

	VECTOR capStartPos = capsule->GetWorldStartPos();
	VECTOR capEndPos = capsule->GetWorldEndPos();

	VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(capStartPos, capEndPos, wavePos);

	VECTOR wavePosXZ = wavePos;
	VECTOR nearestPosXZ = nearestPos;

	wavePosXZ.y = 0.0f;
	nearestPosXZ.y = 0.0f;

	float distance = static_cast<float>(UtilityMath::Distance(nearestPosXZ, wavePosXZ));

	float waveRadius = wave->GetRadius();

	float totalThickness = HIT_WAVE_THICKNESS + capsule->GetRadius();

	bool isHit = abs(distance - waveRadius) < totalThickness;

	if (!isHit)
	{
		return false;
	}

	float capsuleFootY = (capStartPos.y < capEndPos.y ? capStartPos.y : capEndPos.y) - capsule->GetRadius();

	float differenceY = nearestPos.y - wavePos.y;

	if (capsuleFootY > (wavePos.y + HIT_WAVE_HEIGHT))
	{
		return false;
	}

	return true;
}

bool CollisionSphere::CheckSphereVsModelCore(const VECTOR& _centerPos, float _radius, 
	const ColliderBase* _modelCol, CollisionInfo* _outInfo)
{
	if (!_modelCol) { return false; }

	const auto* model = static_cast<const ColliderModel*>(_modelCol);
	if (model == nullptr) { return false; }

	int modelHandle = model->GetModelHandle();
	if (modelHandle == -1) { return false; }

	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Sphere(modelHandle, -1, _centerPos, _radius);

	if (hitResult.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitResult);
		return false;
	}

	float maxPenetration = -1.0f;
	int bestIndex = -1;

	ColliderBase::TAG modelTag = _modelCol->GetCollisionTag();

	for (int i = 0; i < hitResult.HitNum; ++i)
	{
		if (model->IsExcludedFrame(hitResult.Dim[i].FrameIndex)) { continue; }

		const auto& poly = hitResult.Dim[i];

		// タグによる法線フィルタ(カプセル版と同じ基準)
		if (modelTag == ColliderBase::TAG::STAGE && poly.Normal.y <= 0.5f) { continue; }
		if (modelTag == ColliderBase::TAG::WALL && poly.Normal.y > 0.5f) { continue; }

		VECTOR polyPoint = poly.Position[0];
		VECTOR toHit = VSub(polyPoint, _centerPos);
		float distAlongNormal = VDot(toHit, poly.Normal);
		float polyPenetration = _radius - distAlongNormal;

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

	// 呼び出し側が衝突情報を必要としない場合(スイープ判定など)はここで終了
	if (_outInfo == nullptr)
	{
		MV1CollResultPolyDimTerminate(hitResult);
		return true;
	}

	const auto& bestHit = hitResult.Dim[bestIndex];

	// 呼び出し側で設定する
	_outInfo->myCollider = nullptr;
	_outInfo->hitCollider = _modelCol;

	VECTOR bestPolyPoint = VScale(
		VAdd(VAdd(bestHit.Position[0], bestHit.Position[1]), bestHit.Position[2]),
		1.0f / 3.0f);

	_outInfo->hitPosition = bestPolyPoint;
	_outInfo->hitNormal = bestHit.Normal;
	_outInfo->isActive = true;

	VECTOR toHit = VSub(bestPolyPoint, _centerPos);
	float distAlongNormal = VDot(toHit, bestHit.Normal);
	_outInfo->penetration = _radius - distAlongNormal;

	if (_outInfo->penetration < 0.0f) { _outInfo->penetration = 0.0f; }

	MV1CollResultPolyDimTerminate(hitResult);
	return true;
}
