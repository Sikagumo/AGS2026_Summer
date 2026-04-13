#include "EnemyRobot.h"
#include <vector>
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Common/AnimationController.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderModel.h"
#include "../../Player/Player.h"


EnemyRobot::EnemyRobot(const EnemyBase::EnemyData& data, Player& _player)
	:
	EnemyBase::EnemyBase(data, _player),
	state_(STATE::NONE),
	step_(0.0f),
	wayPoints_(),
	activeWayPointIndex_(0),
	nextWayPoint_(AsoUtility::VECTOR_ZERO),
	viewRangeTransform_(Transform())
{

}


void EnemyRobot::InitLoadPost(void)
{
	// モデルのロード
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_ROBOT));

	// 視野(円錐)モデルのロード
	viewRangeTransform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_VIEW_RANGE));

}

void EnemyRobot::InitTransform(void)
{
	transform_.InitTransform(1.0f,
							 Quaternion::Identity(),
							 Quaternion::AngleAxis(DEFAULT_LOCAL_ROT, AsoUtility::AXIS_Y));

	// 視野用円錐モデル
	viewRangeTransform_.InitTransform(VIEW_RANGE_SCL,
									  Quaternion::AngleAxis(VIEW_RANGE_ROT_X, AsoUtility::AXIS_X),
									  Quaternion::AngleAxis(VIEW_RANGE_LOCAL_ROT_X, AsoUtility::AXIS_X),
									  MV1GetFramePosition(transform_.modelId, VIEW_RANGE_SYNC_FRAME_IDX));
}
void EnemyRobot::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMY, &transform_,
											 COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::ENEMY, &transform_,
													  COL_CAPSULE_TOP_LOCAL_POS,
													  COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);


	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(viewRangeTransform_.modelId);

	// モデルのコライダ割り当て
	ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::VIEW_RANGE, &viewRangeTransform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::VIEW_RANGE), colModel);
}

void EnemyRobot::InitAnimationPost(void)
{
	animation_ = new AnimationController(transform_.modelId);

	// FBX内のアニメーション設定
	int type = -1;
	type = static_cast<int>(ANIM_TYPE::DANCE);
	animation_->AddInternal(type, 10.0f);
	type = static_cast<int>(ANIM_TYPE::IDLE);
	animation_->AddInternal(type, 20.0f);
	type = static_cast<int>(ANIM_TYPE::WALK);
	animation_->AddInternal(type, 30.0f);
	type = static_cast<int>(ANIM_TYPE::RUN);
	animation_->AddInternal(type, 30.0f);
	type = static_cast<int>(ANIM_TYPE::KICK);
	animation_->AddInternal(type, 45.0f);
	type = static_cast<int>(ANIM_TYPE::SHOOT);
	animation_->AddInternal(type, 30.0f);

	// 初期アニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRobot::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyRobot::ChangeStateNone, this));

	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyRobot::ChangeStateThink, this));

	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyRobot::ChangeStateIdle, this));

	stateChanges_.emplace(static_cast<int>(STATE::PATROL),
		std::bind(&EnemyRobot::ChangeStatePatrol, this));

	stateChanges_.emplace(static_cast<int>(STATE::SURPRISE),
		std::bind(&EnemyRobot::ChangeStateSurprise, this));

	stateChanges_.emplace(static_cast<int>(STATE::ALERT),
		std::bind(&EnemyRobot::ChangeStateAlert, this));

	stateChanges_.emplace(static_cast<int>(STATE::CHASE),
		std::bind(&EnemyRobot::ChangeStateChase, this));

	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_KICK),
		std::bind(&EnemyRobot::ChangeStateAttackKick, this));

	stateChanges_.emplace(static_cast<int>(STATE::ATTACK_SHOOT),
		std::bind(&EnemyRobot::ChangeStateAttackShoot, this));

	stateChanges_.emplace(static_cast<int>(STATE::ESCAPE),
		std::bind(&EnemyRobot::ChangeStateEscape, this));

	stateChanges_.emplace(static_cast<int>(STATE::DEAD),
		std::bind(&EnemyRobot::ChangeStateDead, this));

	stateChanges_.emplace(static_cast<int>(STATE::KNOCKBACK),
		std::bind(&EnemyRobot::ChangeStateKnockBack, this));

	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyRobot::ChangeStateEnd, this));


	// 巡回ルート
	const VECTOR POINT_1 = VGet(1926.18f, 1.76f, 618.34f);
	const VECTOR POINT_2 = VGet(2553.30f, -11.82f, -593.32f);
	const VECTOR POINT_3 = VGet(1400.34f, -26.21f, -457.11f);
	const VECTOR POINT_4 = VGet(1274.85f, -49.35f, 168.02f);
	wayPoints_.emplace_back(POINT_1);
	wayPoints_.emplace_back(POINT_2);
	wayPoints_.emplace_back(POINT_3);
	wayPoints_.emplace_back(POINT_4);

	nextWayPoint_ = wayPoints_[0];

	// 初期状態設定
	ChangeState(STATE::THINK);
}

void EnemyRobot::UpdateProcess(void)
{
	// 状態別更新
	stateUpdate_();
}
void EnemyRobot::UpdateProcessPost(void)
{
	EnemyBase::UpdateProcessPost();


	viewRangeTransform_.pos = MV1GetFramePosition(transform_.modelId, VIEW_RANGE_SYNC_FRAME_IDX);

	viewRangeTransform_.quaRot = transform_.quaRot.Mult(Quaternion::AngleAxis(VIEW_RANGE_ROT_X, AsoUtility::AXIS_X));

	viewRangeTransform_.Update();
}

bool EnemyRobot::IsSearchConeModel(void)
{
	bool ret = false;

	// 視野モデルコライダ
	int viewRangeType = static_cast<int>(COLLIDER_TYPE::VIEW_RANGE);

	// 視野モデルコライダが無ければ処理を抜ける
	if (ownColliders_.count(viewRangeType) == 0) { return ret; }

	// 視野モデルコライダ情報
	ColliderModel* colliderModel = dynamic_cast<ColliderModel*>(ownColliders_.at(viewRangeType));

	if (colliderModel == nullptr) { return ret; }

	// 衝突情報の更新
	MV1RefreshCollInfo(colliderModel->GetFollow()->modelId);


	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外はスキップ
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) { continue; }

		// 派生クラスへキャスト
		const ColliderCapsule* colliderCapsule = dynamic_cast<const ColliderCapsule*>(hitCol);

		if (colliderCapsule == nullptr) { continue; }

		if (colliderCapsule->IsHit(colliderModel, false, false))
		{
			return true;
		}
	}
	return	ret;
}

void EnemyRobot::ChangeState(STATE state)
{
	state_ = state;
	EnemyBase::ChangeState(static_cast<int>(state_));
}
void EnemyRobot::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateNone, this);
}
void EnemyRobot::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateThink, this);

	// 思考
	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	int rand = GetRand(100);
	if (rand < 30)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::PATROL);
	}
}
void EnemyRobot::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateIdle, this);

	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}
void EnemyRobot::ChangeStatePatrol(void)
{
	const float MOVE_SPEED = 5.0f;

	stateUpdate_ = std::bind(&EnemyRobot::UpdatePatrol, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	if (activeWayPointIndex_ == wayPoints_.size())
	{
		// 巡回終了
		activeWayPointIndex_ = 0;
		ChangeState(STATE::THINK);
		return;
	}

	// 巡回ポイントインデックス更新
	nextWayPoint_ = wayPoints_[activeWayPointIndex_];

	// 巡回ルートの移動方向を設定する
	SetMoveDirPatrol();

	// 移動スピード
	moveSpeed_ = MOVE_SPEED;

	// 歩きアニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::WALK));
}
void EnemyRobot::ChangeStateSurprise(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateSurprise, this);
}
void EnemyRobot::ChangeStateAlert(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAlert, this);

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// ダンス(足踏み)アニメーション再生
	animation_->Play(static_cast<int>(ANIM_TYPE::DANCE), true);
}
void EnemyRobot::ChangeStateChase(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateChase, this);
}
void EnemyRobot::ChangeStateAttackKick(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAttackKick, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::KICK), false);
}
void EnemyRobot::ChangeStateAttackShoot(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateAttackShoot, this);
	animation_->Play(static_cast<int>(ANIM_TYPE::SHOOT), false);
}
void EnemyRobot::ChangeStateEscape(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateEscape, this);
}
void EnemyRobot::ChangeStateDead(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateDead, this);
}
void EnemyRobot::ChangeStateKnockBack(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateKnockBack, this);
}
void EnemyRobot::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRobot::UpdateEnd, this);
}


void EnemyRobot::UpdateNone(void)
{
}
void EnemyRobot::UpdateThink(void)
{
}
void EnemyRobot::UpdateIdle(void)
{
	step_ -= sceneMng_.GetDeltaTime();

	if (step_ < 0.0f)
	{
		// 待機終了
		ChangeState(STATE::THINK);
		return;
	}

	// プレイヤー索敵
	
	if (IsSearchConeModel())
	{
		ChangeState(STATE::ALERT);
	}
}
void EnemyRobot::UpdatePatrol(void)
{
	const float PATROL_POINT_RADIUS = 30.0f;

	// 巡回ポイントとの球体衝突判定(半径30.0fくらい)
	if (AsoUtility::IsHitSpheres(transform_.pos, 1.0f,
								 nextWayPoint_, PATROL_POINT_RADIUS))
	{
		// 巡回ポイントインデックス更新
		activeWayPointIndex_++;

		// 次の移動地点へか、待機か思考
		ChangeState(STATE::THINK);
		return;
	}

	// 巡回ルートの移動方向を設定する
	SetMoveDirPatrol();

	// 移動量の計算
	movePow_ = VScale(moveDir_, moveSpeed_);
	

	// プレイヤー索敵
	
	if (IsSearchConeModel())
	{
		ChangeState(STATE::ALERT);
	}
}
void EnemyRobot::SetMoveDirPatrol(void)
{
	// 巡回先座標XZ
	VECTOR tmpPos = nextWayPoint_;
	tmpPos.y = 0.0f;

	// 現在地座標XZ
	VECTOR pos = transform_.pos;
	pos.y = 0.0f;

	// XZ平面上の移動方向を計算
	moveDir_ = VNorm(VSub(tmpPos, pos));
}

void EnemyRobot::UpdateSurprise(void)
{
}
void EnemyRobot::UpdateAlert(void)
{
}
void EnemyRobot::UpdateChase(void)
{
}
void EnemyRobot::UpdateAttackKick(void)
{
}
void EnemyRobot::UpdateAttackShoot(void)
{
}
void EnemyRobot::UpdateEscape(void)
{
}
void EnemyRobot::UpdateDead(void)
{
}
void EnemyRobot::UpdateKnockBack(void)
{
}
void EnemyRobot::UpdateEnd(void)
{
}


void EnemyRobot::Draw(void)
{
	// 基底クラスの描画処理
	CharaBase::Draw();

	// 視野(円錐)の描画
	SetUseLighting(FALSE);
	MV1DrawModel(viewRangeTransform_.modelId);
	const float alphaRate = 0.5f;
	MV1SetOpacityRate(viewRangeTransform_.modelId, alphaRate);
	SetUseLighting(TRUE);

	if (sceneMng_.GetIsDebugMode())
	{
		// 巡回ルート描画
		for (const auto& point : wayPoints_)
		{
			//DrawSphere3D(point, 50.0f, 10, 0x0000ff, 0x0000ff, false);
		}
	}
}