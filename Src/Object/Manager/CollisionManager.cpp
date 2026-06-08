#define NOMINMAX
#include "CollisionManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../Common/Transform.h"
#include "../Actor/ActorBase.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"
#include "../../Utility/UtilityMath.h"
#include "../Collider/ColliderLine.h"
#include "../Collider2D/Collider2DCircle.h"
#include "../Collider2D/Collider2DBox.h"

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

	colliders2D_.clear();

	for (size_t i = 0; i < MATRIX_SIZE_2D; ++i)
	{
		for (size_t j = 0; j < MATRIX_SIZE_2D; ++j)
		{
			collisionMatrix2D_[i][j] = false;
		}
	}
	
	SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON, true);
}

void CollisionManager::Update(void)
{
	updateTimer_ += TimeManager::GetInstance().GetDeltaTime();

	// 一定間隔ごとに衝突判定を実行
	if (updateTimer_ >= UPDATE_INTERVAL)
	{
		updateTimer_ = 0.0f;

		UpdateCollisionPars();

		UpdateCollision2D();
	}
}

void CollisionManager::Clear(void)
{
	actors_.clear();

	activeCollisions_.clear();

	ClearColliders2D();
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
	using TAG = ColliderBase::TAG;

	if (_colliderA->GetCollisionTag() == TAG::HIT_WAVE)
	{
		if (_colliderB->GetShapeType() == SHAPE::CAPSULE)
		{
			return CheckHitWave(_colliderB, const_cast<ColliderBase*>(_colliderA), HIT_WAVE_THICKNESS, HIT_WAVE_HEIGHT);
		}

		return false;
			
	}

	if (_colliderB->GetCollisionTag() == TAG::HIT_WAVE)
	{
		if (_colliderA->GetShapeType() == SHAPE::CAPSULE)
		{
			return CheckHitWave(_colliderA, const_cast<ColliderBase*>(_colliderB), HIT_WAVE_THICKNESS, HIT_WAVE_HEIGHT);
		}
	}

	SHAPE shapeA = _colliderA->GetShapeType();
	SHAPE shapeB = _colliderB->GetShapeType();

	// 形状の組み合わせによる判定の振り分け
	if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsSphere(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::CAPSULE)
	{
		return CheckSphereVsCapsule(_colliderB, _colliderA, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::SPHERE)
	{
		return CheckSphereVsCapsule(_colliderB, _colliderA, _outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::MODEL)
	{
		return CheckSphereVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::CAPSULE)
	{
		return CheckCapsuleVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	
	// 地面との判定はほかの判定と一緒にしない
	if (shapeA == SHAPE::LINE && shapeB == SHAPE::MODEL)
	{
		return CheckLineVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::MODEL)
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

bool CollisionManager::IsTagCollidingWithTag(ColliderBase::TAG _targetTagA,
	ColliderBase::TAG _targetTagB) const
{
	auto pair = (_targetTagA < _targetTagB) ? std::make_pair(_targetTagA, _targetTagB) : std::make_pair(_targetTagB, _targetTagA);

	if (activeCollisions_.count(pair) > 0)
	{
		return true;
	}

	return false;
}

void CollisionManager::SetCollisionActive(ActorBase* _targetActor, 
	ColliderBase::TAG _targetTag, const bool _isActive)
{
	if (_targetActor == nullptr)
	{
		return;
	}

	auto& ownColliders = _targetActor->GetOwnColliders();

	// コライダーマップ(または配列)をループで回す
	for (auto& [id, collider] : ownColliders)
	{
		if (collider == nullptr) { continue; }

		if (collider->GetCollisionTag() == _targetTag)
		{
			collider->SetActive(_isActive);
		}
	}
}

void CollisionManager::SetActorColliderRadius(ActorBase* _targetActor,
	ColliderBase::TAG _targetTag, float _radius)
{
	if (_targetActor == nullptr)
	{
		return;
	}

	auto& ownColliders = _targetActor->GetOwnColliders();

	// コライダーマップ(または配列)をループで回す
	for (auto& [id, collider] : ownColliders)
	{
		if (collider == nullptr) { continue; }

		if (collider->GetCollisionTag() == _targetTag)
		{
			collider->SetRadius(_radius);
		}
	}
}

void CollisionManager::ResolveCollision(ActorBase* _actorA, ActorBase* _actorB,
	const CollisionInfo& _info)
{
	if (_actorA == nullptr || _actorB == nullptr)
	{
		return;
	}

	using TAG = ColliderBase::TAG;

	// 通常の押し戻しベクトルを計算
	VECTOR pushVector = VScale(_info.hitNormal, _info.penetration);

	TAG tagA = _info.myCollider->GetCollisionTag();
	TAG tagB = _info.hitCollider->GetCollisionTag();

	if (tagA == TAG::STAGE && tagB == TAG::STAGE)
	{
		float overlap = fabsf(_info.penetration);
		VECTOR stagePush = VScale(VGet(0.0f, 1.0f, 0.0f), overlap);

		bool isHaveMyCollider = false;
		for (const auto& [id, col] : _actorA->GetOwnColliders())
		{
			if (col == _info.myCollider)
			{
				isHaveMyCollider = true;
				break;
			}
		}

		if (isHaveMyCollider)
		{
			_actorA->GetTransform().Translate(stagePush);
		}
		else
		{
			_actorB->GetTransform().Translate(stagePush);
		}
		return;
	}

	if (tagA != TAG::STAGE && tagB == TAG::STAGE)
	{
		float overlap = fabsf(_info.penetration);
		VECTOR stagePush = VGet(0.0f, overlap, 0.0f);

		_actorA->GetTransform().Translate(stagePush);
		return;
	}
	else if (tagA == TAG::STAGE && tagB != TAG::STAGE)
	{
		float overlap = fabsf(_info.penetration);
		VECTOR stagePush = VGet(0.0f, overlap, 0.0f);

		_actorB->GetTransform().Translate(stagePush);
		return;
	}

	pushVector.y = 0.0f;

	bool isHaveMyCollider = false;

	for (const auto& [id, col] : _actorA->GetOwnColliders())
	{
		if (col == _info.myCollider)
		{
			isHaveMyCollider = true;
			break;
		}
	}

	if (isHaveMyCollider)
	{
		_actorA->GetTransform().Translate(pushVector);
	}
	else
	{
		_actorB->GetTransform().Translate(VScale(pushVector, -1.0f));
	}
}

bool CollisionManager::CheckHitWave(const ColliderBase* _hitCapsuleCol, ColliderBase* _waveCol, float _waveThickness, float _waveHeight)
{
	if (!_hitCapsuleCol || !_waveCol)
	{
		return false;
	}

	const auto* capsule = dynamic_cast <const ColliderCapsule*>(_hitCapsuleCol);
	const auto* wave = dynamic_cast<const ColliderSphere*>(_waveCol);

	if (capsule == nullptr || wave == nullptr)
	{
		return false;
	}

	VECTOR wavePos = wave->GetWorldPosition();

	VECTOR capStartPos = capsule->GetWorldStartPos();
	VECTOR capEndPos = capsule->GetWorldEndPos();

	VECTOR nearestPos = GetNearestPointOnSegment(capStartPos, capEndPos, wavePos);

	VECTOR wavePosXZ = wavePos;
	VECTOR nearestPosXZ = nearestPos;

	wavePosXZ.y = 0.0f;
	nearestPosXZ.y = 0.0f;

	float distance = static_cast<float>(UtilityMath::Distance(nearestPosXZ, wavePosXZ));

	float waveRadius = wave->GetRadius();

	float totalThickness = _waveThickness + capsule->GetRadius();

	bool isHit = abs(distance - waveRadius) < totalThickness;

	if (!isHit)
	{
		return false;
	}

	float capsuleFootY = (capStartPos.y < capEndPos.y ? capStartPos.y : capEndPos.y) - capsule->GetRadius();

	float differenceY = nearestPos.y - wavePos.y;

	if (capsuleFootY > (wavePos.y + _waveHeight))
	{
		return false;
	}

	return true;
}

void CollisionManager::UpdateCollisionPars(void)
{
	// 前フレームの衝突情報リセット
	for (auto& actor : actors_)
	{
		actor->ClearHitCollider();
	}

	activeCollisions_.clear();

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

			bool isStageCollision = false;

			// アクターAのコライダーの中にSTAGEがあるかチェック
			for (const auto& [idA, colA] : collidersA) 
			{
				if (colA->GetCollisionTag() == ColliderBase::TAG::STAGE) { isStageCollision = true; break; }
			}

			// アクターBのコライダーの中にSTAGEがあるかチェック
			const auto& collidersB = actorB->GetOwnColliders();
			for (const auto& [idB, colB] : collidersB)
			{
				if (colB->GetCollisionTag() == ColliderBase::TAG::STAGE) { isStageCollision = true; break; }
			}

			// どちらもステージではない場合のみ、距離によるカリングを行う
			if (!isStageCollision)
			{
				VECTOR positionA = actorA->GetTransform().pos;
				VECTOR positionB = actorB->GetTransform().pos;
				float distanceX = positionB.x - positionA.x;
				float distanceY = positionB.y - positionA.y;
				float distanceZ = positionB.z - positionA.z;
				float distSquare = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);

				// 一定距離以上離れている場合は、詳細な判定をスキップ
				if (distSquare > cullingDistSquare_) { continue; }
			}
			
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

							auto tagA = colA->GetCollisionTag();
							auto tagB = colB->GetCollisionTag();

							auto pair = (tagA < tagB) ? std::make_pair(tagA, tagB) : std::make_pair(tagB, tagA);

							activeCollisions_.insert(pair);

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

		// ステージ同士のみ、衝突を許可する
		if (tagHit == TAG::STAGE) { return true; }
		return false;
	}

	// プレイヤーの衝突ルール
	if (tagHit == TAG::PLAYER || tagHit == TAG::PLAYER_BULLET)
	{
		if (tagHurt == TAG::ENEMY
			|| tagHurt == TAG::STAGE
			|| tagHurt == TAG::BOSS
			|| tagHurt == TAG::WEAPON_CANNON_L || tagHurt == TAG::WEAPON_CANNON_R
			|| tagHurt == TAG::WEAPON_MG_L || tagHurt == TAG::WEAPON_MG_R
			|| tagHurt == TAG::WEAPON_MP_L || tagHurt == TAG::WEAPON_MP_R
			|| tagHurt == TAG::WEAPON_RG
			|| tagHurt == TAG::HIT_WAVE
			|| tagHurt == TAG::MG_BULLET)
		{
			return true;
		}
	}
	
	if (tagHit == TAG::BOSS
		|| tagHit == TAG::WEAPON_CANNON_L || tagHit == TAG::WEAPON_CANNON_R
		|| tagHit == TAG::WEAPON_MG_L || tagHit == TAG::WEAPON_MG_R
		|| tagHit == TAG::WEAPON_MP_L || tagHit == TAG::WEAPON_MP_R
		|| tagHit == TAG::WEAPON_RG)
	{
		if (tagHurt == TAG::PLAYER || tagHurt == TAG::PLAYER_BULLET || tagHurt == TAG::STAGE)
		{
			return true;
		}
	}

	if (tagHit == TAG::MG_BULLET)
	{
		if (tagHurt == TAG::PLAYER || tagHurt == TAG::STAGE)
		{
			return true;
		}
	}


	if (tagHit == TAG::HIT_WAVE)
	{
		if (tagHurt == TAG::PLAYER)
		{
			return true;
		}
	}

	if (tagHit == TAG::STAGE)
	{
		if (tagHurt == TAG::PLAYER || tagHurt == TAG::PLAYER_BULLET || tagHurt == TAG::BOSS || tagHurt == TAG::WEAPON_CANNON_L || tagHurt == TAG::WEAPON_CANNON_R
			|| tagHurt == TAG::WEAPON_MG_L || tagHurt == TAG::WEAPON_MG_R
			|| tagHurt == TAG::WEAPON_MP_L || tagHurt == TAG::WEAPON_MP_R
			|| tagHurt == TAG::WEAPON_RG || tagHurt == TAG::MG_BULLET)
		{
			return true;
		}
		
	}

	return false;
}

bool CollisionManager::CheckSphereVsSphere(const ColliderBase* _colliderA,
	const ColliderBase* _colliderB, CollisionInfo& _outInfo)
{
	if (!_colliderA || !_colliderB) { return false; }

	const auto* sphereA = dynamic_cast<const ColliderSphere*>(_colliderA);
	const auto* sphereB = dynamic_cast<const ColliderSphere*>(_colliderB);

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

bool CollisionManager::CheckSphereVsCapsule(const ColliderBase* _sphereCol,
	const ColliderBase* _capsuleCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_capsuleCol) { return false; }

	const auto* sphereHit = dynamic_cast<const ColliderSphere*>(_sphereCol);
	const auto* capsuleHit = dynamic_cast<const ColliderCapsule*>(_capsuleCol);

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

bool CollisionManager::CheckSphereVsModel(const ColliderBase* _sphereCol, const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_sphereCol || !_modelCol) { return false; }

	const auto* sphere = dynamic_cast<const ColliderSphere*>(_sphereCol);
	const auto* model = dynamic_cast<const ColliderModel*>(_modelCol);

	if (sphere == nullptr || model == nullptr)
	{
		return false;
	}

	int modelHandle = model->GetModelHandle();

	if (modelHandle == -1)
	{
		return false;
	}

	VECTOR centerPos = sphere->GetWorldPosition();
	float radius = sphere->GetRadius();
	
	// 球体とモデル全体の衝突判定
	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Sphere(modelHandle, -1, centerPos);

	// 衝突の解析
	if (hitResult.HitNum > 0)
	{
		float maxUpward = -2.0f;
		int bestIndex = -1;

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			// 除外対象のフレームチェック
			if (model->IsExcludedFrame(hitResult.Dim[i].FrameIndex))
			{
				continue;
			}

			if (hitResult.Dim[i].Normal.y > maxUpward)
			{
				maxUpward = hitResult.Dim[i].Normal.y;
				bestIndex = i;
			}
		}

		if (bestIndex == -1)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		const auto& bestHit = hitResult.Dim[bestIndex];

		_outInfo.myCollider = _sphereCol;
		_outInfo.hitCollider = _modelCol;
		_outInfo.hitPosition = bestHit.HitPosition;
		_outInfo.hitNormal = bestHit.Normal;
		_outInfo.isActive = true;

		if (bestHit.Normal.y > 0.5f)
		{
			float sphereBottomY = centerPos.y - radius;

			_outInfo.penetration = bestHit.HitPosition.y - sphereBottomY;
		}
		else
		{
			float distance = UtilityMath::MagnitudeF(VSub(bestHit.HitPosition, centerPos));
			
			_outInfo.penetration = radius - distance;
		}

		// めり込み量が極端にマイナスにならないよにする
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

bool CollisionManager::CheckCapsuleVsModel(const ColliderBase* _capsuleCol,
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
			VECTOR nearestPos = GetNearestPointOnSegment(startPos, endPos, bestHit.HitPosition);
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

bool CollisionManager::CheckLineVsModel(const ColliderBase* _lineCol,
	const ColliderBase* _modelCol, CollisionInfo& _outInfo)
{
	if (!_lineCol || !_modelCol) { return false; }

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
		_outInfo.penetration = UtilityMath::MagnitudeF(VSub(endPos, hitResult.HitPosition));

		return true;
	}

	return false;
}

bool CollisionManager::CheckCapsuleVsCapsule(const ColliderBase* _colliderA,
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

	// 1. まずカプセルAの中点を仮のターゲットにする
	VECTOR centerA = VScale(VAdd(startPositionA, endPositionA), 0.5f);

	// 2. 既存の関数を使い、お互いの線分上の最近接点を交互に割り出して追い詰める
	// 2〜3回往復させるだけで、実用上全く問題のないレベルまで正確な2点に収束します
	VECTOR nearestPositionB = GetNearestPointOnSegment(startPositionB, endPositionB, centerA);
	VECTOR nearestPositionA = GetNearestPointOnSegment(startPositionA, endPositionA, 
		nearestPositionB);
	nearestPositionB = GetNearestPointOnSegment(startPositionB, endPositionB, nearestPositionA);

	// 3. 割り出した2点間の距離の2乗を計算する
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


void CollisionManager::RegisterCollider2D(Collider2DBase* _collider)
{
	if (_collider == nullptr)
	{
		return;
	}

	auto it = std::find(colliders2D_.begin(), colliders2D_.end(), _collider);
	if (it == colliders2D_.end())
	{
		colliders2D_.push_back(_collider);
	}
}

void CollisionManager::UnregisterCollider2D(Collider2DBase* _collider)
{
	if (_collider == nullptr)
	{
		return;
	}

	auto it = std::find(colliders2D_.begin(), colliders2D_.end(), _collider);
	if (it != colliders2D_.end())
	{
		colliders2D_.erase(it);
	}
}

void CollisionManager::ClearColliders2D(void)
{
	colliders2D_.clear();
}

void CollisionManager::SetCollisionGroup2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB, bool _isEnable)
{
	size_t indexA = static_cast<size_t>(_tagA);
	size_t indexB = static_cast<size_t>(_tagB);

	if (indexA >= MATRIX_SIZE_2D || indexB >= MATRIX_SIZE_2D)
	{
		return;
	}

	collisionMatrix2D_[indexA][indexB] = _isEnable;
	collisionMatrix2D_[indexB][indexA] = _isEnable;
}

void CollisionManager::UpdateCollision2D(void)
{
	activeCollisions2D_.clear();

	size_t count = colliders2D_.size();
	if (count < 2)
	{
		return;
	}

	for (size_t i = 0; i < count; ++i)
	{
		auto colA = colliders2D_[i];

		for (size_t j = i + 1; j < count; ++j)
		{
			auto colB = colliders2D_[j];

			if (CanCollide2D(colA->GetCollisionTag(), colB->GetCollisionTag()))
			{
				if (CheckCollision2D(colA, colB))
				{
					auto tagA = colA->GetCollisionTag();
					auto tagB = colB->GetCollisionTag();

					auto pair = (tagA < tagB) ? std::make_pair(tagA, tagB) : std::make_pair(tagB, tagA);
					activeCollisions2D_.insert(pair);
				}
			}
		}
	}
}

bool CollisionManager::CanCollide2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB) const
{
	size_t indexA = static_cast<size_t>(_tagA);
	size_t indexB = static_cast<size_t>(_tagB);

	if (indexA >= MATRIX_SIZE_2D || indexB >= MATRIX_SIZE_2D)
	{
		return false;
	}

	return collisionMatrix2D_[indexA][indexB];
}

bool CollisionManager::CheckCollision2D(const Collider2DBase* _colA, const Collider2DBase* _colB) const
{
	using SHAPE = Collider2DBase::SHAPE_2D;
	SHAPE shapeA = _colA->GetShapeType();
	SHAPE shapeB = _colB->GetShapeType();

	if (shapeA == SHAPE::CIRCLE && shapeB == SHAPE::CIRCLE)
	{
		return CheckCircleVsCircle(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DCircle*>(_colB));
	}

	if (shapeA == SHAPE::BOX && shapeB == SHAPE::BOX)
	{
		return CheckBoxVsBox(static_cast<const Collider2DBox*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (shapeA == SHAPE::CIRCLE && shapeB == SHAPE::BOX)
	{
		return CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (shapeA == SHAPE::BOX && shapeB == SHAPE::CIRCLE)
	{
		return CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colB), static_cast<const Collider2DBox*>(_colA));
	}

	return false;
}

bool CollisionManager::IsTagCollidingWithTag2D(Collider2DBase::TAG_2D _targetTagA, 
	Collider2DBase::TAG_2D _targetTagB) const
{
	// 登録時と同じ順番（小さい順）にしてペアを作る
	auto pair = (_targetTagA < _targetTagB) ? std::make_pair(_targetTagA, _targetTagB) 
		: std::make_pair(_targetTagB, _targetTagA);

	// 履歴の中に存在していれば当たっている
	if (activeCollisions2D_.count(pair) > 0)
	{
		return true;
	}

	return false;
}

void CollisionManager::DrawDebug2D(void)
{
	// 登録されている2Dコライダーが一つもなければ何もしない
	if (colliders2D_.empty())
	{
		return;
	}

	// 通常時の色（緑）と、衝突時の色（赤）のカラーコードを取得
	int colorGreen = GetColor(0, 255, 0);
	int colorRed = GetColor(255, 0, 0);

	// 全ての2Dコライダーをループ処理して描画
	for (const auto* collider : colliders2D_)
	{
		if (collider == nullptr)
		{
			continue;
		}

		// 初期状態は緑色に設定
		int drawColor = colorGreen;

		for (const auto* other : colliders2D_)
		{
			if (other == nullptr || collider == other)
			{
				continue;
			}

			// 自分と相手のタグの組み合わせが衝突履歴（activeCollisions2D_）にあるか調べる
			if (IsTagCollidingWithTag2D(collider->GetCollisionTag(), other->GetCollisionTag()))
			{
				// 衝突している組み合わせがあれば赤色にする
				drawColor = colorRed;
				break;
			}
		}

		// 各コライダー（円や矩形）が自分で持っている DrawDebug を呼び出して実際の形を描画
		collider->DrawDebug(drawColor);
	}
}

bool CollisionManager::CheckCircleVsCircle(const Collider2DBase* _circleA, const Collider2DBase* _circleB) const
{
	if (!_circleA || !_circleB) { return false; }

	const auto* circleA = dynamic_cast<const Collider2DCircle*>(_circleA);
	const auto* circleB = dynamic_cast<const Collider2DCircle*>(_circleB);

	if (circleA == nullptr || circleB == nullptr) { return false; }

	Vector2F posA = circleA->GetWorldCenterPos();
	Vector2F posB = circleB->GetWorldCenterPos();

	float distX = posB.x - posA.x;
	float distY = posB.y - posA.y;
	float distSquare = (distX * distX) + (distY * distY);

	float radiusSum = circleA->GetRadius() + circleB->GetRadius();
	float radiusSumSquare = radiusSum * radiusSum;

	if (distSquare < radiusSumSquare)
	{
		return true;
	}

	return false;
}

bool CollisionManager::CheckBoxVsBox(const Collider2DBase* _boxA, const Collider2DBase* _boxB) const
{
	if (!_boxA || !_boxB) { return false; }

	const auto* boxA = dynamic_cast<const Collider2DBox*>(_boxA);
	const auto* boxB = dynamic_cast<const Collider2DBox*>(_boxB);

	if (boxA == nullptr || boxB == nullptr) { return false; }

	Vector2F leftTopA = boxA->GetLeftTop();
	Vector2F rightBottomA = boxA->GetRightBottom();

	Vector2F leftTopB = boxB->GetLeftTop();
	Vector2F rightBottomB = boxB->GetRightBottom();

	if (rightBottomA.x < leftTopB.x) { return false; }
	if (leftTopA.x > rightBottomB.x) { return false; }
	if (rightBottomA.y < leftTopB.y) { return false; }
	if (leftTopA.y > rightBottomB.y) { return false; }

	return true;
}

bool CollisionManager::CheckCircleVsBox(const Collider2DBase* _circle, const Collider2DBase * _box) const
{
	if (!_circle || !_box) { return false; }

	const auto* circle = dynamic_cast<const Collider2DCircle*>(_circle);
	if (circle == nullptr)
	{
		circle = dynamic_cast<const Collider2DCircle*>(_box);
	}

	const auto* box = dynamic_cast<const Collider2DBox*>(_box);
	if (box == nullptr)
	{
		box = dynamic_cast<const Collider2DBox*>(_circle);
	}

	if (circle == nullptr || box == nullptr) { return false; }

	Vector2F circlePos = circle->GetWorldCenterPos();
	Vector2F boxLeftTop = box->GetLeftTop();
	Vector2F boxRightBottom = box->GetRightBottom();

	float closestX = std::max(boxLeftTop.x, std::min(circlePos.x, boxRightBottom.x));
	float closestY = std::max(boxLeftTop.y, std::min(circlePos.y, boxRightBottom.y));

	float distX = circlePos.x - closestX;
	float distY = circlePos.y - closestY;
	float distSquare = (distX * distX) + (distY * distY);

	float radiusSquare = circle->GetRadius() * circle->GetRadius();

	if (distSquare < radiusSquare)
	{
		return true;
	}

	return false;
}