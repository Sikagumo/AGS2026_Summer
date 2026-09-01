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
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"

CollisionController* CollisionController::instance_ = nullptr;

CollisionController::CollisionController(void)
	: cullingDistSquare_(0.0f)
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
	UpdateCollisionPars();
	UpdateCollision2D();
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

	for (const auto& actor : actors_)
	{
		if (actor == _actor)
		{
			return;
		}
	}

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
	if (_colliderA == nullptr || _colliderB == nullptr)
	{
		return false;
	}

	// 形状タイプ取得
	using SHAPE = ColliderBase::SHAPE;
	using TAG = ColliderBase::TAG;

	auto* nonConstColliderA = const_cast<ColliderBase*>(_colliderA);
	auto* nonConstColliderB = const_cast<ColliderBase*>(_colliderB);

	if (_colliderA->GetCollisionTag() == TAG::HIT_WAVE)
	{
		if (_colliderB->GetShapeType() == SHAPE::CAPSULE)
		{
			return CollisionSphere::CheckHitWave(nonConstColliderB, nonConstColliderA);
		}

		return false;
	}

	if (_colliderB->GetCollisionTag() == TAG::HIT_WAVE)
	{
		if (_colliderA->GetShapeType() == SHAPE::CAPSULE)
		{
			return CollisionSphere::CheckHitWave(nonConstColliderA, nonConstColliderB);
		}
	}

	const SHAPE SHAPE_A = _colliderA->GetShapeType();
	const SHAPE SHAPE_B = _colliderB->GetShapeType();

	// 形状の組み合わせによる判定の振り分け
	if (SHAPE_A == SHAPE::SPHERE && SHAPE_B == SHAPE::SPHERE)
	{
		return CollisionSphere::CheckSphereVsSphere(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::SPHERE && SHAPE_B == SHAPE::CAPSULE)
	{
		return CollisionSphere::CheckSphereVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::CAPSULE && SHAPE_B == SHAPE::CAPSULE)
	{
		return CollisionCapsule::CheckCapsuleVsCapsule(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::CAPSULE && SHAPE_B == SHAPE::SPHERE)
	{
		return CollisionCapsule::CheckCapsuleVsSphere(_colliderA, _colliderB, _outInfo);
	}

	// 地面（MODEL）との判定
	if (SHAPE_A == SHAPE::LINE && SHAPE_B == SHAPE::MODEL)
	{
		return CollisionLine::CheckLineVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::MODEL && SHAPE_B == SHAPE::LINE)
	{
		return CollisionLine::CheckLineVsModel(_colliderB, _colliderA, _outInfo);
	}

	if (SHAPE_A == SHAPE::CAPSULE && SHAPE_B == SHAPE::MODEL)
	{
		return CollisionCapsule::CheckCapsuleVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::MODEL && SHAPE_B == SHAPE::CAPSULE)
	{
		return CollisionCapsule::CheckCapsuleVsModel(_colliderB, _colliderA, _outInfo);
	}

	if (SHAPE_A == SHAPE::SPHERE && SHAPE_B == SHAPE::MODEL)
	{
		return CollisionSphere::CheckSphereVsModel(_colliderA, _colliderB, _outInfo);
	}
	else if (SHAPE_A == SHAPE::MODEL && SHAPE_B == SHAPE::SPHERE)
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

	const auto& HIT_COLLIDERS = _actor->GetHitCollider();

	for (const auto& hitCollider : HIT_COLLIDERS)
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
	const auto PAIR = (_targetTagA < _targetTagB) ? std::make_pair(_targetTagA, _targetTagB) : std::make_pair(_targetTagB, _targetTagA);

	if (activeCollisions_.count(PAIR) > 0)
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

	const auto HIT_COL_INFO = currentColInfos_.find(_actor);

	// 当たった履歴が1つもなければ即座にゼロを返す
	if (HIT_COL_INFO == currentColInfos_.end())
	{
		return UtilityMath::VECTOR_ZERO;
	}

	const auto& HIT_INFOS = HIT_COL_INFO->second;

	for (const auto& info : HIT_INFOS)
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

	auto& ownCollidersMap = _targetActor->GetOwnColliders();
	const int TARGET_KEY = static_cast<int>(_targetTag);

	auto it = const_cast<ActorBase::ColliderMap&>(ownCollidersMap).find(TARGET_KEY);
	if (it != const_cast<ActorBase::ColliderMap&>(ownCollidersMap).end())
	{
		for (auto* collider : it->second)
		{
			if (collider == nullptr)
			{
				continue;
			}

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

	auto& ownCollidersMap = _targetActor->GetOwnColliders();
	const int TARGET_KEY = static_cast<int>(_targetTag);
	auto it = const_cast<ActorBase::ColliderMap&>(ownCollidersMap).find(TARGET_KEY);

	if (it != const_cast<ActorBase::ColliderMap&>(ownCollidersMap).end())
	{
		for (auto* collider : it->second)
		{
			if (collider == nullptr)
			{
				continue;
			}

			collider->SetRadius(_radius);
		}
	}
}

void CollisionController::SetActorSphereLocalPos(ActorBase* _targetActor,
	ColliderBase::TAG _targetTag, const VECTOR& _localPosition)
{
	if (_targetActor == nullptr)
	{
		return;
	}

	auto& ownCollidersMap = _targetActor->GetOwnColliders();
	const int TARGET_KEY = static_cast<int>(_targetTag);
	auto it = const_cast<ActorBase::ColliderMap&>(ownCollidersMap).find(TARGET_KEY);

	// そもそも指定したタグが存在するかチェック
	if (it == const_cast<ActorBase::ColliderMap&>(ownCollidersMap).end())
	{
		return;
	}

	for (auto* collider : it->second)
	{
		if (collider == nullptr)
		{
			continue;
		}

		// 形状が球である場合のみキャストして位置を変更する
		if (collider->GetShapeType() == ColliderBase::SHAPE::SPHERE)
		{
			auto* sphereCollider = static_cast<ColliderSphere*>(collider);
			if (sphereCollider != nullptr)
			{
				sphereCollider->SetLocalPosition(_localPosition);
			}
		}
	}
}

void CollisionController::SetActorCapsuleShape(ActorBase* _targetActor,
	ColliderBase::TAG _targetTag, const VECTOR& _localStartPos, const VECTOR& _localEndPos,
	float _radius)
{
	if (_targetActor == nullptr)
	{
		return;
	}

	auto& ownCollidersMap = _targetActor->GetOwnColliders();
	const int TARGET_KEY = static_cast<int>(_targetTag);
	auto it = const_cast<ActorBase::ColliderMap&>(ownCollidersMap).find(TARGET_KEY);

	// そもそも指定したタグが存在するかチェック
	if (it == const_cast<ActorBase::ColliderMap&>(ownCollidersMap).end())
	{
		return;
	}

	for (auto* collider : it->second)
	{
		if (collider == nullptr)
		{
			continue;
		}

		// 形状がカプセルである場合のみキャストして形状を変更する
		if (collider->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
		{
			auto* capsuleCollider = static_cast<ColliderCapsule*>(collider);
			if (capsuleCollider != nullptr)
			{
				capsuleCollider->SetLocalStartPos(_localStartPos);
				capsuleCollider->SetLocalEndPos(_localEndPos);
				capsuleCollider->SetRadius(_radius);
			}
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

	// カメラの押し戻しはカメラ自身で行うため、カメラが関わる衝突はここで一切処理しない
	if (_info.myCollider->GetCollisionTag() == TAG::CAMERA ||
		_info.hitCollider->GetCollisionTag() == TAG::CAMERA)
	{
		return;
	}

	// 通常の押し戻しベクトルを計算
	VECTOR pushVector = VScale(_info.hitNormal, _info.penetration);

	// アクターAが静的オブジェクトのコライダーを持っているかチェック
	bool isActorAStatic = false;
	for (const auto& [id, colliderVector] : _actorA->GetOwnColliders())
	{
		for (const auto* collider : colliderVector)
		{
			if (collider->GetCollisionTag() == TAG::STAGE || collider->GetCollisionTag() == TAG::WALL)
			{
				isActorAStatic = true;
				break;
			}
		}
		if (isActorAStatic)
		{
			break;
		}
	}

	// アクターBが静的オブジェクトのコライダーを持っているかチェック
	bool isActorBStatic = false;
	for (const auto& [id, colliderVector] : _actorB->GetOwnColliders())
	{
		for (const auto* collider : colliderVector)
		{
			if (collider->GetCollisionTag() == TAG::STAGE || collider->GetCollisionTag() == TAG::WALL)
			{
				isActorBStatic = true;
				break;
			}
		}
		if (isActorBStatic)
		{
			break;
		}
	}

	// 両方が静的オブジェクトの場合は位置を動かさない
	if (isActorAStatic && isActorBStatic)
	{
		return;
	}

	if (!isActorAStatic && isActorBStatic)
	{
		if (fabsf(_info.hitNormal.y) < 0.5f)
		{
			pushVector.y = 0.0f;
		}

		_actorA->GetTransform().Translate(pushVector);
		return;
	}
	else if (isActorAStatic && !isActorBStatic)
	{
		if (fabsf(_info.hitNormal.y) < 0.5f)
		{
			pushVector.y = 0.0f;
		}

		_actorB->GetTransform().Translate(VScale(pushVector, -1.0f));
		return;
	}

	// 動くオブジェクト同士の衝突の場合は、Y軸の押し戻しを無視する
	pushVector.y = 0.0f;

	bool hasMyCollider = false;
	for (const auto& [id, colliderVector] : _actorA->GetOwnColliders())
	{
		for (const auto* collider : colliderVector)
		{
			if (collider == _info.myCollider)
			{
				hasMyCollider = true;
				break;
			}
		}
		if (hasMyCollider)
		{
			break;
		}
	}

	if (hasMyCollider)
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

	const size_t ACTOR_COUNT = actors_.size();

	// 判定対象が2つ未満なら処理終了
	if (ACTOR_COUNT < 2)
	{
		return;
	}

	// アクター間の総当たり判定
	for (size_t i = 0; i < ACTOR_COUNT; ++i)
	{
		auto actorA = actors_[i];
		const auto& COLLIDERS_MAP_A = actorA->GetOwnColliders();

		for (size_t j = i + 1; j < ACTOR_COUNT; ++j)
		{
			auto actorB = actors_[j];
			const auto& COLLIDERS_MAP_B = actorB->GetOwnColliders();

			bool isStageCollision = false;

			// アクターAのコライダーの中にSTAGEがあるかチェック
			for (const auto& [idA, colliderVectorA] : COLLIDERS_MAP_A)
			{
				for (const auto* colA : colliderVectorA)
				{
					if (colA->GetCollisionTag() == ColliderBase::TAG::STAGE
						|| colA->GetCollisionTag() == ColliderBase::TAG::WALL)
					{
						isStageCollision = true;
						break;
					}
				}
				if (isStageCollision)
				{
					break;
				}
			}

			// アクターBのコライダーの中にSTAGEがあるかチェック
			for (const auto& [idB, colliderVectorB] : COLLIDERS_MAP_B)
			{
				for (const auto* colB : colliderVectorB)
				{
					if (colB->GetCollisionTag() == ColliderBase::TAG::STAGE
						|| colB->GetCollisionTag() == ColliderBase::TAG::WALL)
					{
						isStageCollision = true;
						break;
					}
				}
				if (isStageCollision)
				{
					break;
				}
			}

			// どちらもステージではない場合のみ、距離によるカリングを行う
			if (!isStageCollision)
			{
				const VECTOR POS_A = actorA->GetTransform().pos;
				const VECTOR POS_B = actorB->GetTransform().pos;
				const float DIST_X = POS_B.x - POS_A.x;
				const float DIST_Y = POS_B.y - POS_A.y;
				const float DIST_Z = POS_B.z - POS_A.z;
				const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y) + (DIST_Z * DIST_Z);

				// 一定距離以上離れている場合は、詳細な判定をスキップ
				if (DIST_SQUARE > cullingDistSquare_)
				{
					continue;
				}
			}

			// マップから vector を取り出すループ
			for (const auto& [idA, colliderVectorA] : COLLIDERS_MAP_A)
			{
				for (const auto* colA : colliderVectorA)
				{
					if (colA == nullptr || !colA->IsActive())
					{
						continue;
					}

					// 相手のマップから vector を取り出すループ
					for (const auto& [idB, colliderVectorB] : COLLIDERS_MAP_B)
					{
						for (const auto* colB : colliderVectorB)
						{
							if (colB == nullptr || !colB->IsActive())
							{
								continue;
							}

							// 衝突タグによる判定可否の確認
							if (CanCollide(static_cast<int>(colA->GetCollisionTag()),
								static_cast<int>(colB->GetCollisionTag())))
							{
								CollisionInfo info;

								if (CheckCollision(colA, colB, info))
								{
									actorA->AddHitCollider(colB);
									actorB->AddHitCollider(colA);

									currentColInfos_[actorA].push_back(info);

									CollisionInfo reorderInfo = info;
									reorderInfo.myCollider = info.hitCollider;
									reorderInfo.hitCollider = info.myCollider;
									reorderInfo.hitNormal = VScale(info.hitNormal, -1.0f);
									currentColInfos_[actorB].push_back(reorderInfo);

									const auto TAG_A = colA->GetCollisionTag();
									const auto TAG_B = colB->GetCollisionTag();

									const auto PAIR = (TAG_A < TAG_B) ? std::make_pair(TAG_A, TAG_B) : std::make_pair(TAG_B, TAG_A);
									activeCollisions_.insert(PAIR);

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
	}
}

bool CollisionController::CanCollide(int _tagA, int _tagB) const
{
	using TAG = ColliderBase::TAG;
	const TAG TAG_HIT = static_cast<TAG>(_tagA);
	const TAG TAG_HURT = static_cast<TAG>(_tagB);

	// 同一タグ同士の判定
	if (TAG_HIT == TAG_HURT)
	{
		// エネミー同士のみ、衝突を許可する
		if (TAG_HIT == TAG::ENEMY) { return true; }

		// エネミーロボ同士のみ、衝突を許可する
		if (TAG_HIT == TAG::ENEMYROBO) { return true; }

		// ステージ同士のみ、衝突を許可する
		if (TAG_HIT == TAG::STAGE) { return true; }
		return false;
	}

	// プレイヤーの衝突ルール
	if (TAG_HIT == TAG::PLAYER || TAG_HIT == TAG::PLAYER_BULLET)
	{
		if (TAG_HURT == TAG::ENEMY
			|| TAG_HURT == TAG::STAGE || TAG_HURT == TAG::TREE
			|| TAG_HURT == TAG::BOSS
			|| TAG_HURT == TAG::WEAPON_CANNON_L || TAG_HURT == TAG::WEAPON_CANNON_R
			|| TAG_HURT == TAG::WEAPON_MG_L || TAG_HURT == TAG::WEAPON_MG_R
			|| TAG_HURT == TAG::WEAPON_MP_L || TAG_HURT == TAG::WEAPON_MP_R
			|| TAG_HURT == TAG::WEAPON_RG
			|| TAG_HURT == TAG::HIT_WAVE
			|| TAG_HURT == TAG::MG_BULLET
			|| TAG_HURT == TAG::ROAD_ATTACK
			|| TAG_HURT == TAG::CANNON_BULLET
			|| TAG_HURT == TAG::LASER
			|| TAG_HURT == TAG::MISSILE_ATTACK
			|| TAG_HURT == TAG::MISSILE_PUSH
			|| TAG_HURT == TAG::PLAYER_RECOVERY
			|| TAG_HURT == TAG::WALL
			|| TAG_HURT == TAG::ENEMYROBO
			|| TAG_HURT == TAG::ENEMY_ATTACK)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::ENEMYROBO || TAG_HIT == TAG::ENEMY_ATTACK)
	{
		if (TAG_HURT == TAG::STAGE || TAG_HURT == TAG::TREE
			|| TAG_HURT == TAG::PLAYER_BULLET || TAG_HURT == TAG::PLAYER_BLAST
			|| TAG_HURT == TAG::LASER
			|| TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::WALL)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::BOSS || TAG_HIT == TAG::ENEMY
		|| TAG_HIT == TAG::WEAPON_CANNON_L || TAG_HIT == TAG::WEAPON_CANNON_R
		|| TAG_HIT == TAG::WEAPON_MG_L || TAG_HIT == TAG::WEAPON_MG_R
		|| TAG_HIT == TAG::WEAPON_MP_L || TAG_HIT == TAG::WEAPON_MP_R
		|| TAG_HIT == TAG::WEAPON_RG
		|| TAG_HIT == TAG::PLAYER_RECOVERY
		)
	{
		if (TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::PLAYER_BULLET || TAG_HURT == TAG::PLAYER_BLAST
			|| TAG_HURT == TAG::STAGE || TAG_HURT == TAG::TREE)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::MG_BULLET
		|| TAG_HIT == TAG::CANNON_BULLET)
	{
		if (TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::STAGE || TAG_HURT == TAG::TREE)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::HIT_WAVE
		|| TAG_HIT == TAG::ROAD_ATTACK
		|| TAG_HIT == TAG::LASER
		|| TAG_HIT == TAG::MISSILE_ATTACK
		|| TAG_HIT == TAG::MISSILE_PUSH)
	{
		if (TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::WALL
			|| TAG_HURT == TAG::ENEMYROBO
			|| TAG_HURT == TAG::TREE)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::STAGE || TAG_HIT == TAG::TREE)
	{
		if (TAG_HURT == TAG::PLAYER || TAG_HURT == TAG::PLAYER_BULLET || TAG_HURT == TAG::PLAYER_BLAST
			|| TAG_HURT == TAG::BOSS || TAG_HURT == TAG::ENEMYROBO
			|| TAG_HURT == TAG::WEAPON_CANNON_L || TAG_HURT == TAG::WEAPON_CANNON_R
			|| TAG_HURT == TAG::WEAPON_MG_L || TAG_HURT == TAG::WEAPON_MG_R
			|| TAG_HURT == TAG::WEAPON_MP_L || TAG_HURT == TAG::WEAPON_MP_R
			|| TAG_HURT == TAG::WEAPON_RG || TAG_HURT == TAG::MG_BULLET
			|| TAG_HURT == TAG::CAMERA)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::WALL)
	{
		if (TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::PLAYER_BULLET
			|| TAG_HURT == TAG::PLAYER_BLAST
			|| TAG_HURT == TAG::ROAD_ATTACK
			|| TAG_HURT == TAG::ENEMYROBO
			|| TAG_HURT == TAG::CAMERA)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::CAMERA)
	{
		if (TAG_HURT == TAG::STAGE
			|| TAG_HURT == TAG::TREE
			|| TAG_HURT == TAG::WALL)
		{
			return true;
		}
	}

	if (TAG_HIT == TAG::TREE)
	{
		if (TAG_HURT == TAG::PLAYER
			|| TAG_HURT == TAG::ENEMY)
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
	const size_t INDEX_A = static_cast<size_t>(_tagA);
	const size_t INDEX_B = static_cast<size_t>(_tagB);

	if (INDEX_A >= MATRIX_SIZE_2D || INDEX_B >= MATRIX_SIZE_2D)
	{
		return;
	}

	collisionMatrix2D_[INDEX_A][INDEX_B] = _isEnable;
	collisionMatrix2D_[INDEX_B][INDEX_A] = _isEnable;
}

void CollisionController::UpdateCollision2D(void)
{
	activeCollisions2D_.clear();

	const size_t COUNT = colliders2D_.size();
	if (COUNT < 2)
	{
		return;
	}

	for (size_t i = 0; i < COUNT; ++i)
	{
		auto* colA = colliders2D_[i];

		for (size_t j = i + 1; j < COUNT; ++j)
		{
			auto* colB = colliders2D_[j];

			if (CanCollide2D(colA->GetCollisionTag(), colB->GetCollisionTag()))
			{
				if (CheckCollision2D(colA, colB))
				{
					const auto TAG_A = colA->GetCollisionTag();
					const auto TAG_B = colB->GetCollisionTag();

					const auto PAIR = (TAG_A < TAG_B) ? std::make_pair(TAG_A, TAG_B) : std::make_pair(TAG_B, TAG_A);
					activeCollisions2D_.insert(PAIR);
				}
			}
		}
	}
}

bool CollisionController::CanCollide2D(Collider2DBase::TAG_2D _tagA, Collider2DBase::TAG_2D _tagB) const
{
	const size_t INDEX_A = static_cast<size_t>(_tagA);
	const size_t INDEX_B = static_cast<size_t>(_tagB);

	if (INDEX_A >= MATRIX_SIZE_2D || INDEX_B >= MATRIX_SIZE_2D)
	{
		return false;
	}

	return collisionMatrix2D_[INDEX_A][INDEX_B];
}

bool CollisionController::CheckCollision2D(const Collider2DBase* _colA, const Collider2DBase* _colB) const
{
	using SHAPE = Collider2DBase::SHAPE_2D;
	const SHAPE SHAPE_A = _colA->GetShapeType();
	const SHAPE SHAPE_B = _colB->GetShapeType();

	if (SHAPE_A == SHAPE::CIRCLE && SHAPE_B == SHAPE::CIRCLE)
	{
		return CollisionCircle::CheckCircleVsCircle(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DCircle*>(_colB));
	}

	if (SHAPE_A == SHAPE::BOX && SHAPE_B == SHAPE::BOX)
	{
		return CollisionBox::CheckBoxVsBox(static_cast<const Collider2DBox*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (SHAPE_A == SHAPE::CIRCLE && SHAPE_B == SHAPE::BOX)
	{
		return CollisionCircle::CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colA), static_cast<const Collider2DBox*>(_colB));
	}

	if (SHAPE_A == SHAPE::BOX && SHAPE_B == SHAPE::CIRCLE)
	{
		return CollisionCircle::CheckCircleVsBox(static_cast<const Collider2DCircle*>(_colB), static_cast<const Collider2DBox*>(_colA));
	}

	return false;
}

bool CollisionController::IsTagCollidingWithTag2D(Collider2DBase::TAG_2D _targetTagA,
	Collider2DBase::TAG_2D _targetTagB) const
{
	// 登録時と同じ順番にしてペアを作る
	const auto PAIR = (_targetTagA < _targetTagB) ? std::make_pair(_targetTagA, _targetTagB)
		: std::make_pair(_targetTagB, _targetTagA);

	// 履歴の中に存在していれば当たっている
	if (activeCollisions2D_.count(PAIR) > 0)
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

	// 通常時の色と衝突時の色のカラーコードを取得
	const int COLOR_GREEN = GetColor(0, 255, 0);
	const int COLOR_RED = GetColor(255, 0, 0);

	// 全ての2Dコライダーをループ処理して描画
	for (const auto* collider : colliders2D_)
	{
		if (collider == nullptr)
		{
			continue;
		}

		// 初期状態は緑色に設定
		int drawColor = COLOR_GREEN;

		for (const auto* other : colliders2D_)
		{
			if (other == nullptr || collider == other)
			{
				continue;
			}

			// 自分と相手のタグの組み合わせが衝突履歴にあるか調べる
			if (IsTagCollidingWithTag2D(collider->GetCollisionTag(), other->GetCollisionTag()))
			{
				// 衝突している組み合わせがあれば赤色にする
				drawColor = COLOR_RED;
				break;
			}
		}

		// 各コライダーが自分で持っている DrawDebug を呼び出して実際の形を描画
		//collider->DrawDebug(drawColor);
	}
}