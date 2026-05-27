#include "SceneGame.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Utility/UtilityMath.h"
//#include "SceneScore.h"

SceneGame::SceneGame(void)
	: sceneManager_(SceneManager::GetInstance())
	, tempBossWeaponPos_(VGet(0.0f, 100.0f, 500.0f))
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

	sceneManager_.GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
	sceneManager_.GetCamera()->SetFollow(&player_->GetTransform());

	player_->Init();
	boss_->Init();
	stage_->Init();
}

void SceneGame::Update(void)
{
	if (Loading::GetInstance()->IsLoading()) { return; }

	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = TimeManager::GetInstance();
	auto& camera = sceneManager_.GetCamera();
	auto loader = Loading::GetInstance();

	// 時間を取得
	float times = time.GetGameTime();

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_E))
	{
		const std::unique_ptr<Camera>& camera = sceneManager_.GetCamera();
		if (!camera->GetIsLockOn())
		{
			camera->SetLockOnPosition(tempBossWeaponPos_);
		}
		else
		{
			camera->SetIsLockOn(false);
		}
	}
	player_->Update();
	boss_->Update();
	stage_->Update();
	damageController_->Update();
}

void SceneGame::UpdateCollision(void)
{
	boss_->UpdateCollision();
}

void SceneGame::Draw(void)
{
	if (Loading::GetInstance()->IsLoading()) { return; }

	DrawString(0, 0, "Game Scene Now!", GetColor(255, 255, 255));

	stage_->Draw();

	player_->Draw();

	boss_->Draw();
	
	// 追従位置
	DrawSphere3D(tempBossWeaponPos_, 10.0f, 16, 0x0000ff, 0xffffff, true);


#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

}

void SceneGame::Release(void)
{
	stage_->Release();
	
	player_->Release();

	boss_->Release();
}

void SceneGame::DrawDebug(void)
{

}