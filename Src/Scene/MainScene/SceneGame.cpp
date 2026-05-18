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
}


void SceneGame::Load(void)
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

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{
	sceneManager_.GetCamera()->ChangeMode(Camera::MODE::FOLLOW);
	sceneManager_.GetCamera()->SetFollow(&player_->GetTransform());

	if (Loading::GetInstance()->IsLoading()) { return; }

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
}

void SceneGame::Draw(void)
{
	DrawString(0, 0, "Game Scene Now!", GetColor(255, 255, 255));

	stage_->Draw();

	player_->Draw();

	boss_->Draw();
	
	// 追従位置
	DrawSphere3D(tempBossWeaponPos_, 10.0f, 16, 0x0000ff, 0xffffff, true);

	constexpr float GROUND_SIZE = 5000.0f;
	DrawCube3D(VGet(GROUND_SIZE, 0.0f, GROUND_SIZE), VGet(-GROUND_SIZE, -100.0f, -GROUND_SIZE),
		0xff00ff, 0xffffff, true);


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