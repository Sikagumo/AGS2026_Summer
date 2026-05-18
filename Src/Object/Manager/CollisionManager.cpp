#include "CollisionManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../Common/Transform.h"
#include "../Actor/ActorBase.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"
#include "../../Utility/UtilityMath.h"
#include "../Collider/ColliderLine.h"

CollisionManager* CollisionManager::instance_ = nullptr;

CollisionManager::CollisionManager(void)
	: cullingDistSquare_(0.0f)
	, updateTimer_(0.0f)
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

	activeCollisions_.clear();

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

	activeCollisions_.clear();
}

void CollisionManager::RegisterActor(ActorBase* _actor)
{
	// 重複登録防止
	if (_actor == nullptr)
	{
		return;
	}

	for (const auto& actors : actors_)
	{
		if (actors == _actor)
		{
			return;
		}
	}
	// リストへの追加
	actors_.push_back(_actor);
}

void CollisionManager::UnregisterActor(ActorBase* _actor)
{

	if (_actor == nullptr)
	{
		return;
	}

	// 消去
	actors_.erase
	(
		std::remove(actors_.begin(), actors_.end(), _actor),
		actors_.end()
	);
}

bool CollisionManager::CheckCollision(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
	CollisionInfo& _outInfo)
{
	// 有効性チェック
	if (!_colliderA || !_colliderB)
	{
		return false;
	}

	// 形状タイプ取得
	using SHAPE = ColliderBase::SHAPE;

	auto shapeA = _colliderA->GetShapeType();
	auto shapeB = _colliderB->GetShapeType();

	// 形状の組み合わせによる判定の振り分け
	if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsSphere(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::CAPSULE)
	{
		return CheckSphereVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	
	if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::MODEL)
	{
		return CheckCapsuleVsModel(_colliderA, _colliderB, _outInfo);
	}

	return false;
}

bool CollisionManager::IsActorCollidingWithTag(const ActorBase* _actor,
	ColliderBase::TAG _targetTag) const
{
	if (_actor == nullptr)
	{
		return false;
	}

	const auto& hitColliders = _actor->GetHitCollider();

	for (const auto& hitCollider : hitColliders)
	{
		if (hitCollider->GetCollisionTag() == _targetTag)
		{
			return true;
		}
	}

	return false;
}

void CollisionManager::ResolveCollision(ActorBase* _actorA, ActorBase* _actorB,
	const CollisionInfo& _info)
{
	if (_actorA == nullptr || _actorB == nullptr)
	{
		return;
	}

	using TAG = ColliderBase::TAG;

	// 押し戻すベクトルを計算
	VECTOR pushVector = VScale(_info.hitNormal, _info.penetration);

	bool isAHaveMyCollider = false;
	for (const auto& [id, col] : _actorA->GetOwnColliders())
	{
		if (col == _info.myCollider)
		{
			isAHaveMyCollider = true;
			break;
		}
	}

	if (isAHaveMyCollider)
	{
		if (_info.myCollider->GetCollisionTag() == TAG::PLAYER || _info.myCollider->GetCollisionTag() == TAG::BOSS)
		{
			_actorA->GetTransform().Translate(pushVector);;
		}
	}
	else
	{
		if (_info.myCollider->GetCollisionTag() == TAG::PLAYER || _info.myCollider->GetCollisionTag() == TAG::BOSS)
		{
			_actorB->GetTransform().Translate(pushVector);;
		}
	}
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

							if (!colA->IsTrigger() && !colB->IsTrigger())
							{
								ResolveCollision(actorA, actorB, info);
							}

						}
					}
				}
			}
		}
	}
}

bool CollisionManager::CanCollide(int _tagA, int _tagB) const
{
	using TAG = ColliderBase::TAG;
	TAG tagHit = static_cast<TAG>(_tagA);
	TAG tagHurt = static_cast<TAG>(_tagB);

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
		if (tagHurt == TAG::ENEMY || tagHurt == TAG::STAGE || tagHurt == TAG::BOSS)
		{
			return true;
		}
	}

	if (tagHit == TAG::BOSS)
	{
		if (tagHurt == TAG::PLAYER)
		{
			return true;
		}
	}

	return false;
}

bool CollisionManager::CheckSphereVsSphere(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	const auto* sphereA = dynamic_cast<const ColliderSphere*>(_colliderA);
	const auto* sphereB = dynamic_cast<const ColliderSphere*>(_colliderB);

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

bool CollisionManager::CheckSphereVsCapsule(const ColliderBase* _sphereCol,
	const ColliderBase* _capsuleCol, CollisionInfo& _outInfo)
{
	const auto* sphereHit = dynamic_cast<const ColliderSphere*>(_sphereCol);
	const auto* capsuleHit = dynamic_cast<const ColliderCapsule*>(_capsuleCol);

	if (!_sphereCol || !_capsuleCol) { return false; }

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

bool CollisionManager::CheckCapsuleVsModel(const ColliderBase* _capsuleCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	const auto* capsule = dynamic_cast<const ColliderCapsule*>(_capsuleCol);
	const auto* model = dynamic_cast<const ColliderModel*>(_modelCol);

	if (!capsule || !model) { return false; };

	// モデルハンドル取得
	int modelHandle = model->GetModelHandle();

	if (modelHandle == -1) { return false; }

	// 判定用パラメータ取得
	VECTOR startPos = capsule->GetWorldStartPos();
	VECTOR endPos = capsule->GetWorldEndPos();
	float radius = capsule->GetRadius();

	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(modelHandle, -1,
		startPos, endPos, radius);

	// 衝突結果の解析
	if (hitResult.HitNum > 0)
	{
		const auto& bestHit = hitResult.Dim[0];

		// 除外対象のフレームチェック
		if (model->IsExcludedFrame(bestHit.FrameIndex))
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		// 衝突情報の設定
		_outInfo.myCollider = _capsuleCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.hitPosition = bestHit.HitPosition;
		_outInfo.hitNormal = bestHit.Normal;
		_outInfo.isActive = true;

		// カプセルの軸（線分）上の最近接点を求め、正確なめり込み量を算出
		VECTOR nearestPos = GetNearestPointOnSegment(startPos, endPos, bestHit.HitPosition);
		float distance = UtilityMath::MagnitudeF(VSub(bestHit.HitPosition, nearestPos));
		_outInfo.penetration = radius - distance;

		// メモリ解放
		MV1CollResultPolyDimTerminate(hitResult);

		return true;
	}

	// 衝突しなかった場合のメモリ解放
	MV1CollResultPolyDimTerminate(hitResult);

	return false;
}

bool CollisionManager::CheckLineVsModel(const ColliderBase* _lineCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	const auto* line = dynamic_cast<const ColliderLine*>(_lineCol);
	const auto* model = dynamic_cast<const ColliderModel*>(_modelCol);

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

		_outInfo.myCollider = _lineCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.isActive = true;

		_outInfo.hitPosition = hitResult.HitPosition;
		_outInfo.hitNormal = hitResult.Normal;

		_outInfo.penetration = 0.0f;

		return true;
	}

	return false;
}

VECTOR CollisionManager::GetNearestPointOnSegment(const VECTOR& _startPos,
	const VECTOR& _endPos, const VECTOR& _targetPos)
{
	VECTOR segmentVec = VSub(_endPos, _startPos);
	VECTOR toTargetVec = VSub(_targetPos, _startPos);

	float lenSquare = static_cast<float>(UtilityMath::SqrMagnitude(segmentVec));

	if (lenSquare < 1e-6)
	{
		return _startPos;
	}

	float segmentRatio = VDot(toTargetVec, segmentVec) / lenSquare;

	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	VECTOR nearestPos = VAdd(_startPos, VScale(segmentVec, segmentRatio));

	return nearestPos;
}
