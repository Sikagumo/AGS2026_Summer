#include "CollisionController.h"
#include "../../Manager/System/TimeManager.h"
#include "../Common/Transform.h"
#include "../Actor/ActorBase.h"
#include "../Collider2D/Collider2DCircle.h"
#include "../Collider2D/Collider2DBox.h"
#include "../Collision/CollisionSphere.h"
#include "../Collision/CollisionCapsule.h"
#include "../Collision/CollisionLine.h"
#include "../Collision/CollisionCircle.h"
#include "../Collision/CollisionBox.h"
#include "../../Utility/UtilityMath.h"

CollisionController* CollisionController::instance_ = nullptr;

CollisionController::CollisionController(void)
	: cullingDistSquare_(0.0f)
	, updateTimer_(0.0f)
{
}

void CollisionController::CreateInstance(void)
{
	// 二重生成を防ぐためのチェック
	if (instance_ == nullptr)
	{
		instance_ = new CollisionController();
	}
}

CollisionController& CollisionController::GetInstance(void)
{
	return *instance_;
}

void CollisionController::DestroyInstance(void)
{
	// インスタンス破棄
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void CollisionController::Initialize(void)
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

void CollisionController::Update(void)
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

void CollisionController::Clear(void)
{
	actors_.clear();

	activeCollisions_.clear();

	ClearColliders2D();
}

void CollisionController::RegisterActor(ActorBase* _actor)
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

void CollisionController::UnregisterActor(ActorBase* _actor)
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

bool CollisionController::CheckCollision(const ColliderBase* _colliderA, const ColliderBase* _colliderB,
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
			return CollisionSphere::CheckHitWave(_colliderB, const_cast<ColliderBase*>(_colliderA));
		}

		return false;
			
	}

	if (_colliderB->GetCollisionTag() == TAG::HIT_WAVE)
	{
		if (_colliderA->GetShapeType() == SHAPE::CAPSULE)
		{
			return CollisionSphere::CheckHitWave(_colliderA, const_cast<ColliderBase*>(_colliderB));
		}
	}

	SHAPE shapeA = _colliderA->GetShapeType();
	SHAPE shapeB = _colliderB->GetShapeType();

	// 形状の組み合わせによる判定の振り分け
	if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::SPHERE)
	{
		return CollisionSphere::CheckSphereVsSphere(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::CAPSULE)
	{
		return CollisionSphere::CheckSphereVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::CAPSULE)
	{
		return CollisionCapsule::CheckCapsuleVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::SPHERE)
	{
		return CollisionCapsule::CheckCapsuleVsSphere(_colliderA, _colliderB, _outInfo);
	}
	
	// 地面（MODEL）との判定
	if (shapeA == SHAPE::LINE && shapeB == SHAPE::MODEL)
	{
		return CollisionLine::CheckLineVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::MODEL && shapeB == SHAPE::LINE)
	{
		return CollisionLine::CheckLineVsModel(_colliderB, _colliderA, _outInfo);
	}
	else if (shapeA == SHAPE::CAPSULE && shapeB == SHAPE::MODEL)
	{
		return CollisionCapsule::CheckCapsuleVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::MODEL && shapeB == SHAPE::CAPSULE)
	{
		return CollisionCapsule::CheckCapsuleVsModel(_colliderB, _colliderA, _outInfo);
	}
	else if (shapeA == SHAPE::SPHERE && shapeB == SHAPE::MODEL)
	{
		return CollisionSphere::CheckSphereVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (shapeA == SHAPE::MODEL && shapeB == SHAPE::SPHERE)
	{
		return CollisionSphere::CheckSphereVsModel(_colliderB, _colliderA, _outInfo);
	}
	
	return false;
}

bool CollisionController::IsActorCollidingWithTag(const ActorBase* _actor,
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

bool CollisionController::IsTagCollidingWithTag(ColliderBase::TAG _targetTagA,
	ColliderBase::TAG _targetTagB) const
{
	auto pair = (_targetTagA < _targetTagB) ? std::make_pair(_targetTagA, _targetTagB) : std::make_pair(_targetTagB, _targetTagA);

	if (activeCollisions_.count(pair) > 0)
	{
		return true;
	}

	return false;
}

VECTOR CollisionController::IsActorHitPosWithTag(const ActorBase* _actor, 
	ColliderBase::TAG _targetTag) const
{
	if (_actor == nullptr)
	{
		return UtilityMath::VECTOR_ZERO;
	}

	auto hitColInfo = currentColInfos_.find(_actor);
	
	// 当たった履歴が1つもなければ即座にゼロを返す
	if (hitColInfo == currentColInfos_.end())
	{
		return UtilityMath::VECTOR_ZERO;
	}

	const auto& hitInfos = hitColInfo->second;

	for (const auto& info : hitInfos)
	{
		if (info.hitCollider == nullptr)
		{
			continue;
		}

		if (info.hitCollider->GetCollisionTag() == _targetTag)
		{
			return info.hitPosition;
		}
	}

	return UtilityMath::VECTOR_ZERO;
}

void CollisionController::SetCollisionActive(ActorBase* _targetActor, 
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

void CollisionController::SetActorColliderRadius(ActorBase* _targetActor,
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

void CollisionController::ResolveCollision(ActorBase* _actorA, ActorBase* _actorB,
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

void CollisionController::UpdateCollisionPars(void)
{
	// 前フレームの衝突情報リセット
	for (auto& actor : actors_)
	{
		actor->ClearHitCollider();
	}

	activeCollisions_.clear();
	currentColInfos_.clear();

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

							currentColInfos_[actorA].push_back(info);

							CollisionInfo reorderInfo = info;
							reorderInfo.myCollider = info.hitCollider;
							reorderInfo.hitCollider = info.myCollider;
							reorderInfo.hitNormal = VScale(info.hitNormal, -1.0f);
							currentColInfos_[actorB].push_back(reorderInfo);

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

bool CollisionController::CanCollide(int _tagA, int _tagB) const
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
			|| tagHurt == TAG::MG_BULLET
			|| tagHurt == TAG::ROAD_ATTACK)
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


	if (tagHit == TAG::HIT_WAVE||tagHit==TAG::ROAD_ATTACK)
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

void CollisionController::RegisterCollider2D(Collider2DBase* _collider)
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

void CollisionController::UnregisterCollider2D(Collider2DBase* _collider)
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

void CollisionController::ClearColliders2D(void)
{
	colliders2D_.clear();
}

void CollisionController::SetCollisionGroup2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB, bool _isEnable)
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

void CollisionController::UpdateCollision2D(void)
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

bool CollisionController::CanCollide2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB) const
{
	size_t indexA = static_cast<size_t>(_tagA);
	size_t indexB = static_cast<size_t>(_tagB);

	if (indexA >= MATRIX_SIZE_2D || indexB >= MATRIX_SIZE_2D)
	{
		return false;
	}

	return collisionMatrix2D_[indexA][indexB];
}

bool CollisionController::CheckCollision2D(const Collider2DBase* _colA, const Collider2DBase* _colB) const
{
	using SHAPE = Collider2DBase::SHAPE_2D;
	SHAPE shapeA = _colA->GetShapeType();
	SHAPE shapeB = _colB->GetShapeType();

	if (shapeA == SHAPE::CIRCLE && shapeB == SHAPE::CIRCLE)
	{
		return CollisionCircle::CheckCircleVsCircle(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DCircle*>(_colB));
	}

	if (shapeA == SHAPE::BOX && shapeB == SHAPE::BOX)
	{
		return CollisionBox::CheckBoxVsBox(static_cast<const Collider2DBox*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (shapeA == SHAPE::CIRCLE && shapeB == SHAPE::BOX)
	{
		return CollisionCircle::CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (shapeA == SHAPE::BOX && shapeB == SHAPE::CIRCLE)
	{
		return CollisionCircle::CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colB), static_cast<const Collider2DBox*>(_colA));
	}

	return false;
}

bool CollisionController::IsTagCollidingWithTag2D(Collider2DBase::TAG_2D _targetTagA, 
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

void CollisionController::DrawDebug2D(void)
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