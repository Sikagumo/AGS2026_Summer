#include "CollisionManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../Collider/ColliderBase.h"
#include "../Actor/ActorBase.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"
#include "../../Utility/UtilityMath.h"
#include "../Collider/ColliderLine.h"

CollisionManager* CollisionManager::instance_ = nullptr;

CollisionManager::CollisionManager(void)
{
}

void CollisionManager::CreateInstance(void)
{
	// 二重生成を防ぐためのチェック
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
}

CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

void CollisionManager::DestroyInstance(void)
{
	// インスタンス破棄
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void CollisionManager::Initialize(void)
{
	actors_.clear();

	// カリング距離の事前計算
	cullingDistSquare_ = DEFAULT_CULL_DIST * DEFAULT_CULL_DIST;

	updateTimer_ = 0.0f;
}

void CollisionManager::Update(void)
{
	updateTimer_ += SceneManager::GetInstance().GetDeltaTime();

	// 一定間隔ごとに衝突判定を実行
	if (updateTimer_ >= UPDATE_INTERVAL)
	{
		updateTimer_ = 0.0f;

		UpdateCollisionPars();
	}
}

void CollisionManager::Clear(void)
{
	actors_.clear();
}

void CollisionManager::RegisterActor(ActorBase* actor)
{
	// 重複登録防止
	if (actor == nullptr)
	{
		return;
	}

	for (const auto& actors : actors_)
	{
		if (actors == actor)
		{
			return;
		}
	}
	// リストへの追加
	actors_.push_back(actor);
}

void CollisionManager::UnregisterActor(ActorBase* actor)
{

	if (actor == nullptr)
	{
		return;
	}

	// 消去
	actors_.erase
	(
		std::remove(actors_.begin(), actors_.end(), actor),
		actors_.end()
	);
}

bool CollisionManager::CheckCollision(const ColliderBase* colliderA, const ColliderBase* colliderB,
	CollisionInfo& outInfo)
{
	// 有効性チェック
	if (!colliderA || !colliderB)
	{
		return false;
	}

	// 形状タイプ取得
	using SHAPE = ColliderBase::SHAPE;

	auto shapeA = colliderA->GetShapeType();
	auto shapeB = colliderB->GetShapeType();

	// 形状の組み合わせによる判定の振り分け
	if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsSphere(colliderA, colliderB, outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::CAPSULE)
	{
		return CheckSphereVsCapsule(colliderA, colliderB, outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsCapsule(colliderA, colliderB, outInfo);
	}
	
	if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::MODEL)
	{
		return CheckCapsuleVsModel(colliderA, colliderB, outInfo);
	}

	return false;
}

void CollisionManager::UpdateCollisionPars(void)
{
	// 前フレームの衝突情報リセット
	for (auto& actor : actors_)
	{
		actor->ClearHitCollider();
	}

	size_t actorCount = actors_.size();

	// 判定対象が2つ未満なら処理終了
	if (actorCount < 2) { return; }

	// アクター間の総当たり判定
	for (size_t i = 0; i < actorCount; ++i)
	{
		auto actorA = actors_[i];
		const auto& collidersA = actorA->GetOwnColliders();

		for (size_t j = i + 1; j < actorCount; ++j)
		{
			auto actorB = actors_[j];

			// 距離によるカリング
			VECTOR positionA = actorA->GetTransform().pos;
			VECTOR positionB = actorB->GetTransform().pos;
			float distanceX = positionB.x - positionA.x;
			float distanceY = positionB.y - positionA.y;
			float distanceZ = positionB.z - positionA.z;
			float distSquare = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);

			// 一定距離以上離れている場合は、詳細な判定をスキップ
			if (distSquare > cullingDistSquare_) { continue; }

			const auto& collidersB = actorB->GetOwnColliders();
			
			// コライダー同士の詳細判定
			for (auto& [idA, colA] : collidersA)
			{
				if (!colA->IsActive()) { continue; }

				for (auto& [idB, colB] : collidersB)
				{
					if (!colB->IsActive()) { continue; }

					// 衝突タグによる判定可否の確認
					if (CanCollide(static_cast<int>(colA->GetCollisionTag()),
						static_cast<int>(colB->GetCollisionTag())))
					{
						CollisionInfo info;

						if (CheckCollision(colA, colB, info))
						{
							// 衝突した相手を相互に登録
							actorA->AddHitCollider(colB);
							actorB->AddHitCollider(colA);
						}
					}
				}
			}
		}
	}
}

bool CollisionManager::CanCollide(int tagA, int tagB) const
{
	using TAG = ColliderBase::TAG;
	TAG tagHit = static_cast<TAG>(tagA);
	TAG tagHurt = static_cast<TAG>(tagB);

	// 同一タグ同士の判定
	if (tagHit == tagHurt)
	{
		// エネミー同士のみ、衝突を許可する
		if (tagHit == TAG::ENEMY) { return true; }

		return false;
	}

	// プレイヤーの衝突ルール
	if (tagHit == TAG::PLAYER)
	{
		if (tagHurt == TAG::ENEMY || tagHurt == TAG::STAGE)
		{
			return true;
		}
	}

	return false;
}

bool CollisionManager::CheckSphereVsSphere(const ColliderBase* colliderA, 
	const ColliderBase* colliderB, CollisionInfo& outInfo)
{
	const auto* sphereA = dynamic_cast<const ColliderSphere*>(colliderA);
	const auto* sphereB = dynamic_cast<const ColliderSphere*>(colliderB);

	if (!sphereA || !sphereB) { return false; }

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
		outInfo.myCollider = colliderA;
		outInfo.hitCollider = colliderB;
		outInfo.isActive = true;
		// 衝突位置の計算
		outInfo.hitPosition = VAdd(positionB, VScale(VSub(positionA, positionB), 0.5f));

		// 法線ベクトルと押し出し量の計算
		if (distance > 0.0f)
		{
			outInfo.hitNormal = VScale(VSub(positionA, positionB), 1.0f / distance);
		}

		// めり込んでいる距離を算出
		outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionManager::CheckSphereVsCapsule(const ColliderBase* sphereCol, 
	const ColliderBase* capsuleCol, CollisionInfo& outInfo)
{
	const auto* sphereHit = dynamic_cast<const ColliderSphere*>(sphereCol);
	const auto* capsuleHit = dynamic_cast<const ColliderCapsule*>(capsuleCol);

	if (!sphereCol || !capsuleCol) { return false; }

	// 各形状のパラメータ取得
	VECTOR spherePos = sphereHit->GetWorldPosition();
	VECTOR capStartPos = capsuleHit->GetWorldStartPos();
	VECTOR capEndPos = capsuleHit->GetWorldEndPos();
	float sphereRadius = sphereHit->GetRadius();
	float capsuleRadius = capsuleHit->GetRadius();

	// カプセルの線分上で、球体に最も近い点を算出
	VECTOR nearestPos = GetNearestPointOnSegment(capStartPos, capEndPos, spherePos);
	
	// 最近接点と球体の中心距離による判定
	float distSquare = static_cast<float>(UtilityMath::SqrMagnitude(spherePos, nearestPos));
	float radiusSum = sphereRadius + capsuleRadius;
	float radiusSumSq = radiusSum * radiusSum;

	// 衝突判定
	if (distSquare < radiusSumSq)
	{
		float distance = sqrtf(distSquare);

		// 衝突情報の設定
		outInfo.myCollider = sphereCol;
		outInfo.hitCollider = capsuleCol;
		outInfo.isActive = true;

		// 衝突位置の計算
		outInfo.hitPosition = UtilityMath::Lerp(nearestPos, spherePos, UtilityMath::HALF_NUM);

		// 法線ベクトルと押し出し量の計算
		if (distance > 0.0f)
		{
			outInfo.hitNormal = VScale(VSub(spherePos, nearestPos), 1.0f / distance);
		}
		outInfo.penetration = radiusSum - distance;

		return true;
	}

	return false;
}

bool CollisionManager::CheckCapsuleVsModel(const ColliderBase* capsuleCol, 
	const ColliderBase* modelCol, CollisionInfo& outInfo)
{
	const auto* capsule = dynamic_cast<const ColliderCapsule*>(capsuleCol);
	const auto* model = dynamic_cast<const ColliderModel*>(modelCol);

	if (!capsule || !model) { return false; };

	int modelHandle = model->GetModelHandle();

	if (modelHandle == -1) { return false; }

	VECTOR startPos = capsule->GetWorldStartPos();
	VECTOR endPos = capsule->GetWorldEndPos();
	float radius = capsule->GetRadius();

	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(modelHandle, -1,
		startPos, endPos, radius);

	if (hitResult.HitNum > 0)
	{
		const auto& bestHit = hitResult.Dim[0];

		if (model->IsExcludedFrame(bestHit.FrameIndex))
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		outInfo.myCollider = capsuleCol;
		outInfo.hitCollider = modelCol;
		outInfo.hitPosition = bestHit.HitPosition;
		outInfo.hitNormal = bestHit.Normal;
		outInfo.isActive = true;

		VECTOR nearestPos = GetNearestPointOnSegment(startPos, endPos, bestHit.HitPosition);
		float distance = UtilityMath::MagnitudeF(VSub(bestHit.HitPosition, nearestPos));
		outInfo.penetration = radius - distance;

		MV1CollResultPolyDimTerminate(hitResult);

		return true;
	}

	MV1CollResultPolyDimTerminate(hitResult);

	return false;
}

bool CollisionManager::CheckLineVsModel(const ColliderBase* lineCol, 
	const ColliderBase* modelCol, CollisionInfo& outInfo)
{
	const auto* line = dynamic_cast<const ColliderLine*>(lineCol);
	const auto* model = dynamic_cast<const ColliderModel*>(modelCol);

	if (!line || !model) { return false; }

	int modelHandle = model->GetModelHandle();

	if (modelHandle == -1) { return false; }

	VECTOR startPos = line->GetWorldStartPos();
	VECTOR endPos = line->GetWorldEndPos();

	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(modelHandle, -1, startPos, endPos);

	if (hitResult.HitFlag == 1)
	{
		if (model->IsExcludedFrame(hitResult.FrameIndex))
		{
			return false;
		}

		outInfo.myCollider = lineCol;
		outInfo.hitCollider = modelCol;
		outInfo.isActive = true;

		outInfo.hitPosition = hitResult.HitPosition;
		outInfo.hitNormal = hitResult.Normal;

		outInfo.penetration = 0.0f;

		return true;
	}

	return false;
}

VECTOR CollisionManager::GetNearestPointOnSegment(const VECTOR& startPos, 
	const VECTOR& endPos, const VECTOR& targetPos)
{
	VECTOR segmentVec = VSub(endPos, startPos);
	VECTOR toTargetVec = VSub(targetPos, startPos);

	float lenSquare = static_cast<float>(UtilityMath::SqrMagnitude(segmentVec));

	if (lenSquare < 1e-6)
	{
		return startPos;
	}

	float segmentRatio = VDot(toTargetVec, segmentVec) / lenSquare;

	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	VECTOR nearestPos = VAdd(startPos, VScale(segmentVec, segmentRatio));

	return nearestPos;
}
