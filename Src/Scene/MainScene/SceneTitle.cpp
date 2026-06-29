#include "SceneTitle.h"

#include <cmath>

#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Camera/Camera.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "../../Application.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Utility/UtilityMath.h"
#include "../../Shader/ShaderManager.h"

void SceneTitle::Load(void)
{
    // isLoading_ を true に
    SceneBase::Load();

    // BGM・SEロード

    // 音量調整

    // タイトル画像
    imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_TITLE);

    // メニュー画像
    ResourceManager::GetInstance().LoadHandleIds
    (ResourceManager::SRC::IMGS_TITLE_TEXT, imageMenu_.data());

    // 桃の画像
    peachHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_PEACH);

    // 桃のノーマルマップ画像
    peachNormalHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_PEACH);

    // 波の画像
    waveHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_WAVE);

    // 波のノーマルマップ画像
    waveNormalHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_WAVE);

    // 鬼ヶ島の画像
    oniSimaHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_ONIGASIMA);

    // 鬼ヶ島のノーマルマップ画像
    oniSimaNormalHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_ONIGASIMA);

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
    , peachHandle_(-1)
    , peachNormalHandle_(-1)
    , waveHandle_(-1)
    , waveNormalHandle_(-1)
    , oniSimaHandle_(-1)
    , oniSimaNormalHandle_(-1)
    , time_(0)
    , selectedIdx_(0)
    , cursorCollider_(nullptr)
    , soloPlayButtonCollider_(nullptr)
    , multiPlayButtonCollider_(nullptr)
    , optionButtonCollider_(nullptr)
    , exitButtonCollider_(nullptr)
    , imageMenu_()
    , buttonTags()
    , prevMousePos_(0.0f, 0.0f)
    , psHandle_(-1)
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
    cursorCollider_ = std::make_unique<Collider2DCircle>
        (Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);

    CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

    // UIボタンの配置計算設定
    const float BUTTON_WIDTH = 250.0f;
    const float BUTTON_HEIGHT = 50.0f;
    const float START_Y = Application::SCREEN_HALF_Y + 20;
    const float INTERVAL_Y = 100.0f;
    const float CENTER_X = Application::SCREEN_HALF_X;

    // ソロプレイボタン
    Vector2F posSolo(CENTER_X, START_Y + (INTERVAL_Y * 0.0f));
    soloPlayButtonCollider_ = std::make_unique<Collider2DBox>
        (posSolo, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON);
    CollisionController::GetInstance().RegisterCollider2D(soloPlayButtonCollider_.get());
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON, true);

    // マルチプレイボタン
    Vector2F posMulti(CENTER_X, START_Y + (INTERVAL_Y * 1.0f));
    multiPlayButtonCollider_ = std::make_unique<Collider2DBox>(posMulti, BUTTON_WIDTH, 
        BUTTON_HEIGHT, Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON);
    CollisionController::GetInstance().RegisterCollider2D(multiPlayButtonCollider_.get());
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, 
        Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON, true);

    // 設定ボタン
    Vector2F posOption(CENTER_X, START_Y + (INTERVAL_Y * 2.0f));
    optionButtonCollider_ = std::make_unique<Collider2DBox>(posOption, BUTTON_WIDTH,
        BUTTON_HEIGHT, Collider2DBase::TAG_2D::OPTION_BUTTON);
    CollisionController::GetInstance().RegisterCollider2D(optionButtonCollider_.get());
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::OPTION_BUTTON, true);

    // 終了ボタン
    Vector2F posExit(CENTER_X, START_Y + (INTERVAL_Y * 3.0f));
    exitButtonCollider_ = std::make_unique<Collider2DBox>(posExit, BUTTON_WIDTH, 
        BUTTON_HEIGHT, Collider2DBase::TAG_2D::EXIT_UTTON);
    CollisionController::GetInstance().RegisterCollider2D(exitButtonCollider_.get());
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, 
        Collider2DBase::TAG_2D::EXIT_UTTON, true);

    SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);

    buttonTags =
    {
        Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON,
        Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON,
        Collider2DBase::TAG_2D::OPTION_BUTTON,
        Collider2DBase::TAG_2D::EXIT_UTTON
    };
}

void SceneTitle::Update(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

    auto& sceneManager = SceneManager::GetInstance();
    auto& keyConfInputManager = KeyConfInputManager::GetInstance();

    // スティック入力による選択インデックスの更新
    Vector2F stick = keyConfInputManager.GetLeftStickRaw();
    const float THRESHOLD = 0.5f;
    const int STICK_TINERVAL = 15;
    static int inputIntervalCounter = 0;

    if (inputIntervalCounter > 0)
    {
        inputIntervalCounter--;
    }
    else if (std::abs(stick.y) > THRESHOLD)
    {
        if (stick.y < 0.0f) 
        { 
            selectedIdx_ = (selectedIdx_ + 1) % MENU_BUTTON_NUM; 
        }
        else 
        {
            selectedIdx_ = (selectedIdx_ - 1 + MENU_BUTTON_NUM) % MENU_BUTTON_NUM; 
        }
        inputIntervalCounter = STICK_TINERVAL;
    }

    // マウス座標の更新
    Vector2 mousePos = keyConfInputManager.GetMousePosition();
    Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    cursorCollider_->SetCenterPos(mousePosF);

    // マウスが動いたときはパッドの選択カーソルも追従させる
    for (int i = 0; i < MENU_BUTTON_NUM; ++i)
    {
        if (CollisionController::GetInstance().IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, buttonTags[i]))
        {
            selectedIdx_ = i;
            break;
        }
    }

    // 決定処理
    for (int i = 0; i < MENU_BUTTON_NUM; ++i)
    {
        bool isTarget = CollisionController::GetInstance().IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, buttonTags[i]) || (selectedIdx_ == i);

        if (isTarget && keyConfInputManager.isTrigerDown("OK"))
        {
            switch (static_cast<MENU_ITEM>(i))
            {
            case MENU_ITEM::SOLO:
                sceneManager.ChangeScene(std::make_shared<SceneGame>());
                break;
            case MENU_ITEM::MULTI:
                // マルチプレイ処理
                break;
            case MENU_ITEM::OPTION:
                // 設定処理
                break;
            case MENU_ITEM::EXIT:
                Application::GetInstance().GameEnd();
                break;
            }
        }
    }
}

void SceneTitle::Draw(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

    time_ += 0.02f;

    float amplitude = 20.0f;
    float bobSpeed = 3.0f;
    float offsetY = std::sinf(time_ * bobSpeed) * amplitude;
    float peachPosY = Application::SCREEN_HALF_Y + offsetY;
    int peachPosX = -20;
    int wavePosY = 20;
    int oniSimaPosX = Application::SCREEN_HALF_X - 100;
    int oniSimaPosY = 30;
    const float PEACH_SCALE = 0.5f;
    const float WAVE_SCALE = 1.0f;
    const float ONISIMA_SCALE = 1.0f;
    auto* shaderPeach = ShaderManager::GetInstance().GetShaderNormal();
    auto* shaderOniSima = ShaderManager::GetInstance().GetShaderNormal();
    auto* shaderWave = ShaderManager::GetInstance().GetShaderWave();

    shaderOniSima->SetAmbient(0.8f);
    shaderOniSima->Draw(oniSimaPosX, oniSimaPosY, oniSimaHandle_, oniSimaNormalHandle_, ONISIMA_SCALE);


    shaderPeach->SetLightDirection(0.5f, 0.5f, 0.5f);
    shaderPeach->SetAmbient(0.8f);

    shaderWave->SetWaveParam(time_, 3.0f, 0.015f);

    ShaderManager::GetInstance().DrawNormalAndWave(0, wavePosY, 
        waveHandle_, waveNormalHandle_, WAVE_SCALE);

    shaderPeach->Draw(peachPosX, static_cast<int>(peachPosY),
        peachHandle_, peachNormalHandle_, PEACH_SCALE);

    const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 4;
    DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 0.7f, 0.0f, imageTitle_, true);

    const float DEFAULT_SCALE = 0.5f;
    using TAG_2D = Collider2DBase::TAG_2D;

    // 各ボタンの情報を配列にまとめる
    std::array<Collider2DBox*, 4> colliders = 
    {
        soloPlayButtonCollider_.get(),
        multiPlayButtonCollider_.get(),
        optionButtonCollider_.get(),
        exitButtonCollider_.get()
    };

    for (int i = 0; i < MENU_BUTTON_NUM; ++i)
    {
        // マウスホバー OR パッドで選択中
        bool isSelected = CollisionController::GetInstance().
            IsTagCollidingWithTag2D(TAG_2D::MOUSE_CURSOR, buttonTags[i])
            || (selectedIdx_ == i);

        // 選択状態に応じて画像インデックスを切り替え
        int imgIdx = isSelected ? (i * 2 + 1) : (i * 2);

        // 描画
        Vector2F pos = colliders[i]->GetCenterPos();
        DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), DEFAULT_SCALE,
            UtilityMath::DEG2RAD, imageMenu_[imgIdx], true);

    }

#ifdef _DEBUG
    DrawDebug();
#endif
}

void SceneTitle::Release(void)
{
    if (psHandle_ != -1)
    {
        DeleteShader(psHandle_);
        psHandle_ = -1;
    }
}

void SceneTitle::DrawDebug(void)
{
    CollisionController::GetInstance().DrawDebug2D();
}

