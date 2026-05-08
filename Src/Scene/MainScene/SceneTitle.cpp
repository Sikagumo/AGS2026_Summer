#include "SceneTitle.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "SceneGame.h"
#include "../../Application.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Manager/System/Loading.h"



void SceneTitle::Load(void)
{
    // isLoading_ を true に
    SceneBase::Load();

    // BGM・SEロード
    Loading::GetInstance()->SetProgress(25.0f);

    // 音量調整
    Loading::GetInstance()->SetProgress(45.0f);

    // ロゴ・操作説明・再生用画像ロード
    Loading::GetInstance()->SetProgress(60.0f);

    // その他画像
    Loading::GetInstance()->SetProgress(80.0f);

    //時間カウントリセット
    TimeManager::GetInstance().Reset();

    Loading::GetInstance()->SetProgress(100.0f);
}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

SceneTitle::SceneTitle(void)
: exitRequested_(false)
{
}

void SceneTitle::Initialize(void)
{
}

void SceneTitle::Update(void)
{

    if (Loading::GetInstance()->IsLoading()) { return; }

    auto& input = InputManager::GetInstance();
}

void SceneTitle::Draw(void)
{

    DrawString(0, 0, "Title Scene Now!", GetColor(255, 255, 255));
}

void SceneTitle::Release(void)
{

}

bool SceneTitle::IsExitRequested(void) const
{
    return exitRequested_;
}


void SceneTitle::DrawDebug(void)
{

}

