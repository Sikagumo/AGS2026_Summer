#include "CollisionSphere.h"
#include <cmath>
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
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

bool CollisionSphere::CheckSphereVsSphere(const ColliderBase* _colliderA, const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	if (!_colliderA || !_colliderB)
	{
		return false;
	}

	const auto* sphereA = static_cast<const ColliderSphere*>(_colliderA);
	const auto* sphereB = static_cast<const ColliderSphere*>(_colliderB);

	if (sphereA == nullptr || sphereB == nullptr)
	{
		return false;
	}

	// 距離計算
	const VECTOR posA = sphereA->GetWorldPosition();
	const VECTOR posB = sphereB->GetWorldPosition();

	const float distX = posA.x - posB.x;
	const float distY = posA.y - posB.y;
	const float distZ = posA.z - posB.z;

	// 距離の二乗を計算
	const float distSquare = (distX * distX) + (distY * distY) + (distZ * distZ);

	// 半径の合計値
	const float radiusSum = sphereA->GetRadius() + sphereB->GetRadius();

	// 衝突判定
	if (distSquare < (radiusSum * radiusSum))
	{
		const float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _colliderA;
		_outInfo.hitCollider = _colliderB;
		_outInfo.isActive = true;

		// 衝突位置の計算
		_outInfo.hitPosition = VAdd(posB, VScale(VSub(posA, posB), VALUE_HALF_F));

		// 法線ベクトルと押し出し量の計算
		if (distance > VALUE_ZERO_F)
		{
			_outInfo.hitNormal = VScale(VSub(posA, posB), VALUE_ONE_F / distance);
		}

		// めり込んでいる距離を算出
		_outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsCapsule(const ColliderBase* _sphereCol, const ColliderBase* _capsuleCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_capsuleCol)
	{
		return false;
	}

	const auto* sphere = static_cast<const ColliderSphere*>(_sphereCol);
	const auto* capsule = static_cast<const ColliderCapsule*>(_capsuleCol);

	if (sphere == nullptr || capsule == nullptr)
	{
		return false;
	}

	// 各形状のパラメータ取得
	const VECTOR spherePos = sphere->GetWorldPosition();
	const VECTOR capStartPos = capsule->GetWorldStartPos();
	const VECTOR capEndPos = capsule->GetWorldEndPos();
	const float sphereRadius = sphere->GetRadius();
	const float capsuleRadius = capsule->GetRadius();

	// カプセルの線分上で、球体に最も近い点を算出
	const VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(capStartPos, capEndPos, spherePos);

	// 最近接点と球体の中心距離による判定
	const float distSquare = static_cast<float>(UtilityMath::SqrMagnitude(spherePos, nearestPos));
	const float radiusSum = sphereRadius + capsuleRadius;

	// 衝突判定
	if (distSquare < (radiusSum * radiusSum))
	{
		const float distance = sqrtf(distSquare);

		// 衝突情報の設定
		_outInfo.myCollider = _sphereCol;
		_outInfo.hitCollider = _capsuleCol;
		_outInfo.isActive = true;

		// 衝突位置の計算
		_outInfo.hitPosition = UtilityMath::Lerp(nearestPos, spherePos, VALUE_HALF_F);

		// 法線ベクトルと押し出し量の計算
		if (distance > VALUE_ZERO_F)
		{
			_outInfo.hitNormal = VScale(VSub(spherePos, nearestPos), VALUE_ONE_F / distance);
		}
		_outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsModel(const ColliderBase* _sphereCol, const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_modelCol)
	{
		return false;
	}

	const auto* sphere = static_cast<const ColliderSphere*>(_sphereCol);
	if (sphere == nullptr)
	{
		return false;
	}

	const bool isHit = CheckSphereVsModelCore(sphere->GetWorldPosition(), sphere->GetRadius(), _modelCol, &_outInfo);

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

	const auto* capsule = static_cast <const ColliderCapsule*>(_hitCapsuleCol);
	const auto* wave = static_cast<const ColliderSphere*>(_waveCol);

	if (capsule == nullptr || wave == nullptr)
	{
		return false;
	}

	const VECTOR wavePos = wave->GetWorldPosition();
	const VECTOR capStartPos = capsule->GetWorldStartPos();
	const VECTOR capEndPos = capsule->GetWorldEndPos();

	const VECTOR nearestPos = UtilityMath::GetNearestPointOnSegment(capStartPos, capEndPos, wavePos);

	// Y軸を無視したXZ平面での位置
	const VECTOR wavePosXZ = VGet(wavePos.x, VALUE_ZERO_F, wavePos.z);
	const VECTOR nearestPosXZ = VGet(nearestPos.x, VALUE_ZERO_F, nearestPos.z);

	const float distance = static_cast<float>(UtilityMath::Distance(nearestPosXZ, wavePosXZ));
	const float waveRadius = wave->GetRadius();
	const float totalThickness = HIT_WAVE_THICKNESS + capsule->GetRadius();

	const bool isHit = std::abs(distance - waveRadius) < totalThickness;

	if (!isHit)
	{
		return false;
	}

	const float capsuleFootY = (capStartPos.y < capEndPos.y ? capStartPos.y : capEndPos.y) - capsule->GetRadius();

	if (capsuleFootY > (wavePos.y + HIT_WAVE_HEIGHT))
	{
		return false;
	}

	return true;
}

bool CollisionSphere::CheckSphereVsModelCore(const VECTOR& _centerPos, float _radius, const ColliderBase* _modelCol, CollisionInfo* _outInfo)
{
	if (!_modelCol)
	{
		return false;
	}

	const auto* model = static_cast<const ColliderModel*>(_modelCol);
	if (model == nullptr)
	{
		return false;
	}

	const int modelHandle = model->GetModelHandle();
	if (modelHandle == INVALID_INDEX)
	{
		return false;
	}

	const MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Sphere(modelHandle, INVALID_INDEX, _centerPos, _radius);

	if (hitResult.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(hitResult);
		return false;
	}

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

		// タグによる法線フィルタ(カプセル版と同じ基準)
		if (modelTag == ColliderBase::TAG::STAGE && poly.Normal.y <= NORMAL_Y_THRESHOLD)
		{
			continue;
		}
		if (modelTag == ColliderBase::TAG::WALL && poly.Normal.y > NORMAL_Y_THRESHOLD)
		{
			continue;
		}

		const VECTOR polyPoint = poly.Position[0];
		const VECTOR toHit = VSub(polyPoint, _centerPos);
		const float distAlongNormal = VDot(toHit, poly.Normal);
		const float polyPenetration = _radius - distAlongNormal;

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

	// 呼び出し側が衝突情報を必要としない場合はここで終了
	if (_outInfo == nullptr)
	{
		MV1CollResultPolyDimTerminate(hitResult);
		return true;
	}

	const auto& bestHit = hitResult.Dim[bestIndex];

	// 呼び出し側で設定する
	_outInfo->myCollider = nullptr;
	_outInfo->hitCollider = _modelCol;

	const VECTOR bestPolyPoint = VScale(
		VAdd(VAdd(bestHit.Position[0], bestHit.Position[1]), bestHit.Position[2]),
		VALUE_ONE_F / VALUE_THREE_F);

	_outInfo->hitPosition = bestPolyPoint;
	_outInfo->hitNormal = bestHit.Normal;
	_outInfo->isActive = true;

	const VECTOR toHit = VSub(bestPolyPoint, _centerPos);
	const float distAlongNormal = VDot(toHit, bestHit.Normal);

	_outInfo->penetration = _radius - distAlongNormal;

	if (_outInfo->penetration < VALUE_ZERO_F)
	{
		_outInfo->penetration = VALUE_ZERO_F;
	}

	MV1CollResultPolyDimTerminate(hitResult);
	return true;
}