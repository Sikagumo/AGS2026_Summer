#define NOMINMAX
#include "SceneGame.h"
#include <algorithm>
#include <math.h>
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Utility/UtilityMath.h"
#include "../../Application.h"
#include "../../Manager/Decoration/EffectManager.h"
#include "../../Shader/ShaderController.h"
#include "SceneTitle.h"
#include "SceneResult.h"
#include "../../Net/NetStructures.h"
#include "../../Manager/System/NetManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../SubScene/ScenePause.h"

SceneGame::SceneGame(std::vector<PlayerSelectType> _playerSelectType)
	: players_()
	, boss_(std::make_unique<Boss>())
	, enemyRobots_()
	, gameTexts_()
	, stage_(std::make_unique<Stage>())
	, damageController_(std::make_unique<DamageController>())
	, targetHpImage_(-1)
	, targetHpBerImage_(-1)
	, gameTimer_(nullptr)
	, slowCount_(0)
	, infoImage_(-1)
{
	for (int i = 0; i < _playerSelectType.size(); i++)
	{
		auto job = _playerSelectType.at(i).job;
		auto skin = _playerSelectType.at(i).skin;
		std::unique_ptr<Player> player
			= std::make_unique<Player>(i, job, skin
									  , PLAYER_INIT_POS[i]);

		players_.emplace_back(std::move(player));
		
	}
	
	for (int i = 0; i < ENEMYS_POP; i++)
	{
		std::unique_ptr<EnemyRobo> enemy = std::make_unique<EnemyRobo>(ENEMY_POS[i]);

		enemyRobots_.emplace_back(std::move(enemy));
	}

	boss_->SetPlayerSize(static_cast<int>(_playerSelectType.size()));
}

void SceneGame::Load(void)
{
	SceneBase::Load();

	playerHpImageBack_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_PLAYER, 0);
	playerHpImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_PLAYER, 1);

	targetHpBerImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 0);
	targetHpImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 1);

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_GAME_TEXT, uiGame_.data());


	for (auto& player : players_)
	{
		player->Load();
	}

	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->Load();
	}
	
	playerHpImageBack_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_PLAYER, 0);
	playerHpImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_PLAYER, 1);

	targetHpBerImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 0);
	targetHpImage_ = ResourceManager::GetInstance().LoadHandleIdsOnce(ResourceManager::SRC::IMGS_HP_TARGET, 1);

	infoImage_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_INFO);

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_GAME_TEXT, uiGame_.data());

	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->Load();
	}

	boss_->Load();

	stage_->Load();

	gameTimer_ = std::make_unique<GameTimer>(GAME_TIME);

	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_GAME
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_GAME));

	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_THUNDER
			, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_THUNDER));

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_HIT_BLAST
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_HIT_BLAST));

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
	SetMouseDispFlag(false);
	
	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::PLAYER_FOLLOW);
	camera->SetFollow(&players_.at(0)->GetTransform());

	NET_JOIN_USER selfUser = NetManager::GetInstance().GetSelfUser();
	auto remoteUsers = NetManager::GetInstance().GetNetUsers();
	bool isBossLocalControl = (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT);
	boss_->SetHostControl(isBossLocalControl);

	// 自分の設定
	players_.at(0)->SetHostControl(true);
	players_.at(0)->SetNetKey(selfUser.key);

	// 他人の設定
	int idx = 1;
	for (const auto& pair : remoteUsers)
	{
		if (idx >= players_.size())
		{
			break;
		}

		// ラジコンにする
		players_.at(idx)->SetHostControl(false); 
		players_.at(idx)->SetNetKey(pair.second.key);
		idx++;
	}
	
	for (auto& player : players_)
	{
		player->Init();
	}
	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->Init();
	}
	boss_->Init();
	stage_->Init();

	// タイマー有効化
	gameTimer_->SetIsTimeActive(true);

	damageController_->SetPlayerMaxHp(players_.at(0)->GetMaxHp());
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_GAME);
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);
	
	
	// 雨シェーダ導入
	constexpr float RAIN_POW = 1.0f;
	constexpr float RAIN_POW_BACK = 1.0f;
	rainyMaterial_.SetUseRainy(RAIN_POW, RAIN_POW_BACK);

	SetMouseDispFlag(true);

	for (size_t i = 0; i < imageResult_.size(); ++i)
	{
		imageResult_[i] = -1;
	}

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_RESULT, imageResult_.data());

	
	stateChanges_.emplace(static_cast<int>(GAME_STATE::GAME), std::bind(&SceneGame::ChangeGame, this));
	stateChanges_.emplace(static_cast<int>(GAME_STATE::GAME_END), std::bind(&SceneGame::ChangeGameEnd, this));
	
	ChangeState(GAME_STATE::GAME);
}

void SceneGame::Update(void)
{
	if (KeyConfInputManager::GetInstance().isTrigerDown("PAUSE"))
	{
		SceneManager::GetInstance().PushScene(std::make_shared<ScenePause>());
	}

	if (NetManager::GetInstance().GetMode() != NET_MODE::NONE
		&& NetManager::GetInstance().GetIsConnectionLost())
	{
		NetManager::GetInstance().Stop();
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneTitle>());
		return;
	}

	stateUpdate_();
}

void SceneGame::DamageProcess(void)
{
	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->SetPlayerPos(players_.at(0)->GetBodyPos());
	}

	std::vector<std::pair<int, VECTOR>> keyPosList;
	for (auto& player : players_)
	{
		keyPosList.emplace_back(player->GetNetKey(), player->GetBodyPos());
	}

	std::sort(keyPosList.begin(), keyPosList.end(), [](const auto& a, const auto& b)
		{
			return a.first < b.first; 
		});

	const int playerCount = static_cast<int>(keyPosList.size());

	if (playerCount >= 1)
	{
		boss_->SetPlayer1Pos(keyPosList[0].second);
	}

	if (playerCount >= 2)
	{
		boss_->SetPlayer2Pos(keyPosList[1].second);
	}

	if (playerCount >= 3)
	{
		boss_->SetPlayer3Pos(keyPosList[2].second);
	}

	if (playerCount >= 4)
	{
		boss_->SetPlayer4Pos(keyPosList[3].second);
	}
	
	if (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT)
	{
		// ボスへのダメージ適用をホストのみに限定する
		boss_->SetBossDamage(damageController_->GetBossDamage());
		boss_->SetWeaponCannonLDamage(damageController_->GetWeaponCannonLDamage());
		boss_->SetWeaponCannonRDamage(damageController_->GetWeaponCannonRDamage());
		boss_->SetWeaponMGLDamage(damageController_->GetWeaponMGLDamage());
		boss_->SetWeaponMGRDamage(damageController_->GetWeaponMGRDamage());
		boss_->SetWeaponMPLDamage(damageController_->GetWeaponMPLDamage());
		boss_->SetWeaponMPRDamage(damageController_->GetWeaponMPRDamage());
		boss_->SetWeaponRGDamage(damageController_->GetWeaponRGDamage());
	}


	if (damageController_->GetBossDamage() > 0
		|| damageController_->GetWeaponCannonLDamage() > 0
		|| damageController_->GetWeaponCannonRDamage() > 0
		|| damageController_->GetWeaponMGLDamage() > 0
		|| damageController_->GetWeaponMGRDamage() > 0
		|| damageController_->GetWeaponMPLDamage() > 0
		|| damageController_->GetWeaponMPRDamage() > 0
		|| damageController_->GetWeaponRGDamage() > 0)
	{
		SoundManager::GetInstance().Play(SoundManager::SOUND::SE_HIT_BLAST);
	}

	// プレイヤー被ダメージ処理
	for (auto& player : players_)
	{
		if (!player->GetHostControl()) { continue; }

		player->SetDamage(damageController_->GetPlayerDamage()
						  , damageController_->GetInvincible());
	}
}

void SceneGame::UpdateGameTime(void)
{
	if (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT)
	{
		gameTimer_->Update();

		for (auto& player : players_)
		{
			if (!player->GetIsRespawn()) { continue; }
			gameTimer_->SetTime(gameTimer_->GetTime() - GAME_TIME_DEFEAT_DEC);
		}

		NetManager::GetInstance().SetGameTime(gameTimer_->GetTime());
	}
	else
	{
		gameTimer_->SetTime(NetManager::GetInstance().GetGameTime());
	}

	if (gameTimer_->GetTime() <= 0.0f)
	{
		ChangeState(GAME_STATE::GAME_END);
	}

	rainyMaterial_.SetTime(TimeManager::GetInstance().GetGameTime());
}

void SceneGame::Draw(void)
{
	stateDraw_();
}

void SceneGame::Release(void)
{
	stage_->Release();
	
	for (auto& player : players_)
	{
		player->Release();
	}

	boss_->Release();
}

void SceneGame::DrawHpBerPlayer(void)
{
	constexpr float BER_SCALE = 0.275f;

	Vector2 backSize = Vector2();
	GetGraphSize(playerHpImageBack_, &backSize.x, &backSize.y);
	backSize *= BER_SCALE;

	Vector2 imageSize = Vector2();
	GetGraphSize(playerHpImage_, &imageSize.x, &imageSize.y);

	// 中央位置
	constexpr Vector2 BER_POS_MIDDLE = { 200, 500 };
	Vector2 berPos = BER_POS_MIDDLE;

	// 表示幅のリマップ範囲（数値で調整可能）
	constexpr float DISPLAY_RATIO_MIN = 0.05f;
	constexpr float DISPLAY_RATIO_MAX = 0.815f;

	const int PLAYER_NUM = static_cast<int>(players_.size() + 1);
	for (int i = 1; i < PLAYER_NUM; i++)
	{
		// バー背景描画
		DrawRotaGraph(
			berPos.x, berPos.y,
			BER_SCALE, 0.0f,
			playerHpImageBack_, true
		);

		const float hpRatio
			= (static_cast<float>(players_.at(i - 1)->GetCurHp()) / static_cast<float>(players_.at(i - 1)->GetMaxHp()));
		const float RATIO = std::clamp(hpRatio, 0.0f, 1.0f);


		// 減少範囲にリマップ
		const float DISPLAY_RATIO = std::lerp(DISPLAY_RATIO_MIN, DISPLAY_RATIO_MAX, RATIO);

		constexpr int HP_POS_X = 48;
		berPos.x += HP_POS_X;

		// 左上座標
		const Vector2 POS_UPPER_LEFT
			= { static_cast<int>(berPos.x - (backSize.x / 2)),
				static_cast<int>(berPos.y - (backSize.y / 2)) };

		// 右下座標
		const Vector2 POS_LOWER_RIGHT
			= { (POS_UPPER_LEFT.x + static_cast<int>(backSize.x * DISPLAY_RATIO)),
				(POS_UPPER_LEFT.y + backSize.y) };


		// 切り取り幅
		const int IMAGE_WIDTH = static_cast<int>(imageSize.x * DISPLAY_RATIO);

		if (IMAGE_WIDTH > 0)
		{
			// UV描画位置
			const Vector2 HP_UV_POS = { 175, 0 };
			DrawRectExtendGraph(
				POS_UPPER_LEFT.x , POS_UPPER_LEFT.y,
				POS_LOWER_RIGHT.x, POS_LOWER_RIGHT.y,
				HP_UV_POS.x, HP_UV_POS.y,
				IMAGE_WIDTH, imageSize.y,
				playerHpImage_,
				true
			);
		}

		constexpr int BER_OFFSET_Y = 100;
		berPos.y += (backSize.y + BER_OFFSET_Y);

#ifdef _DEBUG
		DrawFormatString(10, Application::SCREEN_SIZE_Y - (16 * (PLAYER_NUM - i)), 0xff0000, "プレイヤーHP：%d"
			, players_.at(i - 1)->GetCurHp());
#endif
	}
}
void SceneGame::DrawHpBerBoss(void)
{
	const std::unique_ptr<Camera>& camera = SceneManager::GetInstance().GetCamera();
	constexpr float    BER_SCALE = 0.5f;
	constexpr Vector2F BER_OFFSET = { 0.0f, -100.0f };

	// 表示幅としてのRATIOのリマップ範囲（数値で調整可能）
	constexpr float DISPLAY_RATIO_MIN = 0.3f;
	constexpr float DISPLAY_RATIO_MAX = 0.9f;

	Vector2 backSize = Vector2();
	GetGraphSize(targetHpBerImage_, &backSize.x, &backSize.y);

	if (camera->GetIsLockOn() && !camera->IsEasingState())
	{
		VECTOR  viewPos = ConvWorldPosToScreenPos(camera->GetTargetPos());
		viewPos.x += BER_OFFSET.x;
		viewPos.y += BER_OFFSET.y;

		// 背景
		DrawRotaGraph(
			static_cast<int>(viewPos.x), static_cast<int>(viewPos.y),
			BER_SCALE, 0.0f,
			targetHpBerImage_, true
		);


		Vector2 imageSize = Vector2();
		GetGraphSize(targetHpImage_, &imageSize.x, &imageSize.y);


		// スクリーン上でのゲージ画像全体のサイズ（BER_SIZE倍）
		const Vector2F BER_SIZE = { (imageSize.x * BER_SCALE), (imageSize.y * BER_SCALE) };

		// 追従先のHPが残っている場合に割合を計算
		const float hpRatio = (camera->GetLockOnMaxHp() > 0)
			? static_cast<float>(camera->GetLockOnHp()) / static_cast<float>(camera->GetLockOnMaxHp())
			: 0.0f;

		const float RATIO = std::clamp(hpRatio, 0.0f, 1.0f);

		// HP割合を範囲にリマップ
		const float DISPLAY_RATIO = std::lerp(DISPLAY_RATIO_MIN, DISPLAY_RATIO_MAX, RATIO);


		// スクリーン上でのゲージ画像全体のサイズ
		const float scaledW = (imageSize.x * BER_SIZE.x);
		const float scaledH = (imageSize.y * BER_SIZE.y);

		// 左上座標
		const Vector2 POS_UPPER_LEFT
			= { static_cast<int>(viewPos.x - (BER_SIZE.x / 2)),
				static_cast<int>(viewPos.y - (BER_SIZE.y / 2)) };

		// 右下座標
		constexpr int OFFSET_X = -10;
		const Vector2 POS_LOWER_RIGHT
			= { (POS_UPPER_LEFT.x + static_cast<int>(BER_SIZE.x * RATIO) + OFFSET_X),
				(POS_UPPER_LEFT.y + static_cast<int>(BER_SIZE.y)) };

		// 切り取り幅 (HP割合分)
		const int IMAGE_WIDTH = static_cast<int>(imageSize.x * RATIO);

		if (IMAGE_WIDTH > 0)
		{
			DrawRectExtendGraph(
				POS_UPPER_LEFT.x, POS_UPPER_LEFT.y,
				POS_LOWER_RIGHT.x, POS_LOWER_RIGHT.y,
				0, 0,
				IMAGE_WIDTH, imageSize.y,
				targetHpImage_,
				true
			);
		}

#ifdef _DEBUG
		DrawFormatString(10, 60, GetColor(255, 255, 255),
			"Boss HP: %d / %d  Ratio: %.2f",
			camera->GetLockOnHp(), camera->GetLockOnMaxHp(), RATIO);
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
	damageController_->DebugDraw();

	bool isHit = CollisionController::GetInstance().IsTagCollidingWithTag(ColliderBase::TAG::CAMERA, ColliderBase::TAG::WALL);

	if (isHit)
	{
		DrawString(0, 600, "当たってる", 0x000000);
	}
}

void SceneGame::UpdateGui(void)
{
}

void SceneGame::ChangeState(GAME_STATE _state)
{
	state_ = _state;

	int state = static_cast<int>(state_);

	// 各状態遷移の初期処理
	ChangeState(state);
}

void SceneGame::ChangeState(int state)
{
	stateBase_ = state;
	// 各状態遷移の初期処理
	stateChanges_[stateBase_]();
}

void SceneGame::ChangeGame(void)
{
	stateUpdate_ = std::bind(&SceneGame::UpdateGame, this);
	stateDraw_ = std::bind(&SceneGame::DrawGame, this);

	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::PLAYER_FOLLOW);
	camera->SetFollow(&players_.at(0)->GetTransform());


}

void SceneGame::ChangeGameEnd(void)
{
	stateUpdate_ = std::bind(&SceneGame::UpdateGameEnd, this);
	stateDraw_ = std::bind(&SceneGame::DrawGameEnd, this);

	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::BOSS_FOLLOW);
	camera->SetFollow(&boss_->GetBodyTransform());
	


}

void SceneGame::UpdateGame(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& camera = SceneManager::GetInstance().GetCamera();

	if (Loading::GetInstance()->IsLoading()) { return; }

	damageController_->Update();
	DamageProcess();

	if (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT)
	{
		boss_->Update();
		NetManager::GetInstance().SetBossAction(boss_->GetNetworkAction());
	}
	else
	{
		boss_->SetNetworkAction(NetManager::GetInstance().GetBossAction());
		boss_->Update();
	}

	stage_->Update();

	auto remoteHisMap = NetManager::GetInstance().GetRemoteActionHis();

	for (auto& player : players_)
	{
		if (player->GetHostControl()) { continue; }

		int key = player->GetNetKey();

		if (remoteHisMap.find(key) != remoteHisMap.end())
		{
			NET_ACTION latestAction = remoteHisMap[key].actions[0];

			player->SetNetworkAction(latestAction.pos, latestAction.quaRot, latestAction.animId, latestAction.isAttack, latestAction.currentHp);
		}
	}


	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->Update();
	}
	for (auto& player : players_)
	{
		player->Update();
		player->SetSoundData(boss_->GetBossPos(), boss_->GetSoundRadius(), boss_->GetLandingFlag(), boss_->GetMGFireFlag(), boss_->GetRoadFlag());
	}

	// プレイヤーの攻撃
	int attackPower = 0;
	int attackBlast = 0;
	for (auto& player : players_)
	{
		// 弾
		for (auto& bullet : player->GetBullets())
		{
			attackPower = std::max(attackPower, bullet->GetPowerBullet());
			attackBlast = std::max(attackBlast, bullet->GetPowerBlast());
		}

		// 拡散弾
		for (auto& bullet : player->GetBulletsCluster())
		{
			if (bullet == nullptr) { continue; }
			attackPower = std::max(attackPower, bullet->GetPowerBullet());
			attackBlast = std::max(attackBlast, bullet->GetPowerBlast());
			if (bullet->GetPowerBullet() > 0)
			{
				int temp = 0;
			}
		}
	}
	damageController_->SetPlayerAttack(attackPower, attackBlast);

	UpdateGameTime();

	EffectManager::GetInstance().Update();

	// ボスHPが０の時、ゲームクリア
	if (boss_->GetHP() <= 0 || gameTimer_->GetTime() <= 0.0f)
	{
		//SoundManager::GetInstance().Stop(SoundManager::SOUND::BGM_GAME);
		ChangeState(GAME_STATE::GAME_END);
	}

	SceneManager::GetInstance().GetCamera()->Update();
}

void SceneGame::UpdateGameEnd(void)
{
	slowCount_++;

	if (boss_->GetHP() <= 0)
	{
		if (slowCount_ <= SLOW_COUNT_MAX * 10)
		{
			if (slowCount_ % SLOW_COUNT_MAX == 0)
			{
				boss_->Update();

			}
		}
		else
		{
			boss_->Update();
			EffectManager::GetInstance().Update();
		}
	}
	else
	{
		boss_->Update();
		EffectManager::GetInstance().Update();
	}


	if (slowCount_ >= SLOW_COUNT_MAX * 100)
	{
		if (boss_->GetHP() >= 0)
		{
			SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(true));
		}
		else
		{
			SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(false));
		}
		
	}

	stage_->Update();
	
}

void SceneGame::DrawGame(void)
{


	stage_->Draw();

	for (auto& player : players_)
	{
		player->Draw();
	}

	boss_->Draw();
	for (auto& enemyRobo : enemyRobots_)
	{
		if (!enemyRobo->IsAlive()) continue;
		enemyRobo->Draw();
	}
	auto& effect = EffectManager::GetInstance();
	effect.Draw();

	ShaderController::GetInstance()
		.CreateShaderDrawRainy(0, 0, rainyMaterial_);

	DrawHpBerBoss();

	gameTimer_->Draw();

	DrawRotaGraph((Application::SCREEN_SIZE_X - (770 / 6)), (587 / 6), 0.25, 0.0, infoImage_, true);

	DrawRotaGraph((Application::SCREEN_HALF_X - 300), 35, 0.5, 0.0, uiGame_.at(0), true);

	DrawHpBerPlayer();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

	SceneManager::GetInstance().GetCamera()->DrawDebug();
}

void SceneGame::DrawGameEnd(void)
{
	stage_->Draw();
	boss_->Draw();
	auto& effect = EffectManager::GetInstance();
	effect.Draw();

	ShaderController::GetInstance()
		.CreateShaderDrawRainy(0, 0, rainyMaterial_);

	const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 3;
	if (slowCount_ >= SLOW_COUNT_MAX * 30)
	{
		if (boss_->GetHP() > 0)
		{
			DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 2.0f, UtilityMath::DEG2RAD, imageResult_[3], true);
		}
		else
		{
			DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 2.0f, UtilityMath::DEG2RAD, imageResult_[1], true);
		}
		
	}

}


