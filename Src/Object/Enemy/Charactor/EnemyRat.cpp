#include "EnemyRat.h"
#include "../EnemyBase.h"
#include "../../Common/Transform.h"
#include "../../Common/AnimationController.h"
#include "../../../Common/Quaternion.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Player/Player.h"

EnemyRat::EnemyRat(const EnemyBase::EnemyData& data, Player& _player):
	EnemyBase::EnemyBase(data, _player),
	state_(STATE::NONE),
	step_(0.0f)
{

}


void EnemyRat::InitLoadPost(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_RAT));

}

void EnemyRat::InitTransform(void)
{
	transform_.InitTransform(0.5f,
							 Quaternion::Identity(),
							 Quaternion::AngleAxis(180.0f, AsoUtility::AXIS_Y));
}

void EnemyRat::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);


	// 当たり判定リストに格納
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
}

void EnemyRat::InitAnimationPost(void)
{
	animation_->AddInternal(static_cast<int>(ANIM_TYPE::IDLE));
	animation_->AddInternal(static_cast<int>(ANIM_TYPE::WALK));

	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRat::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyRat::ChangeStateNone, this));

	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyRat::ChangeStateThink, this));

	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyRat::ChangeStateIdle, this));

	stateChanges_.emplace(static_cast<int>(STATE::WANDER),
		std::bind(&EnemyRat::ChangeStateWander, this));

	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyRat::ChangeStateEnd, this));

	// 初期状態設定
	ChangeState(STATE::THINK);
}


void EnemyRat::UpdateProcess(void)
{
	// 前フレーム位置取得
	transform_.prePos = transform_.pos;

	// 状態別更新
	stateUpdate_();

	if (state_ != STATE::THINK && state_ != STATE::NONE)
	{
		float delta = sceneMng_.GetDeltaTime();
		step_ -= ((step_ > 0.0f) ? delta : 0.0f);
		if (step_ <= 0.0f) { ChangeState(STATE::THINK); }
	}
}

void EnemyRat::UpdateProcessPost(void)
{
}

void EnemyRat::UpdateNone(void)
{

}
void EnemyRat::UpdateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateThink, this);

	// 思考
	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	const int RAND_WAIT = 30;
	int rand = GetRand(100);

	if (rand < RAND_WAIT)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::WANDER);
	}
}
void EnemyRat::UpdateIdle(void)
{
	
}
void EnemyRat::UpdateWander(void)
{
	movePow_ = VScale(moveDir_, moveSpeed_);

	// 移動範囲外の時
	if (!InMovableRange())
	{
		transform_.pos = transform_.prePos;
		ChangeState(STATE::THINK);
	}
}
void EnemyRat::UpdateEnd(void)
{
}


void EnemyRat::ChangeState(EnemyRat::STATE _state)
{
	state_ = _state;

	// 各状態遷移の初期処理
	EnemyBase::ChangeState(static_cast<int>(state_));

}
void EnemyRat::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateNone, this);
}
void EnemyRat::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateThink, this);

	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	int rand = GetRand(100);
	if (rand < 30)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::WANDER);
	}
}
void EnemyRat::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateIdle, this);

	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}
void EnemyRat::ChangeStateWander(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateWander, this);

	// ランダムな角度
	float angle = static_cast<float>(GetRand(360)) * AsoUtility::DEG2RAD;

	// 移動方向
	moveDir_ = VGet(cosf(angle), 0.0f, sinf(angle));

	const float STEP_TIME_MIN = 2.0f;
	const int STEP_TIME_RAND = 5;
	// ランダムな移動時間
	step_ = STEP_TIME_MIN + static_cast<float>(GetRand(STEP_TIME_RAND));

	// 移動スピード
	const float SPEED = 3.0f;
	moveSpeed_ = SPEED;

	// 歩きアニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}
void EnemyRat::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateEnd, this);
}