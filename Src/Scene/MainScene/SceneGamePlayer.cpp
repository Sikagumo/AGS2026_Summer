#include "SceneGamePlayer.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Utility/UtilityMath.h"


SceneGamePlayer::SceneGamePlayer(void)
	: sceneManager_(SceneManager::GetInstance())
	, tempBossWeaponPos_(VGet(0.0f, 100.0f, 500.0f))
{
	player_ = std::make_unique<Player>(0, Player::BULLET_TYPE::BIG);
	boss_ = std::make_unique<Boss>();
	stage_ = std::make_unique<Stage>();
	damageController_ = std::make_unique<DamageController>();
}

void SceneGamePlayer::Load(void)
{
	Loading::GetInstance()->SetProgress(10.0f);

	SceneBase::Load();

	Loading::GetInstance()->SetProgress(15.0f);

	player_->Load();

	Loading::GetInstance()->SetProgress(25.0f);

	boss_->Load();

	Loading::GetInstance()->SetProgress(45.0f);

	stage_->Load();

	Loading::GetInstance()->SetProgress(60.0f);


	Loading::GetInstance()->SetProgress(80.0f);

	//時間カウントリセット
	TimeManager::GetInstance().Reset();


	Loading::GetInstance()->SetProgress(100.0f);
}

void SceneGamePlayer::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGamePlayer::Initialize(void)
{
	sceneManager_.GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
	sceneManager_.GetCamera()->SetFollow(&player_->GetTransform());

	if (Loading::GetInstance()->IsLoading()) { return; }

	player_->Init();
	boss_->Init();
	stage_->Init();
}

void SceneGamePlayer::Update(void)
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
			camera->LockOnChoice();
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

void SceneGamePlayer::Draw(void)
{
	auto& camera = sceneManager_.GetCamera();

	stage_->Draw();

	player_->Draw();

	boss_->Draw();

#ifdef _DEBUG
	DrawDebug();

#endif // _DEBUG

}

void SceneGamePlayer::Release(void)
{
	stage_->Release();

	player_->Release();

	boss_->Release();
}

void SceneGamePlayer::DrawDebug(void)
{
	player_->DrawDebug();
}