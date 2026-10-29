#define NOMINMAX
#include "SceneGame.h"

#include <algorithm>
#include <cmath>

#include "../../Manager/Generic/SceneManager.h"
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

namespace
{
	// 描画・UI関連の定数
	constexpr float RAIN_POW = 1.0f;               // 雨の強さの基本係数
	constexpr float RAIN_POW_BACK = 1.0f;          // 背景における雨の強さの係数
	constexpr int INFO_IMAGE_X_OFFSET = 770 / 6;   // 情報画像のX方向オフセット
	constexpr int INFO_IMAGE_Y_OFFSET = 587 / 6;   // 情報画像のY方向オフセット
	constexpr int UI_GAME_X_OFFSET = 300;          // ゲームUIのX方向オフセット
	constexpr int UI_GAME_Y_POS = 35;              // ゲームUIのY方向位置
	constexpr int INVALID_HANDLE = -1;             // 無効なリソースハンドルを示す値
	constexpr float HP_RATIO_MIN = 0.0f;           // HP比率の最小値（0%）
	constexpr float HP_RATIO_MAX = 1.0f;           // HP比率の最大値（100%）

	// HPバー描画用の定数
	constexpr float PLAYER_HP_BAR_SCALE = 0.275f;				 // プレイヤー用HPバーの表示スケール
	constexpr Vector2 PLAYER_HP_BAR_MIDDLE_POS = { 200, 500 };	 // プレイヤー用HPバーの中央基準座標
	constexpr int PLAYER_HP_BAR_OFFSET_Y = 100;					 // 複数プレイヤー表示時の縦方向の間隔

	constexpr float BOSS_HP_BAR_SCALE = 0.5f;					 // ボス用HPバーの表示スケール
	constexpr Vector2F BOSS_HP_BAR_OFFSET = { 0.0f, -100.0f };	 // ボス頭上等のバー表示オフセット
	constexpr float BOSS_HP_DISPLAY_RATIO_MIN = 0.3f;			 // ボスHPバーの表示比率最小閾値
	constexpr float BOSS_HP_DISPLAY_RATIO_MAX = 0.9f;			 // ボスHPバーの表示比率最大閾値
	constexpr int BOSS_HP_BAR_OFFSET_X = -10;					 // ボス用HPバーのX方向微調整オフセット

	constexpr float HP_BAR_DIST_MIN = 50.0f;       // HPバーのスケール計算における最小距離
	constexpr float HP_BAR_MIN_SCALE = 0.375f;     // HPバーの最小スケール制限
	constexpr float HP_BAR_MAX_SCALE = 1.5f;       // HPバーの最大スケール制限

	constexpr float CONNECTION_TIMEOUT_SEC = 5.0f; // ネットワーク接続のタイムアウト時間（秒）
	constexpr double RESULT_IMAGE_SCALE = 0.25;    // リザルト画面での画像表示スケール
	constexpr double UI_GAME_IMAGE_SCALE = 0.5;    // ゲーム中UI画像の表示スケール
	constexpr double RESULT_TITLE_SCALE = 2.0f;    // リザルトタイトルの表示スケール
}

SceneGame::SceneGame(std::vector<PlayerSelectType> _playerSelectType)
	: players_()
	, boss_(std::make_unique<Boss>())
	, enemyRobots_()
	, stage_(std::make_unique<Stage>())
	, damageController_(std::make_unique<DamageController>())
	, gameTimer_(nullptr)
	, targetHpImage_(INVALID_HANDLE)
	, targetHpBerImage_(INVALID_HANDLE)
	, infoImage_(INVALID_HANDLE)
	, gameTexts_(INVALID_HANDLE)
	, uiGame_()
	, playerHpImage_(INVALID_HANDLE)
	, playerHpImageBack_(INVALID_HANDLE)
	, slowCount_(0)
	, imageResult_()
	, isGameOver_(false)
	, rainyParams_()
	, rainyTime_(0.0f)
	, state_(GAME_STATE::NONE)
	, stateBase_(0)
	, stateChanges_()
	, stateUpdate_(nullptr)
	, stateDraw_(nullptr)
{
	// プレイヤー選択情報をもとにプレイヤーインスタンスを生成
	for (size_t i = 0; i < _playerSelectType.size(); ++i)
	{
		const auto job = _playerSelectType.at(i).job;
		const auto skin = _playerSelectType.at(i).skin;
		std::unique_ptr<Player> player = std::make_unique<Player>(static_cast<int>(i), job, skin, PLAYER_INIT_POS[i]);

		players_.emplace_back(std::move(player));
	}

	// 指定数分の敵ロボを生成
	for (int i = 0; i < ENEMYS_POP; ++i)
	{
		std::unique_ptr<EnemyRobo> enemy = std::make_unique<EnemyRobo>(ENEMY_POS[i]);

		enemyRobots_.emplace_back(std::move(enemy));
	}

	boss_->SetPlayerSize(static_cast<int>(_playerSelectType.size()));
}

void SceneGame::Load(void)
{
	SceneBase::Load();

	// HPバーやUI画像のロード
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

	infoImage_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_INFO);

	boss_->Load();
	stage_->Load();

	gameTimer_ = std::make_unique<GameTimer>(GAME_TIME);

	// 各種サウンドの追加
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_GAME
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_GAME));

	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_THUNDER
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_SELECT));

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_HIT_BLAST
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_HIT_BLAST));

	TimeManager::GetInstance().Reset();
}

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{
	if (Loading::GetInstance()->IsLoading())
	{
		return;
	}

	NetManager::GetInstance().SetConnectionTimeout(CONNECTION_TIMEOUT_SEC);
	SetMouseDispFlag(false);

	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->Init();
	camera->ChangeMode(Camera::MODE::PLAYER_FOLLOW);
	camera->SetFollow(&players_.at(0)->GetTransform());

	const NET_JOIN_USER selfUser = NetManager::GetInstance().GetSelfUser();
	const auto remoteUsers = NetManager::GetInstance().GetNetUsers();
	const bool isBossLocalControl = (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT);

	boss_->SetHostControl(isBossLocalControl);

	// ホストである自分の設定
	players_.at(0)->SetHostControl(true);
	players_.at(0)->SetNetKey(selfUser.key);

	// クライアントなど他人の設定
	int idx = 1;
	for (const auto& pair : remoteUsers)
	{
		if (idx >= static_cast<int>(players_.size()))
		{
			break;
		}
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

	gameTimer_->SetIsTimeActive(true);
	damageController_->SetPlayerMaxHp(Player::HP_MAX);

	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_GAME);
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);

	// 雨シェーダーのパラメータ初期化
	rainyParams_.rainIntensity = RAIN_POW;
	rainyParams_.rainIntensityBack = RAIN_POW_BACK;
	rainyParams_.rainColor = GetColorF(0.75f, 0.82f, 0.9f, 1.0f);
	rainyParams_.resolutionX = static_cast<float>(Application::SCREEN_SIZE_X);
	rainyParams_.resolutionY = static_cast<float>(Application::SCREEN_SIZE_Y);

	SetMouseDispFlag(true);

	for (size_t i = 0; i < imageResult_.size(); ++i)
	{
		imageResult_[i] = INVALID_HANDLE;
	}

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_RESULT, imageResult_.data());

	stateChanges_.emplace(static_cast<int>(GAME_STATE::GAME), std::bind(&SceneGame::ChangeGame, this));
	stateChanges_.emplace(static_cast<int>(GAME_STATE::GAME_END), std::bind(&SceneGame::ChangeGameEnd, this));

	ChangeState(GAME_STATE::GAME);
}

void SceneGame::Update(void)
{
	if (KeyConfInputManager::GetInstance().IsTriggerDown("PAUSE"))
	{
		SceneManager::GetInstance().PushScene(std::make_shared<ScenePause>());
	}

	if (stateUpdate_ != nullptr)
	{
		stateUpdate_();
	}
}

void SceneGame::DamageProcess(void)
{
	int playerCount = 0;

	for (auto& enemyRobo : enemyRobots_)
	{
		enemyRobo->SetPlayerPos(players_.at(0)->GetBodyPos());
	}

	std::vector<std::pair<int, VECTOR>> keyPosList;
	for (auto& player : players_)
	{
		keyPosList.emplace_back(player->GetNetKey(), player->GetBodyPos());
		boss_->SetPlayerPos(player->GetBodyPos(), playerCount);
		playerCount++;
	}

	std::sort(keyPosList.begin(), keyPosList.end(), [](const auto& _a, const auto& _b)
		{
			return _a.first < _b.first;
		});

	if (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT)
	{
		// ボスや各部位へのダメージ適用をホストのみに限定する
		boss_->SetBossDamage(damageController_->GetBossDamage());
		boss_->SetWeaponCannonLDamage(damageController_->GetWeaponCannonLDamage());
		boss_->SetWeaponCannonRDamage(damageController_->GetWeaponCannonRDamage());
		boss_->SetWeaponMGLDamage(damageController_->GetWeaponMGLDamage());
		boss_->SetWeaponMGRDamage(damageController_->GetWeaponMGRDamage());
		boss_->SetWeaponMPLDamage(damageController_->GetWeaponMPLDamage());
		boss_->SetWeaponMPRDamage(damageController_->GetWeaponMPRDamage());
		boss_->SetWeaponRGDamage(damageController_->GetWeaponRGDamage());
	}

	if (damageController_->GetBossDamage() > 0 ||
		damageController_->GetWeaponCannonLDamage() > 0 ||
		damageController_->GetWeaponCannonRDamage() > 0 ||
		damageController_->GetWeaponMGLDamage() > 0 ||
		damageController_->GetWeaponMGRDamage() > 0 ||
		damageController_->GetWeaponMPLDamage() > 0 ||
		damageController_->GetWeaponMPRDamage() > 0 ||
		damageController_->GetWeaponRGDamage() > 0)
	{
		SoundManager::GetInstance().Play(SoundManager::SOUND::SE_HIT_BLAST);
	}

	for (auto& player : players_)
	{
		if (!player->GetHostControl())
		{
			continue;
		}

		player->SetDamage(damageController_->GetPlayerDamage(), damageController_->GetInvincible());
	}
}

void SceneGame::UpdateGameTime(void)
{
	if (NetManager::GetInstance().GetMode() != NET_MODE::CLIENT)
	{
		gameTimer_->Update();

		for (auto& player : players_)
		{
			if (!player->IsRespawn())
			{
				continue;
			}
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

	rainyTime_ = TimeManager::GetInstance().GetGameTime();
	rainyParams_.timeValue = rainyTime_;
}

void SceneGame::Draw(void)
{
	if (stateDraw_ != nullptr)
	{
		stateDraw_();
	}
}

void SceneGame::Release(void)
{
	stage_->Release();

	for (auto& player : players_)
	{
		player->Release();
	}

	boss_->Release();

	auto& camera = SceneManager::GetInstance().GetCamera();
	camera->SetFollow(nullptr);
	camera->ChangeMode(Camera::MODE::NONE);

	NetManager::GetInstance().Stop();
}

void SceneGame::DrawHpBerPlayer(void)
{
	Vector2 backSize = Vector2();
	GetGraphSize(playerHpImageBack_, &backSize.x, &backSize.y);
	backSize *= PLAYER_HP_BAR_SCALE;

	Vector2 imageSize = Vector2();
	GetGraphSize(playerHpImage_, &imageSize.x, &imageSize.y);

	Vector2 berPos = PLAYER_HP_BAR_MIDDLE_POS;

	const int playerNum = static_cast<int>(players_.size());
	for (int i = 0; i < playerNum; ++i)
	{
		DrawRotaGraph(
			berPos.x, berPos.y,
			PLAYER_HP_BAR_SCALE, 0.0f,
			playerHpImageBack_, true
		);

		const float hpRatio = (static_cast<float>(players_.at(i)->GetCurHp()) / static_cast<float>(Player::HP_MAX));
		const float ratio = std::clamp(hpRatio, HP_RATIO_MIN, HP_RATIO_MAX);

		const float displayRatio = std::lerp(HP_RATIO_MIN, HP_RATIO_MAX, ratio);

		const Vector2 posUpperLeft = {
			static_cast<int>(berPos.x - (backSize.x / 2)),
			static_cast<int>(berPos.y - (backSize.y / 2))
		};

		const Vector2 posLowerRight = {
			static_cast<int>(posUpperLeft.x + (backSize.x * displayRatio)),
			static_cast<int>(posUpperLeft.y + backSize.y)
		};

		const int imageWidth = static_cast<int>(imageSize.x * displayRatio);

		if (imageWidth > 0)
		{
			DrawRectExtendGraph(
				posUpperLeft.x, posUpperLeft.y,
				posLowerRight.x, posLowerRight.y,
				0, 0,
				imageWidth, imageSize.y,
				playerHpImage_,
				true
			);
		}

		berPos.y += (backSize.y + PLAYER_HP_BAR_OFFSET_Y);
	}
}

void SceneGame::DrawHpBerBoss(void)
{
	const std::unique_ptr<Camera>& camera = SceneManager::GetInstance().GetCamera();

	if (camera->GetIsLockOn() && !camera->IsEasingState())
	{
		VECTOR viewPos = ConvWorldPosToScreenPos(camera->GetTargetPos());
		viewPos.x += BOSS_HP_BAR_OFFSET.x;
		viewPos.y += BOSS_HP_BAR_OFFSET.y;

		DrawRotaGraph(
			static_cast<int>(viewPos.x), static_cast<int>(viewPos.y),
			BOSS_HP_BAR_SCALE, 0.0f,
			targetHpBerImage_, true
		);

		Vector2 imageSize = Vector2();
		GetGraphSize(targetHpImage_, &imageSize.x, &imageSize.y);

		const Vector2F berSize = { (imageSize.x * BOSS_HP_BAR_SCALE), (imageSize.y * BOSS_HP_BAR_SCALE) };

		const float hpRatio = (camera->GetLockOnMaxHp() > 0)
			? static_cast<float>(camera->GetLockOnHp()) / static_cast<float>(camera->GetLockOnMaxHp())
			: 0.0f;

		const float ratio = std::clamp(hpRatio, HP_RATIO_MIN, HP_RATIO_MAX);

		const Vector2 posUpperLeft = {
			static_cast<int>(viewPos.x - (berSize.x / 2)),
			static_cast<int>(viewPos.y - (berSize.y / 2))
		};

		const Vector2 posLowerRight = {
			static_cast<int>(posUpperLeft.x + (berSize.x * ratio) + BOSS_HP_BAR_OFFSET_X),
			static_cast<int>(posUpperLeft.y + berSize.y)
		};

		const int imageWidth = static_cast<int>(imageSize.x * ratio);

		if (imageWidth > 0)
		{
			DrawRectExtendGraph(
				posUpperLeft.x, posUpperLeft.y,
				posLowerRight.x, posLowerRight.y,
				0, 0,
				imageWidth, imageSize.y,
				targetHpImage_,
				true
			);
		}
	}
}

float SceneGame::CalcHpBarScale(const VECTOR& _targetPos)
{
	const VECTOR cameraPos = SceneManager::GetInstance().GetCamera()->GetPos();
	VECTOR distVec = VSub(_targetPos, cameraPos);
	distVec.y = 0.0f;
	const float dist = VSize(distVec);

	const float safeDist = ((dist < HP_BAR_DIST_MIN) ? HP_BAR_DIST_MIN : dist);
	const float scale = (HP_BAR_DIST_MIN / safeDist);

	return std::clamp(scale, HP_BAR_MIN_SCALE, HP_BAR_MAX_SCALE);
}

void SceneGame::DrawDebug(void)
{
}

void SceneGame::UpdateGui(void)
{
}

void SceneGame::ChangeState(GAME_STATE _state)
{
	state_ = _state;
	ChangeState(static_cast<int>(state_));
}

void SceneGame::ChangeState(int _state)
{
	stateBase_ = _state;
	if (stateChanges_.find(stateBase_) != stateChanges_.end())
	{
		stateChanges_[stateBase_]();
	}
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
	if (Loading::GetInstance()->IsLoading())
	{
		return;
	}

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

	auto remoteHisMap = NetManager::GetInstance().GetRemoteActionHistory();

	for (auto& player : players_)
	{
		if (player->GetHostControl())
		{
			continue;
		}

		const int key = player->GetNetKey();
		if (remoteHisMap.find(key) != remoteHisMap.end())
		{
			const NET_ACTION latestAction = remoteHisMap[key].actions[0];
			player->SetNetworkAction(latestAction.pos, latestAction.rot, latestAction.animId, latestAction.isAttack, latestAction.currentHp);
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

	int attackPower = 0;
	int attackBlast = 0;
	for (auto& player : players_)
	{
		for (auto& bullet : player->GetWeaponController()->GetBullets())
		{
			attackPower = std::max(attackPower, bullet->GetPowerBullet());
			attackBlast = std::max(attackBlast, bullet->GetPowerBlast());
		}

		for (auto& bullet : player->GetWeaponController()->GetBulletsCluster())
		{
			if (bullet == nullptr)
			{
				continue;
			}
			attackPower = std::max(attackPower, bullet->GetPowerBullet());
			attackBlast = std::max(attackBlast, bullet->GetPowerBlast());
		}
	}
	damageController_->SetPlayerAttack(attackPower, attackBlast);

	UpdateGameTime();
	EffectManager::GetInstance().Update();

	if (boss_->GetHP() <= 0 || gameTimer_->GetTime() <= 0.0f)
	{
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
			EffectManager::GetInstance().StopAll();
			SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(true));
		}
		else
		{
			EffectManager::GetInstance().StopAll();
			SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(false));
		}
	}

	stage_->Update();

	rainyTime_ = TimeManager::GetInstance().GetGameTime();
	rainyParams_.timeValue = rainyTime_;
}

void SceneGame::DrawGame(void)
{
	ShaderController::GetInstance().Draw2D(
		ResourceManager::SRC::PS_RAINY,
		0, 0, 1.0f,
		rainyParams_
	);

	stage_->Draw();
	ShaderController::GetInstance().ExecuteDrawCommands();

	for (auto& player : players_)
	{
		player->Draw();
	}

	boss_->Draw();

	for (auto& enemyRobo : enemyRobots_)
	{
		if (!enemyRobo->IsAlive())
		{
			continue;
		}
		enemyRobo->Draw();
	}

	stage_->DrawTree();
	EffectManager::GetInstance().Draw();
	DrawHpBerBoss();
	gameTimer_->Draw();

	DrawRotaGraph(Application::SCREEN_SIZE_X - INFO_IMAGE_X_OFFSET, INFO_IMAGE_Y_OFFSET, RESULT_IMAGE_SCALE, 0.0, infoImage_, true);
	DrawRotaGraph(Application::SCREEN_HALF_X - UI_GAME_X_OFFSET, UI_GAME_Y_POS, UI_GAME_IMAGE_SCALE, 0.0, uiGame_.at(0), true);

	DrawHpBerPlayer();
}

void SceneGame::DrawGameEnd(void)
{
	stage_->Draw();
	ShaderController::GetInstance().ExecuteDrawCommands();

	boss_->Draw();
	EffectManager::GetInstance().Draw();

	ShaderController::GetInstance().Draw2D(
		ResourceManager::SRC::PS_RAINY,
		0, 0, 1.0f,
		rainyParams_
	);

	const int imageTitleY = Application::SCREEN_SIZE_Y / 3;
	if (slowCount_ >= SLOW_COUNT_MAX * 30)
	{
		if (boss_->GetHP() > 0)
		{
			DrawRotaGraph(Application::SCREEN_HALF_X, imageTitleY, RESULT_TITLE_SCALE, UtilityMath::DEG2RAD, imageResult_[3], true);
		}
		else
		{
			DrawRotaGraph(Application::SCREEN_HALF_X, imageTitleY, RESULT_TITLE_SCALE, UtilityMath::DEG2RAD, imageResult_[1], true);
		}
	}
}