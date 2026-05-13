
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Manager/System/Loading.h"
#include "../../Camera/Camera.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"

#include "SceneGameBoss.h"

SceneGameBoss::SceneGameBoss(void)
	: sceneManager_(SceneManager::GetInstance())
{
}

SceneGameBoss::~SceneGameBoss(void)
{
}

void SceneGameBoss::Load(void)
{
	Loading::GetInstance()->SetProgress(10.0f);

	SceneBase::Load();

	Loading::GetInstance()->SetProgress(15.0f);

	Loading::GetInstance()->SetProgress(25.0f);


	Loading::GetInstance()->SetProgress(45.0f);


	Loading::GetInstance()->SetProgress(60.0f);


	Loading::GetInstance()->SetProgress(80.0f);

	//時間カウントリセット
	TimeManager::GetInstance().Reset();


	Loading::GetInstance()->SetProgress(100.0f);


}

void SceneGameBoss::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGameBoss::Initialize(void)
{
	sceneManager_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);



	if (Loading::GetInstance()->IsLoading()) { return; }

	boss_ = std::make_unique<Boss>();
	boss_->Init();


}

void SceneGameBoss::Update(void)
{
	if (Loading::GetInstance()->IsLoading()) { return; }

	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = TimeManager::GetInstance();
	auto& camera = sceneManager_.GetCamera();
	auto loader = Loading::GetInstance();

	// 時間を取得
	float times = time.GetGameTime();

	boss_->Update();
}

void SceneGameBoss::Draw(void)
{
	auto& camera = sceneManager_.GetCamera();

	boss_->Draw();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

}

void SceneGameBoss::Release(void)
{

}

void SceneGameBoss::DrawDebug(void)
{

}