#include "SceneGame.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Common/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Utility/UtilityMath.h"
#include "SceneTitle.h"
#include "SceneResult.h"
//#include "SceneScore.h"

SceneGame::SceneGame(void)
{
	player_ = std::make_unique<Player>(0, Player::BULLET_TYPE::BIG);
	boss_ = std::make_unique<Boss>();
	stage_ = std::make_unique<Stage>();
	damageController_ = std::make_unique<DamageController>();

}


void SceneGame::Load(void)
{
	SceneBase::Load();

	player_->Load();

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
	camera->SetFollow(&player_->GetTransform());

	player_->Init();
	boss_->Init();
	stage_->Init();
	damageController_->SetPlayerMaxHp(player_->GetMaxHp());
}

void SceneGame::Update(void)
{
	if (Loading::GetInstance()->IsLoading()) { return; }

	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = TimeManager::GetInstance();
	auto& camera = SceneManager::GetInstance().GetCamera();
	auto loader = Loading::GetInstance();

	// 時間を取得
	float times = time.GetGameTime();

	player_->Update();
	boss_->Update();
	stage_->Update();
	damageController_->Update();
	
	CameraLockOn();

	DamageProcess();

	if (player_->GetCurHp() <= 0 || boss_->GetHP() <= 0)
	{
		bool isGameOver = (boss_->GetHP() <= 0 && player_->GetCurHp() > 0);
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>(isGameOver));
	}
}

void SceneGame::DamageProcess(void)
{
	boss_->SetBossDamage(damageController_->GetBossDamage());

	boss_->SetWeaponCannonLDamage(damageController_->GetWeaponCannonLDamage());
	boss_->SetWeaponCannonRDamage(damageController_->GetWeaponCannonRDamage());

	boss_->SetWeaponMGLDamage(damageController_->GetWeaponMGLDamage());
	boss_->SetWeaponMGRDamage(damageController_->GetWeaponMGRDamage());

	boss_->SetWeaponMPLDamage(damageController_->GetWeaponMPLDamage());
	boss_->SetWeaponMPRDamage(damageController_->GetWeaponMPRDamage());

	boss_->SetWeaponRGDamage(damageController_->GetWeaponRGDamage());

	// プレイヤーの攻撃
	damageController_->SetPlayerAttack(player_->GetPower());

	// プレイヤー被ダメージ処理
	player_->SetDamage(damageController_->GetPlayerDamage());
}

void SceneGame::CameraLockOn(void)
{
	auto& camera = SceneManager::GetInstance().GetCamera();

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_E))
	{
		if (!camera->GetIsLockOn())
		{
			camera->LockOnChoice();
		}
		else
		{
			camera->SetIsLockOn(false);
		}
	}

	// ロックオン時、常に追従位置を取得する
	if (camera->GetIsLockOn())
	{
		camera->FollowLockOnPosition();
	}
}

void SceneGame::UpdateCollision(void)
{
	boss_->UpdateCollision();
}


void SceneGame::Draw(void)
{

	stage_->Draw();

	player_->Draw();

	boss_->Draw();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

	SceneManager::GetInstance().GetCamera()->DrawDebug();
}

void SceneGame::Release(void)
{
	stage_->Release();
	
	player_->Release();

	boss_->Release();
}

void SceneGame::DrawDebug(void)
{
	SceneManager::GetInstance().GetCamera()->DrawDebug();
}
