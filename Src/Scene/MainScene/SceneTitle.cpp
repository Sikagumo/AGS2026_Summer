#include "SceneTitle.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Object/Manager/CollisionManager.h"
#include "../../Camera/Camera.h"
#include "SceneGame.h"
#include "SceneResult.h"
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
    for (size_t i = 0; i < imageMenu_.size(); ++i)
    {
        imageMenu_[i] = -1;
    }
}

void SceneTitle::Initialize(void)
{
    SetMouseDispFlag(true);

    // マウスカーソル用のコライダー生成（半径1の円）
    cursorCollider_ = std::make_unique<Collider2DCircle>(Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);
    CollisionManager::GetInstance().RegisterCollider2D(cursorCollider_.get());

    // UIボタンの配置計算設定
    const float BUTTON_WIDTH = 250.0f;
    const float BUTTON_HEIGHT = 50.0f;
    const float START_Y = Application::SCREEN_HALF_Y + 10;
    const float INTERVAL_Y = 100.0f;
    const float CENTER_X = Application::SCREEN_HALF_X;

    // ソロプレイボタン
    Vector2F posSolo(CENTER_X, START_Y + (INTERVAL_Y * 0.0f));
    soloPlayButtonCollider_ = std::make_unique<Collider2DBox>(posSolo, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON);
    CollisionManager::GetInstance().RegisterCollider2D(soloPlayButtonCollider_.get());
    CollisionManager::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON, true);

    // マルチプレイボタン
    Vector2F posMulti(CENTER_X, START_Y + (INTERVAL_Y * 1.0f));
    multiPlayButtonCollider_ = std::make_unique<Collider2DBox>(posMulti, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON);
    CollisionManager::GetInstance().RegisterCollider2D(multiPlayButtonCollider_.get());
    CollisionManager::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON, true);

    // 設定ボタン
    Vector2F posOption(CENTER_X, START_Y + (INTERVAL_Y * 2.0f));
    optionButtonCollider_ = std::make_unique<Collider2DBox>(posOption, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::OPTION_BUTTON);
    CollisionManager::GetInstance().RegisterCollider2D(optionButtonCollider_.get());
    CollisionManager::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::OPTION_BUTTON, true);

    // 終了ボタン
    Vector2F posExit(CENTER_X, START_Y + (INTERVAL_Y * 3.0f));
    exitButtonCollider_ = std::make_unique<Collider2DBox>(posExit, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::EXIT_UTTON);
    CollisionManager::GetInstance().RegisterCollider2D(exitButtonCollider_.get());
    CollisionManager::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::EXIT_UTTON, true);

    SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);
}

void SceneTitle::Update(void)
{

    if (Loading::GetInstance()->IsLoading()) { return; }

    auto& sceneManager = SceneManager::GetInstance();
    auto& input = InputManager::GetInstance();

    Vector2 mousePos = input.GetMousePos();
    Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    cursorCollider_->SetCenterPos(mousePosF);

    using TAG_2D = Collider2DBase::TAG_2D;

    // ソロプレイ
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::SOLO_PLAY_BUTTON))
    {
        if (input.IsTrgMouseLeft())
        {
            sceneManager.ChangeScene(std::make_shared<SceneGame>());
        }
    }

    // マルチ
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::MULTI_PLAY_BUTTON))
    {
        if (input.IsTrgMouseLeft())
        {
            
        }
    }

    // 設定
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::OPTION_BUTTON))
    {
        if (input.IsTrgMouseLeft())
        {
         
        }
    }

    // 終了
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::EXIT_UTTON))
    {
        if (input.IsTrgMouseLeft())
        {

        }
    }

#ifdef _DEBUG
    if (input.IsTrgDown(KEY_INPUT_SPACE))
    {
        sceneManager.ChangeScene(std::make_shared<SceneResult>(false));
    }
#endif
}

void SceneTitle::UpdateCollision(void)
{
}

void SceneTitle::Draw(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

    using TAG_2D = Collider2DBase::TAG_2D;

    DrawString(0, 0, "Title Scene Now!", GetColor(255, 255, 255));
    
    const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 3;

    // タイトル
    DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 1.0f, UtilityMath::DEG2RAD, imageTitle_, true);


    const float DEFULT_SCALE = 0.5f;
    const float SELECT_SCALE = 2.0f;


    // ソロプレイ
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::SOLO_PLAY_BUTTON))
    {
        Vector2F posSolo = soloPlayButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posSolo.x), static_cast<int>(posSolo.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[1], true);

    }
    else
    {
        Vector2F posSolo = soloPlayButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posSolo.x), static_cast<int>(posSolo.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[0], true);
    }

    // マルチ
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::MULTI_PLAY_BUTTON))
    {
        Vector2F posMulti = multiPlayButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posMulti.x), static_cast<int>(posMulti.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[3], true);
    }
    else
    {
        Vector2F posMulti = multiPlayButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posMulti.x), static_cast<int>(posMulti.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[2], true);

    }

    // 設定
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::OPTION_BUTTON))
    {

        Vector2F posOption = optionButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posOption.x), static_cast<int>(posOption.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[5], true);

    }
    else
    {

        Vector2F posOption = optionButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posOption.x), static_cast<int>(posOption.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[4], true);

    }

    // 終了
    if (CollisionManager::GetInstance().IsTagCollidingWithTag2D
    (TAG_2D::MOUSE_CURSOR, TAG_2D::EXIT_UTTON))
    {
        Vector2F posExit = exitButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posExit.x), static_cast<int>(posExit.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[7], true);
    }
    else
    {
        Vector2F posExit = exitButtonCollider_->GetCenterPos();
        DrawRotaGraph(static_cast<int>(posExit.x), static_cast<int>(posExit.y), DEFULT_SCALE, UtilityMath::DEG2RAD, imageMenu_[6], true);
    }

#ifdef _DEBUG
    DrawDebug();
#endif // _DEBUG

}

void SceneTitle::Release(void)
{

}

void SceneTitle::DrawDebug(void)
{
    CollisionManager::GetInstance().DrawDebug2D();
}

