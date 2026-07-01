#include "SceneGame.h"
#define NOMINMAX
#include <algorithm>
#include <math.h>
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Utility/UtilityMath.h"
#include "../../Application.h"
#include "SceneTitle.h"
#include "SceneResult.h"


// ゲーム時間
constexpr float GAME_TIME = 500.0f;
constexpr float GAME_TIME_DEFEAT_DEC = 75.0f;

SceneGame::SceneGame(void)
	: players_()
	, boss_(std::make_unique<Boss>())
	, stage_(std::make_unique<Stage>())
	, damageController_(std::make_unique<DamageController>())
	, gameTime_(GAME_TIME)
	, targetHpImage_(-1), targetHpBerImage_(-1)
{
	std::unique_ptr<Player> player = std::make_unique<Player>(0, Player::BULLET_TYPE::BIG, UtilityMath::VECTOR_ZERO);
	players_.emplace_back(std::move(player));

	targetHpBerImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 0);
	targetHpImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 1);
}


void SceneGame::Load(void)
{
	SceneBase::Load();


	for (auto& player : players_)
	{
		player->Load();
	}

	boss_->Load();

	stage_->Load();

	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_GAME, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_GAME));

	//時間カウントリセット
	TimeManager::GetInstance().Reset();
}

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{

	if (Loading::GetInstance()->IsLoading()) { return; }

	// マウスを表示しない設定にする
	SetMouseDispFlag(FALSE);
	
	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::FOLLOW);
	camera->SetFollow(&players_.at(0)->GetTransform());

	for (auto& player : players_)
	{
		player->Init();
	}

	boss_->Init();
	stage_->Init();
	damageController_->SetPlayerMaxHp(players_.at(0)->GetMaxHp());
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_GAME);
}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& camera = SceneManager::GetInstance().GetCamera();
	auto loader = Loading::GetInstance();

	if (loader->IsLoading()) { return; }

	SceneManager::GetInstance().GetCamera()->Update();

	players_.at(0)->Update();
	boss_->Update();
	stage_->Update();
	damageController_->Update();

	players_.at(0)->SetSoundData(boss_->GetBossPos(), boss_->GetSoundRadius(), boss_->GetLandingFlag(),boss_->GetMGFireFlag(),boss_->GetRoadFlag());

	DamageProcess();

	UpdateGameTime();

	// ボスHPが０の時、ゲームクリア
	if (boss_->GetHP() <= 0 && gameTime_ > 0.0f)
	{
		SoundManager::GetInstance().Stop(SoundManager::SOUND::BGM_GAME);
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(false));
	}
}

void SceneGame::DamageProcess(void)
{
	boss_->SetPlayer1Pos(players_.at(0)->GetBodyPos());
	
	boss_->SetBossDamage(damageController_->GetBossDamage());

	boss_->SetWeaponCannonLDamage(damageController_->GetWeaponCannonLDamage());
	boss_->SetWeaponCannonRDamage(damageController_->GetWeaponCannonRDamage());

	boss_->SetWeaponMGLDamage(damageController_->GetWeaponMGLDamage());
	boss_->SetWeaponMGRDamage(damageController_->GetWeaponMGRDamage());

	boss_->SetWeaponMPLDamage(damageController_->GetWeaponMPLDamage());
	boss_->SetWeaponMPRDamage(damageController_->GetWeaponMPRDamage());

	boss_->SetWeaponRGDamage(damageController_->GetWeaponRGDamage());

	// プレイヤーの攻撃
	damageController_->SetPlayerAttack(players_.at(0)->GetPower());

	// プレイヤー被ダメージ処理
	players_.at(0)->SetDamage(damageController_->GetPlayerDamage(), damageController_->GetInvincible());
}

void SceneGame::CameraLockOn(void)
{
	

	

	
}

void SceneGame::UpdateGameTime(void)
{
	// 時間を取得
	float times = TimeManager::GetInstance().GetDeltaTime();

	gameTime_ -= times;

	for (auto& player : players_)
	{
		if (player->GetCurHp() <= 0)
		{
			gameTime_ -= GAME_TIME_DEFEAT_DEC;
			player->SetRespawn();
		}
	}

	if (gameTime_ <= 0.0f)
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(true));
	}
}


void SceneGame::Draw(void)
{
	stage_->Draw();

	players_.at(0)->Draw();

	boss_->Draw();

	DrawHpBer();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

	SceneManager::GetInstance().GetCamera()->DrawDebug();

	std::string textSecond = (((static_cast<int>(gameTime_) % 60) < 10) ? "0" : "");
	textSecond += std::to_string((static_cast<int>(gameTime_) % 60));

	std::string textMinute = (((static_cast<int>(gameTime_) / 60) < 10) ? "0" : "");
	textMinute += std::to_string((static_cast<int>(gameTime_) / 60));

	DrawFormatString(Application::SCREEN_HALF_X, 0, 0x000000, "撤退まであと %s:%s"
					, textMinute.c_str(), textSecond.c_str());
}

void SceneGame::Release(void)
{
	stage_->Release();
	
	players_.at(0)->Release();

	boss_->Release();
}

void SceneGame::DrawHpBer(void)
{
	const std::unique_ptr<Camera>& camera = SceneManager::GetInstance().GetCamera();

	if (camera->GetIsLockOn() && !camera->IsEasingState())
	{
		constexpr float    BER_SIZE = 0.5f;
		constexpr Vector2F BER_OFFSET = { 0.0f, -100.0f };

		Vector2 size = Vector2();
		VECTOR  viewPos = ConvWorldPosToScreenPos(camera->GetTargetPos());

		GetGraphSize(targetHpBerImage_, &size.x, &size.y);
		viewPos.x += BER_OFFSET.x;
		viewPos.y += BER_OFFSET.y;

		DrawRotaGraph(
			viewPos.x, viewPos.y,
			BER_SIZE, 0.0f,
			targetHpBerImage_, true
		);
		Vector2 gaugeSize = Vector2();
		GetGraphSize(targetHpImage_, &gaugeSize.x, &gaugeSize.y);

		const float hpRatio = (camera->GetLockOnMaxHp() > 0)
			? static_cast<float>(camera->GetLockOnHp()) / static_cast<float>(camera->GetLockOnMaxHp())
			: 0.0f;

		const float clampedRatio = (hpRatio < 0.0f)
				? 0.0f
				: (hpRatio > 1.0f)
					? 1.0f
					: hpRatio;

		// スクリーン上でのゲージ画像全体のサイズ（BER_SIZE倍）
		const float scaledW = gaugeSize.x * BER_SIZE;
		const float scaledH = gaugeSize.y * BER_SIZE;

		// 描画先: 左上座標（viewPos は中心なので半分引く）
		const int destLeft = static_cast<int>(viewPos.x - scaledW * 0.5f);
		const int destTop = static_cast<int>(viewPos.y - scaledH * 0.5f);

		// 描画先: 右下座標（左端固定 → 右端だけHP割合で削る）
		//   destRight = destLeft + scaledW × hpRatio
		const int destRight = destLeft + static_cast<int>(scaledW * clampedRatio);
		const int destBottom = destTop + static_cast<int>(scaledH);

		// 画像内: 切り取り幅（HP割合分）
		const int srcW = static_cast<int>(gaugeSize.x * clampedRatio);

		if (srcW > 0)
		{
			DrawRectExtendGraph(
				destLeft, destTop,
				destRight, destBottom,
				0, 0,
				srcW, gaugeSize.y,
				targetHpImage_,
				true
			);
		}

#ifdef _DEBUG
		DrawFormatString(10, 60, GetColor(255, 255, 255),
			"Boss HP: %d / %d  Ratio: %.2f",
			camera->GetLockOnHp(), camera->GetLockOnMaxHp(), clampedRatio);
#endif
	}
}
float SceneGame::CalcHpBarScale(const VECTOR& _targetPos)
{
	// 距離の最小値
	constexpr float DIST_MIN = 50.0f;

	// スケールの最小/最大
	constexpr float HP_BAR_MIN_SCALE = 0.375f;
	constexpr float HP_BAR_MAX_SCALE = 1.5f;

	const VECTOR cameraPos = SceneManager::GetInstance().GetCamera()->GetPos();
	VECTOR distVec = VSub(_targetPos, cameraPos);
	distVec.y = 0.0f;
	const float DIST = VSize(distVec);

	// ベクトルの距離が最小値より小さい場合は最小値にする
	const float SAFE_DIST = ((DIST < DIST_MIN) ? DIST_MIN : DIST);

	// 遠ければ遠いほどスケールを狭める
	const float SCALE = (DIST_MIN / SAFE_DIST);

	return std::clamp(SCALE, HP_BAR_MIN_SCALE, HP_BAR_MAX_SCALE);
}

void SceneGame::DrawDebug(void)
{
	SceneManager::GetInstance().GetCamera()->DrawDebug();
}

