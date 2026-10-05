#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Manager/Generic/InputManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Manager/Decoration/SoundManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Manager/Decoration/EffectManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../../Camera/Camera.h"
#include "../../../Common/Transform.h"
#include "../../../Common/AnimationController.h"
#include "../../../Collider/ColliderBase.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderSphere.h"
#include "../../../Collision/CollisionController.h"
#include "EnemyRobo.h"

EnemyRobo::EnemyRobo(VECTOR _pos)
	: poizun_(false)
	, hp_(MAX_HP)
	, playerPos_({ 0.0f, 0.0f, 0.0f })
	, moveDir_({ 0.0f, 0.0f, 0.0f })
	, count_(0)
	, state_(STATE::IDLE)
	, stateBase_(-1)
	, stateUpdate_(nullptr)
{
	transform_.pos = _pos;
}

EnemyRobo::~EnemyRobo()
{
}

void EnemyRobo::Load(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMY_ROBO));
	transform_.Update();
}

void EnemyRobo::InitTransform(void)
{
	transform_.scl = INIT_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT_Y), UtilityMath::AXIS_Y);

	transform_.Update();
}

void EnemyRobo::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMYROBO, &transform_, COL_LINE_START_POS, COL_LINE_END_POS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMYROBO)].push_back(colLine);
	colLine->SetTriger(false);

	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMYROBO, &transform_, COL_CAPSULE_START_POS, COL_CAPSULE_END_POS, COL_CAPSULE_END_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMYROBO)].push_back(colCapsule);
	colCapsule->SetTriger(false);

	ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::ENEMYROBO, &transform_, COL_SPHERE_POS, COL_SPHERE_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMYROBO)].push_back(colSphere);
	colSphere->SetTriger(true);

	// 手のボーン座標から攻撃コライダー位置を算出
	VECTOR handPos = MV1GetFramePosition(transform_.modelId, BONE_HAND_INDEX);
	VECTOR handLocalPos = VSub(handPos, transform_.pos);

	ColliderSphere* colAttackSphere = new ColliderSphere(ColliderBase::TAG::ENEMY_ATTACK, &transform_, handLocalPos, COL_ATTACK_SPHERE_RADIUS);
	ownColliders_[static_cast<int>(ColliderBase::TAG::ENEMY_ATTACK)].push_back(colAttackSphere);
	colAttackSphere->SetTriger(false);

	CollisionController::GetInstance().RegisterActor(this);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMY_ATTACK, false);
}

void EnemyRobo::InitAnimation(void)
{
	CharaBase::InitAnimation();
	//アニメーションの追加
	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
	{
		animation_->AddInternal(i, ANIM_OFFSET, ANIM_SPEED);
	}
	animation_->Play(static_cast<int>(ANIM_TYPE::DIR));
}

void EnemyRobo::InitPost(void)
{
	hp_ = MAX_HP;

	stateChanges_.emplace(static_cast<int>(STATE::IDLE), std::bind(&EnemyRobo::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::ATTACK), std::bind(&EnemyRobo::ChangeStateAttack, this));
	stateChanges_.emplace(static_cast<int>(STATE::MOVE), std::bind(&EnemyRobo::ChangeStateMove, this));
	stateChanges_.emplace(static_cast<int>(STATE::END), std::bind(&EnemyRobo::ChangeStateEnd, this));
	ChangeState(STATE::IDLE);
}

void EnemyRobo::UpdateProcess(void)
{
	//死亡時に状態をENDに変更する
	if (hp_ <= 0)
	{
		if (state_ != STATE::END)
		{
			ChangeState(STATE::END);
		}
	}
	else if (hp_ > 0)
	{
		//攻撃判定にプレイヤーが入ったら攻撃状態に変更する
		bool isAttack = CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER);
		if (isAttack == true)
		{
			if (state_ != STATE::ATTACK)
			{
				ChangeState(STATE::ATTACK);
			}
		}
	}

	Damage();

	stateUpdate_();
}

void EnemyRobo::UpdateProcessPost(void)
{
}

void EnemyRobo::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);

#ifdef _DEBUG
	for (auto& [id, colliderVector] : ownColliders_)
	{
		for (auto* collider : colliderVector)
		{
			if (collider == nullptr)
			{
				continue;
			}

			collider->Draw();
		}
	}

	DrawFormatString(10, 200, 0xffffff, "enemyの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
}

void EnemyRobo::ChangeState(STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	EnemyRobo::ChangeState(state);
}

void EnemyRobo::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void EnemyRobo::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateIdle, this);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMY_ATTACK, false);
}

void EnemyRobo::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateAttack, this);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMY_ATTACK, true);

	//経過時間による攻撃アニメーションの切り替え
	float time = TimeManager::GetInstance().GetGameTime();
	if ((static_cast<int>(time) % ATTACK_ANIM_DIVISOR) == 0)
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::ATTACKA), false);
	}
	else
	{
		animation_->Play(static_cast<int>(ANIM_TYPE::ATTACKB), false);
	}
}

void EnemyRobo::ChangeStateMove(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateStateMove, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::WARK));
}

void EnemyRobo::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRobo::UpdateEnd, this);
	count_ = 0;

	EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_MISSILE, transform_.pos, END_EFFECT_ROT, END_EFFECT_SCALE, END_EFFECT_SPEED, this);

	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMYROBO, false);
	CollisionController::GetInstance().SetCollisionActive(this, ColliderBase::TAG::ENEMY_ATTACK, false);
}

void EnemyRobo::UpdateIdle(void)
{
	ChangeState(STATE::MOVE);
}

void EnemyRobo::UpdateAttack(void)
{
	VECTOR handPos = MV1GetFramePosition(transform_.modelId, BONE_HAND_INDEX);
	VECTOR worldOffset = VSub(handPos, transform_.pos);

	// ワールド回転を打ち消してローカル空間に戻す
	Quaternion invRot = transform_.quaRot.Inverse();
	VECTOR handLocalPos = invRot.PosAxis(worldOffset);

	CollisionController::GetInstance().SetActorSphereLocalPos(this, ColliderBase::TAG::ENEMY_ATTACK, handLocalPos);

	if (animation_->IsEnd() == true)
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyRobo::UpdateStateMove(void)
{
	LockPlayer();
	float speed = MOVE_SPEED_INIT;
	VECTOR movePow = VScale(moveDir_, speed);
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow);
}

void EnemyRobo::UpdateEnd(void)
{
	if (count_ >= COUNT_MAX)
	{
		EffectManager::GetInstance().Stop(EffectManager::EFFECT::EFFECT_MISSILE, this);
	}
	else
	{
		count_++;
	}
}

void EnemyRobo::LockPlayer(void)
{
	//プレイヤーの座標を取得して、敵の向きをプレイヤーの方向に向ける
	VECTOR moveDir = VSub(playerPos_, transform_.pos);
	moveDir.y = 0.0f;
	moveDir = VNorm(moveDir);
	moveDir_ = moveDir;
	float targetAngle = atan2(moveDir.x, moveDir.z);
	transform_.quaRot = Quaternion::AngleAxis(targetAngle, UtilityMath::AXIS_Y);
}

void EnemyRobo::Damage(void)
{
	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER_BLAST) ||
		CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER_BULLET) ||
		CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::LASER))
	{
		hp_ -= NORMAL_DAMAGE;
	}

	if (CollisionController::GetInstance().IsActorCollidingWithTag(this, ColliderBase::TAG::PLAYER_RECOVERY))
	{
		poizun_ = true; 
	}

	if (poizun_)
	{
		hp_ -= POISON_DAMAGE;
	}
}