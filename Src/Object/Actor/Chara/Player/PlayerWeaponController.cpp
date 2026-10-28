#include "PlayerWeaponController.h"
#include <algorithm>
#include <map>
#include "./PlayerBase.h"
#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include "../../../../Manager/System/TimeManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Camera/Camera.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Application.h"
#include "../../../Common/Transform.h"
#include "../Weapon/Bullet/Player/PBulletBig.h"
#include "../Weapon/Bullet/Player/PBulletBomb.h"
#include "../Weapon/Bullet/Player/PBulletRecovery.h"
#include "../Weapon/Bullet/Player/PBulletPoison.h"

namespace
{
	using JOB = PlayerBase::JOB_TYPE;
	using SHOT = PlayerWeaponController::SHOT_TYPE;

	// 巨大弾発射数
	constexpr int CANNON_BULLET_MAX = 3;

	// 通常弾の種類
	constexpr std::array<SHOT, static_cast<int>(JOB::MAX)>
		SHOT_TYPE_NORMAL = { SHOT::BOMB, SHOT::BIG, SHOT::RAPID_FIRE, SHOT::POISON };

	// 特殊弾の種類
	constexpr std::array<SHOT, static_cast<int>(JOB::MAX)>
		SHOT_TYPE_SPECIAL = { SHOT::NONE, SHOT::NONE, SHOT::CLUSTER, SHOT::RECOVERY };

	// 横発射速度
	constexpr std::array<float, static_cast<int>(SHOT::MAX)>
		SHOT_POW_XZ = { PBulletBomb::SHOT_SPEED_BOMB_XZ
			, PBulletBig::SHOT_SPEED_BIG_XZ
			, PlayerWeaponController::RAPID_SHOT_SPEED_XZ, PlayerWeaponController::CLUSTER_SHOT_SPEED
			, PBulletRecovery::SHOT_SPEED_RECOVERY_XZ, PBulletPoison::SHOT_SPEED_POISON_XZ };

	// 縦発射速度
	constexpr std::array<float, static_cast<int>(SHOT::MAX)>
		SHOT_POW_Y = { PBulletBomb::SHOT_SPEED_BOMB_Y
			, PBulletBig::SHOT_SPEED_BIG_Y
			, PlayerWeaponController::RAPID_SHOT_SPEED_Y, PlayerWeaponController::CLUSTER_SHOT_SPEED
			, PBulletRecovery::SHOT_SPEED_RECOVERY_Y, PBulletPoison::SHOT_SPEED_POISON_Y };

	// 軌道の球の分割数
	constexpr int ORBIT_SPHERE_DIV = 12;

	// 軌道のサイズ
	constexpr int ORBIT_START = 60;

	// 軌道の球の最小値
	constexpr float ORBIT_MIN = 0.1f;

	// 軌道の球の半径減少値
	constexpr float ORBIT_RADIUS_DOWN = 0.75f;

	// 軌道の球のサイズ
	constexpr float ORBIT_RADIUS = 0.125f;

	// 着弾地点の透明度
	constexpr float ORBIT_ALPHA = 0.5f;

	// 着弾範囲
	constexpr float ORBIT_SIZE = 85.0f;

	// 軌道の描画縦位置のオフセット
	constexpr float ORBIT_START_OFFSET_Y = 25.0f;

	// 軌道線の球を描く間隔(フレーム)
	constexpr int ORBIT_DRAW_INTERVAL = 10;

	// 軌道の色
	constexpr COLOR_U8 ORBIT_COLOR = { 150, 150, 150 };
};

PlayerWeaponController::PlayerWeaponController(int _jobType, const Transform& _owner)
	: owner_(_owner)
	, shotType_(SHOT::NONE)
	, IS_RAPID_FIRE(_jobType == static_cast<int>(JOB::RAPID_FIRE))
	, SHOT_NORMAL(SHOT_TYPE_NORMAL[static_cast<int>(_jobType)])
	, SHOT_SPECIAL(SHOT_TYPE_SPECIAL[static_cast<int>(_jobType)])
	, imageBulletHitPoint_(-1)
	, attackNumMax_(0)
	, curAttackCount_(0)
	, shotIndex_(-1)
	, throwPos_(UtilityMath::VECTOR_ZERO)
	, throwDir_(UtilityMath::VECTOR_ZERO)
	, shotPointVertex_{}
{
	// 巨大弾の場合は発射数を割り当てる
	attackNumMax_ = ((_jobType == static_cast<int>(JOB::CANNON)) ? CANNON_BULLET_MAX : 0);

	const VECTOR INIT_NORM = UtilityMath::DIR_UP;
	const COLOR_U8 INIT_DIFUSECOLOR = GetColorU8(255, 255, 255, 255);

	for (auto& vertex : shotPointVertex_)
	{
		vertex.norm = INIT_NORM;
		vertex.dif = INIT_DIFUSECOLOR;
	}

	// UV座標の割り当て
	shotPointVertex_[PlayerBase::LEFT_BACK].u     = 0.0f; shotPointVertex_[PlayerBase::LEFT_BACK].v     = 1.0f;
	shotPointVertex_[PlayerBase::LEFT_FORWARD].u  = 0.0f; shotPointVertex_[PlayerBase::LEFT_FORWARD].v  = 0.0f;
	shotPointVertex_[PlayerBase::RIGHT_BACK].u    = 1.0f; shotPointVertex_[PlayerBase::RIGHT_BACK].v    = 1.0f;
	shotPointVertex_[PlayerBase::RIGHT_FORWARD].u = 1.0f; shotPointVertex_[PlayerBase::RIGHT_FORWARD].v = 0.0f;
}

PlayerWeaponController::~PlayerWeaponController(void)
{
	// メモリ解放
	Release();
}

void PlayerWeaponController::Load(void)
{
	imageBulletHitPoint_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_FALLING_POINT);
}

void PlayerWeaponController::Update(int _skinType, bool _isAnimationStop)
{
	// 発射時の手のフレームに生成した弾を追従させる
	const PlayerBase::SKIN_TYPE skinType = static_cast<PlayerBase::SKIN_TYPE>(_skinType);
	const int FRAME_FINGER_LEFT = PlayerBase::FRAME_NUM_FINGER_LEFT.at(skinType);
	const int FRAME_FINGER_RIGHT = PlayerBase::FRAME_NUM_FINGER_RIGHT.at(skinType);
	const int FRAME_FINGER = ((curAttackCount_ % 2 == 0) ? FRAME_FINGER_LEFT : FRAME_FINGER_RIGHT);
	const int FRAME_HAND_PALM = (FRAME_FINGER - 1);

	const VECTOR posFinger = MV1GetFramePosition(owner_.modelId, FRAME_FINGER);
	const VECTOR posHandPalm = MV1GetFramePosition(owner_.modelId, FRAME_HAND_PALM);

	// 発射方向
	throwDir_ = UtilityMath::VNormalize(VSub(posFinger, posHandPalm));
	throwPos_ = posFinger;

	for (auto& bullet : bullets_)
	{
		if (bullet == nullptr) { continue; }
		bullet->Update();
	}

	if (IS_RAPID_FIRE)
	{
		for (auto& cluster : clusterBullets_)
		{
			if (cluster == nullptr) { continue; }

			cluster->Update();

			if (cluster->GetBulletState() == PBulletBase::BULLET_STATE::INACTIVE)
			{
				// 発射前に弾を追従させる
				cluster->SetFollow(throwPos_, throwDir_);
			}
		}
	}

	if (bullets_.empty()) { return; }

	if (shotIndex_ != -1)
	{
		if (_isAnimationStop)
		{
			// アニメーション停止中に発射前処理を行う
			bullets_.at(shotIndex_)->PreActiveProcess();
		}

		// 弾を手に追従
		bullets_.at(shotIndex_)->SetFollow(throwPos_, throwDir_);
	}
}

void PlayerWeaponController::Draw(void)
{
	for (auto& bullet : bullets_)
	{
		// 弾の描画
		bullet->Draw();
	}

	if (IS_RAPID_FIRE)
	{
		// 連射弾の描画
		for (auto& cluster : clusterBullets_)
		{
			if (cluster == nullptr) { continue; }
			cluster->Draw();
		}
	}
}

void PlayerWeaponController::Release(void)
{
	for (auto& bullet : bullets_)
	{
		// 弾の解放
		if (bullet == nullptr) { continue; }
		bullet->Release();
	}

	for (auto& cluster : clusterBullets_)
	{
		// 拡散弾の解放
		if (cluster == nullptr) { continue; }
		cluster->Release();
	}
}

void PlayerWeaponController::CreateBullet(bool _isShotSpecial)
{
	std::unique_ptr<PBulletBase> bulletObj;

	// 特殊弾か通常弾かで弾の種類を割り当てる
	shotType_ = ((_isShotSpecial) ? SHOT_SPECIAL : SHOT_NORMAL);

	// 種類が無効時は処理終了
	if (shotType_ == SHOT::NONE) { return; }

	curAttackCount_++;
	shotIndex_ = 0;

	// 弾の再利用処理
	for (auto& pooledBullet : bullets_)
	{
		// 拡散弾では再利用生成を行わない
		if (shotType_ == SHOT::CLUSTER) { break; }

		if (!pooledBullet->IsAlive()
			&& static_cast<int>(shotType_) == pooledBullet->GetShotType())
		{
			// 無効中の弾を再利用
			pooledBullet->Init();
			pooledBullet->Create(owner_.pos, throwDir_, curAttackCount_);
			return;
		}

		// 発射する弾の識別番号割り当て
		shotIndex_++;
	}

	// 弾丸割り当て
	const int type = static_cast<int>(shotType_);
	switch (shotType_)
	{
		// 巨大弾
		case SHOT::BIG:
			bulletObj = std::make_unique<PBulletBig>(type);
			break;

		// 爆発弾
		case SHOT::BOMB:
			bulletObj = std::make_unique<PBulletBomb>(type);
			break;

		// 回復弾
		case SHOT::RECOVERY:
			bulletObj = std::make_unique<PBulletRecovery>(type);
			break;

		// 毒弾
		case SHOT::POISON:
			bulletObj = std::make_unique<PBulletPoison>(type);
			break;

		// 連射弾
		case SHOT::RAPID_FIRE:
			bulletObj = std::make_unique<PBulletNormal>(RAPID_SCALE, RAPID_RADIUS, RAPID_POWER
				, RAPID_SHOT_SPEED_XZ, RAPID_SHOT_SPEED_Y, RAPID_ALIVE_TIME, type, false);
			break;

		// 拡散弾
		case SHOT::CLUSTER:
			CreateCluster();
			return;
			break;

		default:
			return;
			break;
	}

	// 弾再生成処理
	bulletObj->Load();
	bulletObj->Init();
	bulletObj->Create(throwPos_, throwDir_, curAttackCount_);
	bullets_.emplace_back(std::move(bulletObj));
}

void PlayerWeaponController::CreateCluster(void)
{
	// 拡散する角度の最大値
	constexpr float CONE_ANGLE_MAX = (360.0f / CLUSTER_SPLIT);

	// 正面の方向(ゼロベクトル時はZ+方向を正面にする)
	const VECTOR FORWARD_DIR = (UtilityMath::EqualsVZero(owner_.GetForward())
		? VGet(0.0f, 0.0f, 1.0f) : owner_.GetForward());

	int cnt = 0;

	// 初回生成か否か
	const bool IS_FIRST_CREATE = (clusterBullets_.at(0) == nullptr);

	// 生成位置
	VECTOR createPos = throwPos_;
	createPos.y += 10.0f;

	// 前方向の基準軸
	VECTOR rightAxis = UtilityMath::VNormalize(VCross(UtilityMath::AXIS_Y, FORWARD_DIR));
	if (UtilityMath::EqualsVZero(rightAxis))
	{
		rightAxis = UtilityMath::AXIS_X;
	}

	// 中心の弾を生成
	if (IS_FIRST_CREATE)
	{
		std::unique_ptr<PBulletNormal> centerBullet = CreateClusterBullet();
		centerBullet->Load();
		centerBullet->Init();
		centerBullet->Create(createPos, FORWARD_DIR);
		clusterBullets_.at(cnt++) = std::move(centerBullet);
	}
	else
	{
		// 再利用
		clusterBullets_.at(cnt)->Init();
		clusterBullets_.at(cnt)->Create(createPos, FORWARD_DIR);
		cnt++;
	}

	// 拡散する弾の輪の最大数
	const int RING_NUM = (CLUSTER_NUM_MAX / CLUSTER_SPLIT);

	for (int ring = 1; ring < RING_NUM; ring++)
	{
		const float CONE_ANGLE = (CONE_ANGLE_MAX / static_cast<float>(RING_NUM - 1) * static_cast<float>(ring));
		const Quaternion CONE_ROT = Quaternion::AngleAxis(UtilityMath::Deg2RadF(CONE_ANGLE), rightAxis);
		const VECTOR TILED_DIR = Quaternion::PosAxis(CONE_ROT, FORWARD_DIR);
		const float AZIMUTH_STEP = (360.0f / static_cast<float>(CLUSTER_SPLIT));

		for (int circle = 1; circle < CLUSTER_SPLIT; circle++)
		{
			// 角度
			float azimuth = AZIMUTH_STEP * static_cast<float>(circle);
			const Quaternion azimuthRot = Quaternion::AngleAxis(UtilityMath::Deg2RadF(azimuth), FORWARD_DIR);

			// 放射状に発射するように方向を調整
			VECTOR shotDir = UtilityMath::VNormalize(Quaternion::PosAxis(azimuthRot, TILED_DIR));


			// 各拡散弾を生成
			if (IS_FIRST_CREATE)
			{
				std::unique_ptr bullet = CreateClusterBullet();

				bullet->Load();
				bullet->Init();
				bullet->Create(createPos, shotDir);

				clusterBullets_.at(cnt++) = std::move(bullet);
			}
			else
			{
				// 再利用
				clusterBullets_.at(cnt)->Init();
				clusterBullets_.at(cnt)->Create(createPos, shotDir);
				cnt++;
			}
		}

		// 生成数が一定を超えたら終了
		if (cnt > CLUSTER_NUM_MAX) { break; }
	}

	// 発射番号を無効化
	shotIndex_ = -1;
}

std::unique_ptr<PBulletNormal> PlayerWeaponController::CreateClusterBullet(void)
{
	return std::make_unique<PBulletNormal>(CLUSTER_SCALE, CLUSTER_RADIUS, CLUSTER_POWER
		, CLUSTER_SHOT_SPEED, 0.0f, CLUSTER_ALIVE_TIME
		, static_cast<int>(SHOT::CLUSTER), false);
}

void PlayerWeaponController::ShotBullet(void)
{
	if (shotIndex_ < 0 || shotIndex_ >= static_cast<int>(bullets_.size())) { return; }

	// 発射処理の有効化
	bullets_[shotIndex_]->Shot(CalcShotDirection());
	shotIndex_ = -1;
}

void PlayerWeaponController::ShotCluster(void)
{
	// 発射処理の有効化
	for (auto& bullet : clusterBullets_)
	{
		if (bullet == nullptr) { continue; }
		bullet->Shot();
	}
	shotIndex_ = -1;
}

VECTOR PlayerWeaponController::CalcShotDirection(void)
{
	VECTOR shotDir = owner_.GetForward();
	auto& camera = SceneManager::GetInstance().GetCamera();

	if (!IS_RAPID_FIRE)
	{
		// 放物線状に投げる
		shotDir = UtilityMath::VNormalize(VAdd(shotDir, UtilityMath::DIR_UP));
	}
	else if (camera->GetIsLockOn())
	{
		// 発射位置(手)からロックオン位置へ直進させる
		const VECTOR toTarget = VSub(camera->GetLockOnPos(), throwPos_);

		if (!UtilityMath::EqualsVZero(toTarget))
		{
			// 発射位置とロックオン位置がほぼ重なっている場合は正面方向のまま
			shotDir = UtilityMath::VNormalize(toTarget);
		}
	}

	return shotDir;
}

void PlayerWeaponController::DrawShotOrbit(void)
{
	if (shotType_ == SHOT::NONE) { return; }

	const unsigned int color = GetColor(ORBIT_COLOR.r, ORBIT_COLOR.g, ORBIT_COLOR.b);

	// 重力の有無(PBulletBase::IS_GRAVITY と同じ条件)
	const bool IS_GRAVITY = (shotType_ != SHOT::RAPID_FIRE && shotType_ != SHOT::CLUSTER);

	// 移動方向
	const float POWER_XZ = SHOT_POW_XZ[static_cast<int>(shotType_)];
	const float POWER_Y = SHOT_POW_Y[static_cast<int>(shotType_)];
	const VECTOR SHOT_DIRECTION = UtilityMath::VNormalize(CalcShotDirection());

	// 1フレームの移動量(PBulletBase::Shot と同じ式)
	// 重力なし: 全軸に平面速度 / 重力あり: Y だけ縦速度
	VECTOR velocity = VScale(SHOT_DIRECTION, POWER_XZ);
	if (IS_GRAVITY)
	{
		velocity.y = (SHOT_DIRECTION.y * POWER_Y);
	}

	// 1フレームあたりの重力加算値(PBulletBase::UpdateShot と同じ式)
	const float gravityStep = (Application::GetInstance().GetGravityPow() * (1.0f / 60.0f));

	VECTOR pos = VAdd(owner_.pos, VGet(0.0f, ORBIT_START_OFFSET_Y, 0.0f));
	float curGravity = 0.0f;
	float radius = (ORBIT_RADIUS * ORBIT_START);

	// 実際の弾と同じく、1フレームずつ進める
	const int FRAME_MAX = ((ORBIT_START - 1) * ORBIT_DRAW_INTERVAL);
	for (int frame = 1; frame <= FRAME_MAX; ++frame)
	{
		VECTOR step = velocity;

		if (IS_GRAVITY)
		{
			curGravity += gravityStep;
			step.y -= curGravity;
		}

		const VECTOR prevPos = pos;
		pos = VAdd(pos, step);

		if (pos.y < 0.0f)
		{
			// 地面を跨いだフレームの内分点を着弾位置にする
			const float denom = (prevPos.y - pos.y);
			const float rate = ((denom > 0.0f) ? std::clamp(prevPos.y / denom, 0.0f, 1.0f) : 1.0f);

			DrawShotOrbitPoint(UtilityMath::Lerp(prevPos, pos, rate));
			break;
		}

		if (frame % ORBIT_DRAW_INTERVAL != 0) { continue; }

		DrawSphere3D(pos, radius, ORBIT_SPHERE_DIV, color, color, true);

		// 軌道の球の半径縮小
		radius = (((radius - ORBIT_RADIUS_DOWN) > ORBIT_MIN)
			? (radius - ORBIT_RADIUS_DOWN) : ORBIT_MIN);
	}
}

void PlayerWeaponController::DrawShotOrbitPoint(const VECTOR& _shotPos)
{
	// 範囲の透明度
	constexpr int POINT_ALPHA = static_cast<int>(255 * ORBIT_ALPHA);

	// 範囲の位置
	shotPointVertex_[PlayerBase::LEFT_BACK].pos
		= VGet(_shotPos.x - ORBIT_SIZE
			, _shotPos.y
			, _shotPos.z - ORBIT_SIZE);

	shotPointVertex_[PlayerBase::LEFT_FORWARD].pos
		= VGet(_shotPos.x - ORBIT_SIZE
			, _shotPos.y
			, _shotPos.z + ORBIT_SIZE);

	shotPointVertex_[PlayerBase::RIGHT_BACK].pos
		= VGet(_shotPos.x + ORBIT_SIZE
			, _shotPos.y
			, _shotPos.z - ORBIT_SIZE);

	shotPointVertex_[PlayerBase::RIGHT_FORWARD].pos
		= VGet(_shotPos.x + ORBIT_SIZE
			, _shotPos.y
			, _shotPos.z + ORBIT_SIZE);

	for (auto& vertex : shotPointVertex_)
	{
		// アルファ値を各頂点に適用
		vertex.dif.a = POINT_ALPHA;
	}

	// 描画環境のセットアップ
	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(FALSE);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, POINT_ALPHA);

	// インデックス配列の定義
	constexpr int TRIANGLE_CNT = 2;
	constexpr int POINT_CNT = 6;

	WORD index[POINT_CNT]
		= { PlayerBase::LEFT_BACK, PlayerBase::LEFT_FORWARD, PlayerBase::RIGHT_BACK
		  , PlayerBase::RIGHT_FORWARD, PlayerBase::RIGHT_BACK, PlayerBase::LEFT_FORWARD };

	// 着弾地点の描画
	DrawPolygonIndexed3D(shotPointVertex_.data(), static_cast<int>(shotPointVertex_.size())
		, index, TRIANGLE_CNT, imageBulletHitPoint_, TRUE);

	// グラフィック設定の復元
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}

bool PlayerWeaponController::IsClusterAlive(void) const
{
	return (clusterBullets_.at(0) != nullptr && clusterBullets_.at(0)->IsAlive());
}