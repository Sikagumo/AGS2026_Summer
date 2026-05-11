#include "SceneGame.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Manager/System/Loading.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"
//#include "SceneScore.h"

SceneGame::SceneGame(void)
{

}

void SceneGame::Load(void)
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

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{

	if (Loading::GetInstance()->IsLoading()) { return; }

	boss_ = std::make_unique<Boss>();
	boss_->Init();
}

void SceneGame::Update(void)
{

	if (Loading::GetInstance()->IsLoading()) { return; }

	boss_->Update();
}

void SceneGame::Draw(void)
{
	DrawString(0, 0, "Game Scene Now!", GetColor(255, 255, 255));

	boss_->Draw();

#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG

}

void SceneGame::Release(void)
{
	
}

void SceneGame::DrawDebug(void)
{

}