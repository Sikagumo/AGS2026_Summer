#include "SceneGamePlayer.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Manager/System/Loading.h"
#include "../../Camera/Camera.h"


SceneGamePlayer::SceneGamePlayer(void)
	: sceneManager_(SceneManager::GetInstance())
{
	player_ = std::make_unique<Player>(0, Player::PLAYER_TYPE::BIG);
}

void SceneGamePlayer::Load(void)
{
	Loading::GetInstance()->SetProgress(10.0f);

	SceneBase::Load();

	Loading::GetInstance()->SetProgress(15.0f);

	player_->Init();

	Loading::GetInstance()->SetProgress(25.0f);


	Loading::GetInstance()->SetProgress(45.0f);


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

	player_->Update();
}

void SceneGamePlayer::Draw(void)
{
	auto& camera = sceneManager_.GetCamera();

	player_->Draw();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

}

void SceneGamePlayer::Release(void)
{
	player_->Release();
}

void SceneGamePlayer::DrawDebug(void)
{

}