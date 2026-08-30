#include "SceneTitle.h"

#include <cmath>

#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Camera/Camera.h"
#include "SceneGame.h"
#include "SceneLobby.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Application.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Common/Loading.h"
#include "../../Utility/UtilityMath.h"
#include "../../Shader/ShaderController.h"
#include "../../ImGUI/GuiController.h"
#include "../SubScene/SceneOperation.h"
#include "../../Shader/ShaderParameters.h"

SceneTitle::SceneTitle(void)
    : imageTitle_(-1)
    , peachHandle_(-1)
    , peachNormalHandle_(-1)
    , waveHandle_(-1)
    , waveNormalHandle_(-1)
    , oniSimaHandle_(-1)
    , oniSimaNormalHandle_(-1)
    , backgroundHandle_(-1)
    , backNormalHandle_(-1)
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
    , isSelectMenu_(true)
    , inputIntervalCounter_(0)
{
    for (size_t i = 0; i < imageMenu_.size(); ++i)
    {
        imageMenu_[i] = -1;
    }
}

void SceneTitle::Load(void)
{
    // isLoading_ を true に
    SceneBase::Load();

    Loading::GetInstance()->SetProgress(10.0f);

    // BGM・SEロード
    SoundManager::GetInstance()
        .Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_SEA
            , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_SEA));
    SoundManager::GetInstance()
        .Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_THUNDER
            , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_THUNDER));

    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_UI_SELECT
        , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_UI_SELECT));

    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_UI_CANCEL
        , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_UI_CANCEL));
            
    // 音量調整

    // タイトル画像
    imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_TITLE);

    // タイトルのノーマルマップ画像
    titleNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_NOTMALMAP_TITLE);

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

    // 背景画像
    backgroundHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_BUCGROUND_TITLE);

    // 背景画像のノーマルマップ
    backNormalHandle_ = ResourceManager::GetInstance().
        LoadHandleId(ResourceManager::SRC::IMG_BUCG_NORMALMAP_TITLE);

    // その他画像

    //時間カウントリセット
    TimeManager::GetInstance().Reset();

}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

void SceneTitle::Initialize(void)
{

    if (Loading::GetInstance()->IsLoading()) { return; }

    SetMouseDispFlag(true);

    // マウスカーソル用のコライダー生成（半径1の円）
    cursorCollider_ = std::make_unique<Collider2DCircle>
        (Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);

    CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

    // UI初期化処理
    InitUI();

    SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);

    // メニュー選択有効化
    isSelectMenu_ = true;


    // BGM
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_SEA);
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);

    // 効果音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT
        , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_SELECT));
}

void SceneTitle::InitUI(void)
{
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

    buttonTags =
    {
        Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON,
        Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON,
        Collider2DBase::TAG_2D::OPTION_BUTTON,
        Collider2DBase::TAG_2D::EXIT_UTTON
    };
    
#ifdef _DEBUG

    //// 定数バッファの初期化
    //peachMaterial_.SetAmbient(0.8f);
    //waveMaterial_.SetAmbient(0.8f);
    //titleMaterial_.SetAmbient(0.8f);
    //titleMaterial_.SetLightDirection(0.0f, 0.0f, 0.0f);
    //titleMaterial_.SetWaveSpeed(0.0f);
    //titleMaterial_.SetWaveForce(0.0f);
    //oniSimaMaterial_.SetAmbient(0.8f);
    //waveMaterial_.SetWaveSpeed(3.0f);
    //waveMaterial_.SetWaveForce(0.015f);

    //// GUIの初期化
    //peachGui_ = std::make_shared<ShaderEditorComponent>("Peach", &peachMaterial_);
    //waveGui_ = std::make_shared<ShaderEditorComponent>("Wave", &waveMaterial_);
    //oniSimaGui_ = std::make_shared<ShaderEditorComponent>("OniGashima", &oniSimaMaterial_);
    //titleGui_ = std::make_shared<ShaderEditorComponent>("Title", &titleMaterial_);

    // 座標の初期化
    Vector2F peachPos = Vector2F(100.0f, Application::SCREEN_HALF_Y + 100);
    Vector2F wavePos = Vector2F(800.0f, Application::SCREEN_HALF_Y + 250);
    Vector2F oniSimaPos = Vector2F(Application::SCREEN_SIZE_X - 100.0f, Application::SCREEN_HALF_Y - 50);

    // 桃の当たり判定
    peachCollider_ = std::make_unique<Collider2DBox>(peachPos, 300.0f, 300.0f,
        Collider2DBase::TAG_2D::PEACH);

    // 波の当たり判定
    waveCollider_ = std::make_unique<Collider2DBox>(wavePos, Application::SCREEN_SIZE_X,
        Application::SCREEN_HALF_Y, Collider2DBase::TAG_2D::WAVE);

    // 鬼ヶ島の当たり判定
    oniSimaCollider_ = std::make_unique<Collider2DBox>(oniSimaPos, 200.0f,
        150.0f, Collider2DBase::TAG_2D::ONI_GASHIMA);

    CollisionController::GetInstance().RegisterCollider2D(peachCollider_.get());
    CollisionController::GetInstance().RegisterCollider2D(waveCollider_.get());
    CollisionController::GetInstance().RegisterCollider2D(oniSimaCollider_.get());

    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::PEACH, true);
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::WAVE, true);
    CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::ONI_GASHIMA, true);
        
#endif // _DEBUG

    // メニュー選択有効化
    isSelectMenu_ = true;


    // BGM
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_SEA);
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);

    // 効果音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT
        , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_SELECT));
}

void SceneTitle::Update(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

    auto& keyConfInputManager = KeyConfInputManager::GetInstance();

    // マウス座標の更新
    Vector2 mousePos = keyConfInputManager.GetMousePosition();
    Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    cursorCollider_->SetCenterPos(mousePosF);

#ifdef _DEBUG

    if (KeyConfInputManager::GetInstance().isTrigerDown("APPLY_DEBUG"))
    {
        isDebugMode_ = true;
    }

    

    if (isDebugMode_ == true)
    {

        if (KeyConfInputManager::GetInstance().isTrigerDown("UNAPPLY_DEBUG"))
        {
            isDebugMode_ = false;
        }

        UpdateGui();

        return; 
    }
#endif

    // スティック入力による選択インデックスの更新
    Vector2F stick = keyConfInputManager.GetLeftStickRaw();
    constexpr float THRESHOLD = 0.5f;
    constexpr int STICK_TINERVAL = 15;

    if (isSelectMenu_)
    {
        // インターバルのカウントダウン
        if (inputIntervalCounter_ > 0)
        {
            inputIntervalCounter_--;
        }

        // インターバルが0の時に入力を受け付ける
        if (inputIntervalCounter_ <= 0)
        {
            // 下入力
            if (keyConfInputManager.isPressed("DOWN"))
            {
                selectedIdx_ = (selectedIdx_ + 1) % MENU_BUTTON_NUM;

                // 次の連続入力までのインターバル
                const int STICK_INTERVAL = 15;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
            // 上入力
            else if (keyConfInputManager.isPressed("UP"))
            {
                selectedIdx_ = (selectedIdx_ - 1 + MENU_BUTTON_NUM) % MENU_BUTTON_NUM;

                // 次の連続入力までのインターバル
                const int STICK_INTERVAL = 15;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
        }

        // 上下の入力が全くない場合はインターバルをリセットする
        if (!keyConfInputManager.isPressed("DOWN") && !keyConfInputManager.isPressed("UP"))
        {
            inputIntervalCounter_ = 0;
        }


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
                isSelectMenu_ = false;

                // 効果音再生
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            }
        }
    }
    else
    {
        if (selectedIdx_ == static_cast<int>(MENU_ITEM::OPTION))
        {
            // 設定処理有効化
            SceneManager::GetInstance().PushScene(std::make_shared<SceneOperation>());

            // 選択変更有効化
            isSelectMenu_ = true;
        }
        else if (!SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_SELECT))
        {
            // 効果音終了時に決定処理を実行
            ProcessMenuState();
        }
    }

}

void SceneTitle::Draw(void)
{
    time_ += 0.02f;

    const float PEACH_SCALE = 0.5f;
    const float WAVE_SCALE = 1.0f;
    const float ONISIMA_SCALE = 1.0f;

    float amplitude = 20.0f;
    float bobSpeed = 3.0f;
    float offsetY = std::sinf(time_ * bobSpeed) * amplitude;

    auto& shaderCtrl = ShaderController::GetInstance();

    // 背景のパラメータ設定と描画
    NormalWaveShaderParams backgroundParams_;
    backgroundParams_.lightVectorX = 0.5f;
    backgroundParams_.lightVectorY = 0.5f;
    backgroundParams_.lightVectorZ = 0.5f;
    backgroundParams_.ambientRate = 0.8f;
    backgroundParams_.timeValue = 0;
    backgroundParams_.useNormalMapFlag = 1.0f;

    shaderCtrl.Draw2D(
        ResourceManager::SRC::PS_NORMAL_MAP,
        0.0f, 0.0f,
        1.0f,
        backgroundParams_,
        backgroundHandle_,
        backNormalHandle_
    );

    // 鬼ヶ島のパラメータ設定と描画
    NormalWaveShaderParams oniSimaParams_;
    oniSimaParams_.lightVectorX = 0.5f;
    oniSimaParams_.lightVectorY = 0.5f;
    oniSimaParams_.lightVectorZ = 0.5f;
    oniSimaParams_.ambientRate = 0.8f;
    oniSimaParams_.timeValue = 0;
    oniSimaParams_.useNormalMapFlag = 1.0f;

    shaderCtrl.Draw2D(
        ResourceManager::SRC::PS_NORMAL_MAP,
        static_cast<int>(Application::SCREEN_HALF_X - 100), 30,
        ONISIMA_SCALE,
        oniSimaParams_,
        oniSimaHandle_,
        oniSimaNormalHandle_
    );

    // 波のパラメータ設定と描画
    NormalWaveShaderParams waveParams_;
    waveParams_.lightVectorX = 0.5f;
    waveParams_.lightVectorY = 0.5f;
    waveParams_.lightVectorZ = 0.5f;
    waveParams_.ambientRate = 0.8f;
    waveParams_.timeValue = time_;
    waveParams_.waveSpeedValue = 3.0f;
    waveParams_.waveForceValue = 0.015f;
    waveParams_.useNormalMapFlag = 1.0f;

    shaderCtrl.Draw2D(
        ResourceManager::SRC::PS_NORMAL_MAP,
        0, 20,
        WAVE_SCALE,
        waveParams_,
        waveHandle_,
        waveNormalHandle_
    );

    // 桃のパラメータ設定と描画
    NormalWaveShaderParams peachParams_;
    peachParams_.lightVectorX = 0.5f;
    peachParams_.lightVectorY = 0.5f;
    peachParams_.lightVectorZ = 0.5f;
    peachParams_.ambientRate = 0.8f;
    peachParams_.timeValue = time_;
    peachParams_.useNormalMapFlag = 1.0f;

    shaderCtrl.Draw2D(
        ResourceManager::SRC::PS_NORMAL_MAP,
        -20, static_cast<int>(Application::SCREEN_HALF_Y + offsetY),
        PEACH_SCALE,
        peachParams_,
        peachHandle_,
        peachNormalHandle_
    );

    const int IMAGE_TITLE_Y = 0;
    const int IMAGE_TITLE_X = (Application::SCREEN_HALF_X / 2) + 20;

    // タイトル画像のパラメータ設定と描画
    NormalWaveShaderParams titleParams_;
    titleParams_.lightVectorX = 0.0f;
    titleParams_.lightVectorY = 0.0f;
    titleParams_.lightVectorZ = 0.0f;
    titleParams_.ambientRate = 0.8f;
    titleParams_.timeValue = time_;
    titleParams_.waveSpeedValue = 0.0f;
    titleParams_.waveForceValue = 0.0f;
    titleParams_.useNormalMapFlag = 1.0f;

    shaderCtrl.Draw2D(
        ResourceManager::SRC::PS_NORMAL_MAP,
        IMAGE_TITLE_X, IMAGE_TITLE_Y,
        PEACH_SCALE,
        titleParams_,
        imageTitle_,
        titleNormalHandle_
    );

    // キューに溜まった描画命令を一括ソート＆実行
    shaderCtrl.ExecuteDrawCommands();

    const float DEFAULT_SCALE = 0.5f;
    using TAG_2D = Collider2DBase::TAG_2D;

    std::array<Collider2DBox*, 4> colliders =
    {
        soloPlayButtonCollider_.get(),
        multiPlayButtonCollider_.get(),
        optionButtonCollider_.get(),
        exitButtonCollider_.get()
    };

    for (int i = 0; i < MENU_BUTTON_NUM; ++i)
    {
        bool isSelected = CollisionController::GetInstance().
            IsTagCollidingWithTag2D(TAG_2D::MOUSE_CURSOR, buttonTags[i])
            || (selectedIdx_ == i);

        int imgIdx = isSelected ? (i * 2 + 1) : (i * 2);

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
}


void SceneTitle::DrawDebug(void)
{
    CollisionController::GetInstance().DrawDebug2D();
}

void SceneTitle::ProcessMenuState(void)
{
    switch (static_cast<MENU_ITEM>(selectedIdx_))
    {
        case MENU_ITEM::SOLO:
        {
           /* std::vector<SceneGame::PlayerSelectType> playerSelectTypes;
            SceneGame::PlayerSelectType myType;
            myType.job = PlayerBase::JOB_TYPE::BOMB;
            myType.skin = PlayerBase::SKIN_TYPE::DOG;
            playerSelectTypes.push_back(myType);
            SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(playerSelectTypes));*/
            SceneManager::GetInstance()
                .ChangeScene(std::make_shared<SceneLobby>(false));
        }
        break;

        case MENU_ITEM::MULTI:
            // マルチプレイ処理
            SceneManager::GetInstance().ChangeScene(std::make_shared<SceneLobby>(true));

        break;

        case MENU_ITEM::OPTION:
            SceneManager::GetInstance().PushScene(std::make_shared<SceneOperation>());
            break;

        case MENU_ITEM::EXIT:

            Application::GetInstance().GameEnd();
        break;
    }
}

void SceneTitle::UpdateGui(void)
{
    // クリックされたら、対象のGUIをコントローラに渡す
    if (KeyConfInputManager::GetInstance().isTrigerDown("OK"))
    {
        auto& colCtrl = CollisionController::GetInstance();
        using TAG = Collider2DBase::TAG_2D;

        if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::PEACH)) {
            GuiController::GetInstance().SetActiveGui(peachGui_);
        }
        else if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::WAVE)) {
            GuiController::GetInstance().SetActiveGui(waveGui_);
        }
        else if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::ONI_GASHIMA)) {
            GuiController::GetInstance().SetActiveGui(oniSimaGui_);
        }
    }
}