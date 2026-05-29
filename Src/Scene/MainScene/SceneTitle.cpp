#include "SceneTitle.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "SceneGame.h"
#include "../../Application.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Utility/UtilityMath.h"



void SceneTitle::Load(void)
{
    // isLoading_ を true に
    SceneBase::Load();
    auto& resourceManager = ResourceManager::GetInstance();

    // BGM・SEロード

    // 音量調整

    // ロゴ・操作説明・再生用画像ロード

    imageTitle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_TITLE);
    ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGE_TITLE_TEXT, imageMenu_.data());

    // その他画像

    //時間カウントリセット
    TimeManager::GetInstance().Reset();
}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

SceneTitle::SceneTitle(void)
    : imageTitle_(-1)
{
}

void SceneTitle::Initialize(void)
{

}

void SceneTitle::Update(void)
{

    if (Loading::GetInstance()->IsLoading()) { return; }

    auto& sceneManager = SceneManager::GetInstance();
    auto& input = InputManager::GetInstance();

    if (input.IsTrgDown(KEY_INPUT_SPACE))
    {
        sceneManager.ChangeScene(std::make_shared<SceneGame>());
    }
}

void SceneTitle::UpdateCollision(void)
{
}

void SceneTitle::Draw(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

    const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 3;

    // タイトル
    DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 1.0f, UtilityMath::DEG2RAD, imageTitle_, true);
  

    DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 1.0f, 
        UtilityMath::DEG2RAD, imageMenu_[0], true);
}

void SceneTitle::Release(void)
{

}

void SceneTitle::DrawDebug(void)
{

}

