#include "SceneGame.h"
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
{
	std::unique_ptr<Player> player = std::make_unique<Player>(0, Player::BULLET_TYPE::BIG, UtilityMath::VECTOR_ZERO);
	players_.emplace_back(std::move(player));
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

	DamageProcess();

	UpdateGameTime();

	// ボスHPが０の時、ゲームクリア
	if (boss_->GetHP() <= 0 && gameTime_ > 0.0f)
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(false));
	}
}

void SceneGame::DamageProcess(void)
{
	boss_->SetPlayer1Pos(players_.at(0)->GetTransform().pos);
	
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
	players_.at(0)->SetDamage(damageController_->GetPlayerDamage(), true);
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

void SceneGame::DrawDebug(void)
{
	SceneManager::GetInstance().GetCamera()->DrawDebug();
}
