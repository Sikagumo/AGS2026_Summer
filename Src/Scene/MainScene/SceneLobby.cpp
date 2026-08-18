#include "SceneLobby.h"

#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/NetManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Camera/Camera.h"
#include "./SceneGame.h"
#include "../../Common/Loading.h"
#include "../SubScene/ScenePause.h"

SceneLobby::SceneLobby(bool _isMulti)
    : SceneBase::SceneBase()
    , IS_MULTI(_isMulti)
    , isSelectMenu_(true)
    , selectedJobIndex_(0)
    , selectedSkinIndex_(0)
    , cursorCollider_(nullptr)
    , inputIntervalCounter_(0)
    , multiState_(LOBBY_STATE::SELECT_MODE)
    , buttonSelectIndex_(0)
    , passcode_{ 0, 0, 0, 0 }
    , selectOctet_(0)
    , isEditing_(false)
    , myReadyState_(false)
    , previewModelHandle_(-1)
    , currentModelIndex_(-1)
    , mainSelectIndex_(0)
    , selectState_(SELECT_STATE::MAIN)
    , backgroundHandle_(-1)
    , roomBackHandle_(-1)
    , selectUIBackHandle_(-1)
    , uiBackHeight_(0)
    , uiBackWidth_(0)
    , selectedUIBackHandle_(-1)
    , selectMultiHandle_(-1)
    , selectedMultiHandle_(-1)
    , multiTitleHandle_(-1)
    , connectTexHandle_(-1)
    , allReadyImageHandle_(-1)
    ,leaveRoomTextHandle_(-1)
    , isLeaveWindow_(false)
    , leaveSelectIndex_(1)
{
}

SceneLobby::~SceneLobby(void)
{
}

void SceneLobby::Load(void)
{
    ResourceManager::GetInstance()
        .LoadHandleIds(ResourceManager::SRC::IMGS_SELECT, uiHandles_.data());

    // 選択シーンの背景
    backgroundHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_BACKGROUND_SELCET);

    // マルチマッチング背景
    roomBackHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_BACKGROUND_MALUTI);

    // UIの背景
    selectUIBackHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_SELECT_UI_BACK);

    // 選択中UIの背景
    selectedUIBackHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_SELECTED_UI_BACK);

    // マルチ選択中UIの背景
    selectMultiHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_SELECT_MULTI_BACK);

    // マルチ選択中UIの背景
    selectedMultiHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_SELECTED_MULTI_BACK);

    leaveRoomTextHandle_ = ResourceManager::GetInstance()
        .LoadHandleId(ResourceManager::SRC::IMG_TEXT_LEAVE_ROOM);

    std::array<int, 4> tempSelectHandles;
    std::array<int, 4> tempNormalHandles;

    ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_SELECT_PUSE_TEX, tempSelectHandles.data());
    ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_POUSE_TEX, tempNormalHandles.data());

    selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES)) = tempSelectHandles.at(2);
    selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO)) = tempSelectHandles.at(3);

    noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES)) = tempNormalHandles.at(2);
    noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO)) = tempNormalHandles.at(3);

    multiTitleHandle_ = ResourceManager::GetInstance().
        LoadHandleIdsOnce(ResourceManager::SRC::IMGS_CONECT_TEX, 1);

    connectTexHandle_ = ResourceManager::GetInstance().
        LoadHandleIdsOnce(ResourceManager::SRC::IMGS_CONECT_TEX, 0);

    allReadyImageHandle_ = ResourceManager::GetInstance().
        LoadHandleIdsOnce(ResourceManager::SRC::IMGS_MULTI_TEX, 4);

    ResourceManager::GetInstance()
        .LoadHandleIds(ResourceManager::SRC::IMGS_LOBBY_UI_TEX, uiTexHandles_.data());

    ResourceManager::GetInstance().
        LoadHandleIds(ResourceManager::SRC::IMGS_TIMER, passcodeTextHandles_.data());

    ResourceManager::GetInstance().
        LoadHandleIds(ResourceManager::SRC::IMGS_MULTI_TEX, uiRadyHandles_.data());

    SoundManager::GetInstance() .Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_LOBBY_SELECT
            , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_LOBBY_SELCET));

    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_LOBBY
        , ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_LOBBY));

    lobbySkinHandles_.at(0) = ResourceManager::GetInstance().
        LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_HUMAN);

    lobbySkinHandles_.at(1) = ResourceManager::GetInstance().
        LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_DOG);

    lobbySkinHandles_.at(2) = ResourceManager::GetInstance().
        LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_MONKEY);

    lobbySkinHandles_.at(3) = ResourceManager::GetInstance().
        LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_BIRD);

    int idleAnimHandle = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_IDLE);
    constexpr int ANIM_IDLE = 0;
    constexpr float SPEED_IDLE = 30.0f;

    for (size_t i = 0; i < lobbySkinHandles_.size(); ++i)
    {
        if (lobbySkinHandles_.at(i) != -1)
        {
            // コントローラーを生成
            lobbyAnimControllers_.at(i) = std::make_unique<AnimationController>(lobbySkinHandles_.at(i));

            // 待機アニメーションを追加してループ再生
            lobbyAnimControllers_.at(i)->AddExternal(ANIM_IDLE, idleAnimHandle, SPEED_IDLE);
            lobbyAnimControllers_.at(i)->Play(ANIM_IDLE, true);
        }
    }

}

void SceneLobby::EndLoad(void)
{
}

void SceneLobby::Initialize(void)
{
    NetManager::GetInstance().Stop();

    selectedJobIndex_ = 0;
    selectedSkinIndex_ = 0;

    GetGraphSize(selectUIBackHandle_, &uiBackWidth_, &uiBackHeight_);

    SetMouseDispFlag(true);

    // カーソル用コライダー共通登録
    cursorCollider_ = std::make_unique<Collider2DCircle>
        (Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);
    CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

    // シングル・マルチ共通で装備選択UIを初期化
    InitUISingle();

    if (IS_MULTI)
    {
        multiState_ = LOBBY_STATE::SELECT_EQUIPMENT;
        passcode_[0] = 0;
        passcode_[1] = 0;
        passcode_[2] = 0;
        passcode_[3] = 0;
        buttonSelectIndex_ = static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON);
        isEditing_ = false;

        InitUIMulti();
    }

    SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);

    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_LOBBY);

    // 初回のモデル読み込み
    UpdatePreviewModel();
}

void SceneLobby::InitUISingle(void)
{
    // メインUI位置関連
    constexpr float MAIN_PANEL_WIDTH = 300.0f;
    constexpr float MAIN_PANEL_HEIGHT = 450.0f;
    constexpr float MAIN_PANEL_OFFSET_X = 500.0f;

    // メインUI用コライダー生成
    Vector2F leftPanelPosition
    (
        static_cast<float>(Application::SCREEN_HALF_X) - MAIN_PANEL_OFFSET_X,
        static_cast<float>(Application::SCREEN_HALF_Y)
    );

    Vector2F rightPanelPosition
    (
        static_cast<float>(Application::SCREEN_HALF_X) + MAIN_PANEL_OFFSET_X,
        static_cast<float>(Application::SCREEN_HALF_Y)
    );

    Vector2F readyButtonPos
    (
        static_cast<float>(Application::SCREEN_HALF_X) + 350.0f,
        static_cast<float>(Application::SCREEN_SIZE_Y) - 100.0f
    );

    // それぞれの役割に応じたタグを指定して生成
    mainUiCollisions_.at(0) = std::make_unique<Collider2DBox>(
        leftPanelPosition, MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT, Collider2DBase::TAG_2D::UI_MAIN_WEAPON
    );

    mainUiCollisions_.at(1) = std::make_unique<Collider2DBox>(
        rightPanelPosition, MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT, Collider2DBase::TAG_2D::UI_MAIN_SKIN
    );

    readyButtonCollision_ = std::make_unique<Collider2DBox>(
        readyButtonPos, 500.0f, 150.0f, Collider2DBase::TAG_2D::UI_MAIN_READY
    );

    // ウィンドウ位置関連
    constexpr float WINDOW_WIDTH = 500.0f;
    constexpr float ITEM_HEIGHT = 80.0f;
    constexpr float ITEM_INTERVAL_Y = 150.0f;

    const float windowTopY = static_cast<float>(Application::SCREEN_HALF_Y) - 210.0f;

    // 武器ミニウィンドウ内のコライダー生成
    constexpr int jobMaximum = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    for (int jobIndex = 0; jobIndex < jobMaximum; ++jobIndex)
    {
        float currentItemPositionY = windowTopY + (jobIndex * ITEM_INTERVAL_Y);
        Vector2F itemCenterPosition(static_cast<float>(Application::SCREEN_HALF_X), currentItemPositionY);

        weaponUiCollisions_.at(static_cast<size_t>(jobIndex)) = std::make_unique<Collider2DBox>(
            itemCenterPosition, WINDOW_WIDTH, ITEM_HEIGHT, Collider2DBase::TAG_2D::UI_WINDOW_WEAPON
        );
    }

    // スキンミニウィンドウ内のコライダー生成
    constexpr int skinMaximum = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

    for (int skinIndex = 0; skinIndex < skinMaximum; ++skinIndex)
    {
        float currentItemPositionY = windowTopY + (skinIndex * ITEM_INTERVAL_Y);
        Vector2F itemCenterPosition(static_cast<float>(Application::SCREEN_HALF_X), currentItemPositionY);

        skinUiCollisions_.at(static_cast<size_t>(skinIndex)) = std::make_unique<Collider2DBox>(
            itemCenterPosition, WINDOW_WIDTH, ITEM_HEIGHT, Collider2DBase::TAG_2D::UI_WINDOW_SKIN
        );
    }

    // 生成したコライダーを CollisionController に登録
    auto& collisionController = CollisionController::GetInstance();

    for (auto& collider : mainUiCollisions_)
    {
        if (collider != nullptr)
        {
            collisionController.RegisterCollider2D(collider.get());
        }
    }

    for (auto& collider : weaponUiCollisions_)
    {
        if (collider != nullptr)
        {
            collisionController.RegisterCollider2D(collider.get());
        }
    }

    for (auto& collider : skinUiCollisions_)
    {
        if (collider != nullptr)
        {
            collisionController.RegisterCollider2D(collider.get());
        }
    }

    if (readyButtonCollision_ != nullptr)
    {
        collisionController.RegisterCollider2D(readyButtonCollision_.get());
    }
}

void SceneLobby::InitUIMulti(void)
{
    const int ipBoxX = (Application::SCREEN_SIZE_X - 400) / 2;

    // パスコード入力ボックスのコライダー
    const Vector2F passcodePosition(static_cast<float>(ipBoxX + 200), 280.0f);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::PASSCODE_BOX))
        = std::make_unique<Collider2DBox>(passcodePosition, 400.0f, 60.0f, Collider2DBase::TAG_2D::NONE);

    // 背景UI画像の当たり判定サイズ
    const float colliderWidth = static_cast<float>(uiBackWidth_) * 0.2f;
    const float colliderHeight = static_cast<float>(uiBackHeight_) * 0.2f;

    // HOSTボタンのコライダー
    const Vector2F hostPosition(Application::SCREEN_HALF_X, HOST_BUTTON_Y);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::HOST_BUTTON))
        = std::make_unique<Collider2DBox>(hostPosition, colliderWidth, colliderHeight, Collider2DBase::TAG_2D::NONE);

    // CLIENTボタンのコライダー
    const Vector2F clientPosition(Application::SCREEN_HALF_X, CLIENT_BUTTON_Y);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::CLIENT_BUTTON))
        = std::make_unique<Collider2DBox>(clientPosition, colliderWidth, colliderHeight, Collider2DBase::TAG_2D::NONE);

    for (auto& collider : multiUiCollisions_)
    {
        if (collider != nullptr)
        {
            CollisionController::GetInstance().RegisterCollider2D(collider.get());
        }
    }

    const float buttonColliderWidth = static_cast<float>(uiBackWidth_) * 0.4f;
    const float buttonColliderHeight = static_cast<float>(uiBackHeight_) * 0.4f;

    Vector2F yesPosition(static_cast<float>(Application::SCREEN_HALF_X) - 150.0f, static_cast<float>(Application::SCREEN_HALF_Y) + 80.0f);
    leaveYesCollider_ = std::make_unique<Collider2DBox>(yesPosition, buttonColliderWidth, buttonColliderHeight, Collider2DBase::TAG_2D::NONE);

    Vector2F noPosition(static_cast<float>(Application::SCREEN_HALF_X) + 150.0f, static_cast<float>(Application::SCREEN_HALF_Y) + 80.0f);
    leaveNoCollider_ = std::make_unique<Collider2DBox>(noPosition, buttonColliderWidth, buttonColliderHeight, Collider2DBase::TAG_2D::NONE);

    CollisionController::GetInstance().RegisterCollider2D(leaveYesCollider_.get());
    CollisionController::GetInstance().RegisterCollider2D(leaveNoCollider_.get());
}

void SceneLobby::Update(void)
{
    if (Loading::GetInstance()->IsLoading())
    {
        return;
    }

    // マウス位置を常にコライダーへ追従させる
    Vector2 mousePosition = KeyConfInputManager::GetInstance().GetMousePosition();
    Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    if (cursorCollider_ != nullptr)
    {
        cursorCollider_->SetCenterPos(mousePositionF);
    }

    for (auto& animController : lobbyAnimControllers_)
    {
        if (animController != nullptr)
        {
            animController->Update();
        }
    }

    if (IS_MULTI)
    {
        UpdateMulti();
    }
    else
    {
        UpdateSingle();
    }

    // モデル選択の変更チェック
    UpdatePreviewModel();

    // 3Dモデルの位置・姿勢を設定
    if (previewModelHandle_ != -1)
    {
        // 配置位置
        VECTOR modelPos = VGet(0.0f, -250.0f, 0.0f);
        MV1SetPosition(previewModelHandle_, modelPos);

        // スケール調整
        MV1SetScale(previewModelHandle_, VGet(5.0f, 5.0f, 5.0f));

        // 自動Y軸回転
        static float rotY = 0.0f;
        rotY += 0.01f;
        MV1SetRotationXYZ(previewModelHandle_, VGet(0.0f, rotY, 0.0f));

        if (animController_ != nullptr)
        {
            animController_->Update();
        }
    }

}

void SceneLobby::Draw(void)
{
    DrawExtendGraph(
        0, 0,
        Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
        backgroundHandle_, false
    );

    if (IS_MULTI)
    {
        DrawMulti();
    }
    else
    {
        DrawCenterModel();

        const int startButtonHandle = (mainSelectIndex_ == 2)
            ? selectedUIBackHandle_ : selectUIBackHandle_;

        const Vector2 startButtonPosition =
        {
            Application::SCREEN_HALF_X + 350, Application::SCREEN_SIZE_Y - 100
        };

        // ボタン背景描画
        DrawRotaGraph3(startButtonPosition.x, startButtonPosition.y, uiBackWidth_ / 2,
            uiBackHeight_ / 2, 0.4f, 0.3f, 0.0f, startButtonHandle, true);

        // ボタンテキスト描画
        DrawRotaGraph(startButtonPosition.x, startButtonPosition.y, 0.6f, 0.0f,
            uiHandles_.at(static_cast<int>(UI_SINGLE::GAME_START)), true);

        DrawSelectionPanels();
    }

#ifdef _DEBUG
    //CollisionController::GetInstance().DrawDebug2D();
#endif
}

void SceneLobby::Release(void)
{

}

void SceneLobby::DrawCenterModel(void)
{
    if (previewModelHandle_ != -1)
    {
        MV1DrawModel(previewModelHandle_);
    }
}

void SceneLobby::DrawSelectionPanels(void)
{
    int leftPanelHandle = (mainSelectIndex_ == 0) ? selectedUIBackHandle_ : selectUIBackHandle_;
    int rightPanelHandle = (mainSelectIndex_ == 1) ? selectedUIBackHandle_ : selectUIBackHandle_;
    const float PANELTEX_SCCALE = 0.6f;

    // 左パネルを描画
    DrawRotaGraph3(LEFT_PANEL_X, PANEL_START_Y, uiBackWidth_ / 2, uiBackHeight_ / 2,
        BUTTON_SCALE, 0.8f, 0.0, leftPanelHandle, true);

    // 右パネルを描画
    DrawRotaGraph3(RIGHT_PANEL_X, PANEL_START_Y, uiBackWidth_ / 2, uiBackHeight_ / 2,
        BUTTON_SCALE, 0.8f, 0.0, rightPanelHandle, true);

    // 左パネル文字を描画
    DrawRotaGraph(LEFT_PANEL_X, PANEL_START_Y, PANELTEX_SCCALE, 0.0f,
        uiTexHandles_.at(static_cast<int>(UI_MAIN_TEXT::WEAPON)), true);

    // 右パネル文字を描画
    DrawRotaGraph(RIGHT_PANEL_X, PANEL_START_Y, PANELTEX_SCCALE, 0.0,
        uiTexHandles_.at(static_cast<int>(UI_MAIN_TEXT::SKIN)), true);
      

    // ミニウィンドウが開いている場合は暗めの半透明フィルターを表示
    if (selectState_ != SELECT_STATE::MAIN)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawBox(
            0, 0,
            Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
            GetColor(0, 0, 0), true
        );
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // 各ミニウィンドウの描画
    if (selectState_ == SELECT_STATE::WEAPON_WINDOW)
    {
        // 画面中央に武器選択ウィンドウを描画
        DrawWeaponWindow();
    }
    else if (selectState_ == SELECT_STATE::SKIN_WINDOW)
    {
        // 画面中央にスキン選択ウィンドウを描画
        DrawSkinWindow();
    }
}

void SceneLobby::UpdateSingle(void)
{
    auto& keyConfInputManager = KeyConfInputManager::GetInstance();
    auto& collisionController = CollisionController::GetInstance();

    Vector2F stick = keyConfInputManager.GetLeftStickRaw();
    constexpr float THRESHOLD = 0.5f;
    constexpr int STICK_INTERVAL = 15;

    if (KeyConfInputManager::GetInstance().isTrigerDown("PAUSE"))
    {
        SceneManager::GetInstance().PushScene(std::make_shared<ScenePause>());
    }

    // スティック用のインターバルタイマー更新
    if (inputIntervalCounter_ > 0)
    {
        inputIntervalCounter_--;
    }

    if (!keyConfInputManager.isPressed("UP") && !keyConfInputManager.isPressed("DOWN") &&
        !keyConfInputManager.isPressed("LEFT") && !keyConfInputManager.isPressed("RIGHT"))
    {
        inputIntervalCounter_ = 0;
    }

    switch (selectState_)
    {
    case SELECT_STATE::MAIN:
    {
        // マウスカーソルと左右メインUIの当たり判定
        bool isHoverWeapon = false;
        bool isHoverSkin = false;
        bool isHoverReady = false;

        if (cursorCollider_ != nullptr)
        {
            if (mainUiCollisions_.at(0) != nullptr)
            {
                isHoverWeapon = collisionController.CheckCollision2D(
                    cursorCollider_.get(),
                    mainUiCollisions_.at(0).get()
                );
            }

            if (mainUiCollisions_.at(1) != nullptr)
            {
                isHoverSkin = collisionController.CheckCollision2D(
                    cursorCollider_.get(),
                    mainUiCollisions_.at(1).get()
                );
            }
            
            if (readyButtonCollision_ != nullptr)
            {
                isHoverReady = collisionController.CheckCollision2D(
                    cursorCollider_.get(),
                    readyButtonCollision_.get()
                );
            }
        }

        // マウスホバー位置による選択インデックスの更新
        if (isHoverWeapon)
        {
            // 武器
            mainSelectIndex_ = 0; 
        }
        else if (isHoverSkin)
        {
            // 見た目
            mainSelectIndex_ = 1; 
        }
        else if (isHoverReady)
        {
            // 準備完了
            mainSelectIndex_ = 2; 
        }

        // スティック/キーボード入力でのUI選択切り替え
        if (inputIntervalCounter_ == 0)
        {
            // 左右の入力武器と見た目を切り替える
            if (keyConfInputManager.isPressed("RIGHT"))
            {
                if (mainSelectIndex_ == 0)
                {
                    mainSelectIndex_ = 1;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
                else if (mainSelectIndex_ == 1)
                {
                    mainSelectIndex_ = 0;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
            }
            else if (keyConfInputManager.isPressed("LEFT"))
            {
                if (mainSelectIndex_ == 1)
                {
                    mainSelectIndex_ = 0;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
                else if (mainSelectIndex_ == 0)
                {
                    mainSelectIndex_ = 1;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
            }
            // 下の入力準備完了へ移動する
            else if (keyConfInputManager.isPressed("DOWN"))
            {
                if (mainSelectIndex_ == 0 || mainSelectIndex_ == 1)
                {
                    mainSelectIndex_ = 2;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
            }
            // 上の入力準備完了 から 武器へ戻る
            else if (keyConfInputManager.isPressed("UP"))
            {
                if (mainSelectIndex_ == 2)
                {
                    mainSelectIndex_ = 0;
                    inputIntervalCounter_ = STICK_INTERVAL;
                }
            }
        }


        // 決定入力時の処理
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            if (mainSelectIndex_ == 0)
            {
                selectState_ = SELECT_STATE::WEAPON_WINDOW;
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
                return;
            }
            else if (mainSelectIndex_ == 1)
            {
                selectState_ = SELECT_STATE::SKIN_WINDOW;
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
                return;
            }
            else if (mainSelectIndex_ == 2)
            {
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);

                if (IS_MULTI)
                {
                    // マルチプレイの場合：モード選択状態へ遷移
                    multiState_ = LOBBY_STATE::SELECT_MODE;
                    buttonSelectIndex_ = static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON);
                }
                else
                {
                    // シングルプレイの場合：そのままゲームシーンへ遷移
                    auto selectedJob = static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_);
                    auto selectedSkin = static_cast<PlayerBase::SKIN_TYPE>(selectedSkinIndex_);

                    auto jobs = { SceneGame::PlayerSelectType(selectedJob, selectedSkin) };
                    SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(jobs));
                }
                return;
            }
        }
        break;
    }

case SELECT_STATE::WEAPON_WINDOW:
{
    constexpr int jobMaximum = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    // テーブルを使用した上下入力で武器選択移動
    if (inputIntervalCounter_ == 0)
    {
        if (keyConfInputManager.isPressed("DOWN"))
        {
            selectedJobIndex_ = (selectedJobIndex_ + 1) % jobMaximum;
            inputIntervalCounter_ = STICK_INTERVAL;
        }
        else if (keyConfInputManager.isPressed("UP"))
        {
            selectedJobIndex_ = (selectedJobIndex_ - 1 + jobMaximum) % jobMaximum;
            inputIntervalCounter_ = STICK_INTERVAL;
        }
    }

    // 画面中央武器ミニウィンドウ内のコライダー判定
    for (int jobIndex = 0; jobIndex < jobMaximum; ++jobIndex)
    {
        auto& weaponCollider = weaponUiCollisions_.at(static_cast<size_t>(jobIndex));

        // ヌルチェック
        if (cursorCollider_ != nullptr && weaponCollider != nullptr)
        {
            bool isHoverItem = collisionController.CheckCollision2D(
                cursorCollider_.get(),
                weaponCollider.get()
            );

            if (isHoverItem)
            {
                selectedJobIndex_ = jobIndex;

                if (keyConfInputManager.isTrigerDown("OK"))
                {
                    selectState_ = SELECT_STATE::MAIN;
                    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
                    SetJobToSKin();
                    return;
                }
            }
        }
    }

    // パッド/キーボードでの「OK」決定
    if (keyConfInputManager.isTrigerDown("OK"))
    {
        selectState_ = SELECT_STATE::MAIN;
        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
        SetJobToSKin();
        return;
    }

    // キャンセル入力でウィンドウを閉じる
    if (keyConfInputManager.isTrigerDown("CANCEL"))
    {
        selectState_ = SELECT_STATE::MAIN;
        return;
    }
    break;
}

case SELECT_STATE::SKIN_WINDOW:
{
    constexpr int skinMaximum = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

    // テーブルを使用した上下入力でスキン選択移動
    if (inputIntervalCounter_ == 0)
    {
        if (keyConfInputManager.isPressed("DOWN"))
        {
            selectedSkinIndex_ = (selectedSkinIndex_ + 1) % skinMaximum;
            inputIntervalCounter_ = STICK_INTERVAL;
        }
        else if (keyConfInputManager.isPressed("UP"))
        {
            selectedSkinIndex_ = (selectedSkinIndex_ - 1 + skinMaximum) % skinMaximum;
            inputIntervalCounter_ = STICK_INTERVAL;
        }
    }

    // 画面中央スキンミニウィンドウ内のコライダー判定
    for (int skinIndex = 0; skinIndex < skinMaximum; ++skinIndex)
    {
        auto& skinCollider = skinUiCollisions_.at(static_cast<size_t>(skinIndex));

        // ヌルチェック
        if (cursorCollider_ != nullptr && skinCollider != nullptr)
        {
            bool isHoverItem = collisionController.CheckCollision2D(
                cursorCollider_.get(),
                skinCollider.get()
            );

            if (isHoverItem)
            {
                selectedSkinIndex_ = skinIndex;

                if (keyConfInputManager.isTrigerDown("OK"))
                {
                    selectState_ = SELECT_STATE::MAIN;
                    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
                    SetJobToSKin();
                    return;
                }
            }
        }
    }

    // パッド/キーボードでのOK決定
    if (keyConfInputManager.isTrigerDown("OK"))
    {
        selectState_ = SELECT_STATE::MAIN;
        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
        SetJobToSKin();
        return;
    }

    // キャンセル入力でウィンドウを閉じる
    if (keyConfInputManager.isTrigerDown("CANCEL"))
    {
        selectState_ = SELECT_STATE::MAIN;
        return;
    }
    break;
}

default:
    break;
    } 
}

void SceneLobby::UpdatePreviewModel(void)
{
    // インデックスが変わっていなければ処理しない
    if (currentModelIndex_ == selectedSkinIndex_)
    {
        return;
    }

    // 前のモデルがあればメモリ解放
    if (previewModelHandle_ != -1)
    {
        MV1DeleteModel(previewModelHandle_);
        previewModelHandle_ = -1;
        animController_.reset();
    }

    currentModelIndex_ = selectedSkinIndex_;

    // 選択されたモデルインデックスに応じてリソース識別子を選択
    ResourceManager::SRC modelSrc = ResourceManager::SRC::NONE;

    switch (currentModelIndex_)
    {
    case 0:
        modelSrc = ResourceManager::SRC::MODEL_PLAYER_HUMAN;
        break;
    case 1:
        modelSrc = ResourceManager::SRC::MODEL_PLAYER_DOG;
        break;
    case 2:
        modelSrc = ResourceManager::SRC::MODEL_PLAYER_MONKEY;
        break;
    case 3:
        modelSrc = ResourceManager::SRC::MODEL_PLAYER_BIRD;
        break;
    default:
        break;
    }

    // 該当するモデルがあれば複製して保持
    if (modelSrc != ResourceManager::SRC::NONE)
    {
        previewModelHandle_ = ResourceManager::GetInstance().LoadModelDuplicate(modelSrc);

        // 部アニメーションとして待機モーションを設定
        if (previewModelHandle_ != -1)
        {
            animController_ = std::make_unique<AnimationController>(previewModelHandle_);

            // IDLEアニメーションのハンドルを取得
            int idleAnimHandle = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_IDLE);

            constexpr int ANIM_IDLE = 0; 
            constexpr float SPEED_IDLE = 30.0f;

            // 外部ファイルから読み込んだアニメーションを追加
            animController_->AddExternal(ANIM_IDLE, idleAnimHandle, SPEED_IDLE);

            // ループ再生
            animController_->Play(ANIM_IDLE, true);
        }
    }
}

void SceneLobby::UpdateMulti(void)
{
    switch (multiState_)
    {
    case SceneLobby::LOBBY_STATE::SELECT_EQUIPMENT:
        UpdateSingle();
        break;

    case SceneLobby::LOBBY_STATE::SELECT_MODE:
        UpdateSelectMode();
        break;

    case SceneLobby::LOBBY_STATE::CONNECTING:
        UpdateConnecting();
        break;

    case SceneLobby::LOBBY_STATE::IN_ROOM:
        UpdateInRoom();
        break;

    default:
        break;
    }
}

void SceneLobby::UpdateSelectMode(void)
{
    auto& keyConfInputManager = KeyConfInputManager::GetInstance();
    auto& collisionController = CollisionController::GetInstance();

    constexpr int STICK_INTERVAL = 15;

    // スティック入力インターバルタイマーの更新
    if (inputIntervalCounter_ > 0)
    {
        inputIntervalCounter_--;
    }

    // 操作が何もない場合はインターバルをリセット
    if (!keyConfInputManager.isPressed("UP") && !keyConfInputManager.isPressed("DOWN") &&
        !keyConfInputManager.isPressed("LEFT") && !keyConfInputManager.isPressed("RIGHT"))
    {
        inputIntervalCounter_ = 0;
    }

    // パスコード編集中の操作
    if (isEditing_)
    {
        if (inputIntervalCounter_ == 0)
        {
            // 左右入力で編集桁の移動
            if (keyConfInputManager.isPressed("RIGHT"))
            {
                selectOctet_ = (selectOctet_ + 1) % 4;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
            else if (keyConfInputManager.isPressed("LEFT"))
            {
                selectOctet_ = (selectOctet_ - 1 + 4) % 4;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
            // 上下入力で数値の変更
            else if (keyConfInputManager.isPressed("UP"))
            {
                passcode_[selectOctet_] = (passcode_[selectOctet_] + 1) % 10;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
            else if (keyConfInputManager.isPressed("DOWN"))
            {
                passcode_[selectOctet_] = (passcode_[selectOctet_] - 1 + 10) % 10;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
        }

        // OK または CANCEL で編集終了
        if (keyConfInputManager.isTrigerDown("OK") || keyConfInputManager.isTrigerDown("CANCEL"))
        {
            isEditing_ = false;
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
            return;
        }

        return;
    }

    // パッド/キーボードでの上下選択移動
    bool isInputted = false;
    if (inputIntervalCounter_ == 0)
    {
        if (keyConfInputManager.isPressed("UP"))
        {
            buttonSelectIndex_ = (buttonSelectIndex_ - 1 + 3) % 3;
            inputIntervalCounter_ = STICK_INTERVAL;
            isInputted = true;
        }
        else if (keyConfInputManager.isPressed("DOWN"))
        {
            buttonSelectIndex_ = (buttonSelectIndex_ + 1) % 3;
            inputIntervalCounter_ = STICK_INTERVAL;
            isInputted = true;
        }
    }

    // マウスホバーによる選択
    if (!isInputted && cursorCollider_ != nullptr)
    {
        for (size_t index = 0; index < multiUiCollisions_.size(); ++index)
        {
            if (multiUiCollisions_.at(index) != nullptr)
            {
                bool isHover = collisionController.CheckCollision2D(
                    cursorCollider_.get(),
                    multiUiCollisions_.at(index).get()
                );

                if (isHover)
                {
                    buttonSelectIndex_ = static_cast<int>(index);
                    break;
                }
            }
        }
    }

    // 決定入力時の処理
    if (keyConfInputManager.isTrigerDown("OK"))
    {
        if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::PASSCODE_BOX))
        {
            // パスコード編集モード開始
            isEditing_ = true;
            selectOctet_ = 0;
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
            return;
        }
        else if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON))
        {
            // HOST処理を開始
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);

            // 4桁のパスコード配列を数値に変換
            const int roomWordId = passcode_[0] * 1000 + passcode_[1] * 100 + passcode_[2] * 10 + passcode_[3];

            // NetManagerの準備とHOSTモード起動
            auto& netManager = NetManager::GetInstance();
            netManager.SetRoomWordId(roomWordId);
            netManager.Run(NET_MODE::HOST);

            multiState_ = LOBBY_STATE::CONNECTING;
            return;
        }
        else if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::CLIENT_BUTTON))
        {
            // CLIENT処理を開始
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);

            // 4桁のパスコード配列を数値に変換
            const int roomWordId = passcode_[0] * 1000 + passcode_[1] * 100 + passcode_[2] * 10 + passcode_[3];

            // NetManagerの準備とCLIENTモード起動
            auto& netManager = NetManager::GetInstance();
            IPDATA hostIp = { 255, 255, 255, 255 };
            NetManager::GetInstance().SetHostIp(hostIp);
            netManager.SetRoomWordId(roomWordId);
            netManager.Run(NET_MODE::CLIENT);

            multiState_ = LOBBY_STATE::CONNECTING;
            return;
        }
    }


    // キャンセルボタンで前の画面に戻る
    if (keyConfInputManager.isTrigerDown("CANCEL"))
    {
        multiState_ = LOBBY_STATE::SELECT_EQUIPMENT;
        return;
    }
}

void SceneLobby::UpdateConnecting(void)
{
    if (NetManager::GetInstance().IsHost())
    {
        multiState_ = LOBBY_STATE::IN_ROOM;
        return;
    }

    auto users = NetManager::GetInstance().GetNetUsers();

    if (!users.empty())
    {
        multiState_ = LOBBY_STATE::IN_ROOM;
    }
}

void SceneLobby::UpdateInRoom(void)
{
    auto& keyConfInputManager = KeyConfInputManager::GetInstance();
    constexpr int STICK_INTERVAL = 15;

    // 退出確認が表示されている場合の処理
    if (isLeaveWindow_)
    {
        auto& collisionController = CollisionController::GetInstance();

        bool isHoverYes = false;
        bool isHoverNo = false;

        if (cursorCollider_ != nullptr)
        {
            if (leaveYesCollider_ != nullptr)
            {
                isHoverYes = collisionController.CheckCollision2D(cursorCollider_.get(), leaveYesCollider_.get());
            }

            if (leaveNoCollider_ != nullptr)
            {
                isHoverNo = collisionController.CheckCollision2D(cursorCollider_.get(), leaveNoCollider_.get());
            }
        }

        // マウスホバーによるインデックスの更新
        if (isHoverYes)
        {
            leaveSelectIndex_ = 0;
        }
        else if (isHoverNo)
        {
            leaveSelectIndex_ = 1;
        }

        if (inputIntervalCounter_ > 0)
        {
            inputIntervalCounter_--;
        }

        if (!keyConfInputManager.isPressed("LEFT") && !keyConfInputManager.isPressed("RIGHT"))
        {
            inputIntervalCounter_ = 0;
        }

        // 左右入力での選択切り替え
        if (inputIntervalCounter_ == 0)
        {
            if (keyConfInputManager.isPressed("RIGHT") || keyConfInputManager.isPressed("LEFT"))
            {
                leaveSelectIndex_ = (leaveSelectIndex_ + 1) % 2;
                inputIntervalCounter_ = STICK_INTERVAL;
            }
        }

        // 決定入力
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);

            if (leaveSelectIndex_ == 0)
            {
                // はいを選択した場合は部屋を抜ける
                NetManager::GetInstance().Stop();
                isLeaveWindow_ = false;
                Initialize();
                return;
            }
            else
            {
                // いいえを選択した場合はウィンドウを閉じる
                isLeaveWindow_ = false;
                return;
            }
        }

        // キャンセル入力でウィンドウを閉じる
        if (keyConfInputManager.isTrigerDown("CANCEL"))
        {
            isLeaveWindow_ = false;
            return;
        }

        return;
    }

    if (NetManager::GetInstance().GetHasReceivedGoGame())
    {
        NetManager::GetInstance().ResetGoGame();

        NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
        self.gameState = GAME_STATE::GAME_PLAYING;
        NetManager::GetInstance().SetSelfInfo(self);

        auto users = NetManager::GetInstance().GetNetUsers();
        MoveToGameScene(users);
        return;
    }

    // キャンセルボタンで確認ウィンドウを開く
    if (keyConfInputManager.isTrigerDown("CANCEL"))
    {
        isLeaveWindow_ = true;
        leaveSelectIndex_ = 1;
        return;
    }

    NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
    bool isAllReady = (self.gameState == GAME_STATE::GOTO_GAME);

    auto users = NetManager::GetInstance().GetNetUsers();
    for (const auto& pair : users)
    {
        if (pair.second.gameState != GAME_STATE::GOTO_GAME)
        {
            isAllReady = false;
            break;
        }
    }

    // ホストかつ全員準備完了時の出撃、または通常の準備完了切替をOKキーで行う
    if (isAllReady && NetManager::GetInstance().IsHost() && !users.empty())
    {
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            self.gameState = GAME_STATE::GAME_PLAYING;
            NetManager::GetInstance().SetSelfInfo(self);

            NetManager::GetInstance().Send(NET_DATA_TYPE::GO_GAME_SCENE);

            MoveToGameScene(users);
            return;
        }
    }
    else
    {
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            myReadyState_ = !myReadyState_;
            self.gameState = myReadyState_ ? GAME_STATE::GOTO_GAME : GAME_STATE::CONNECTING;
            NetManager::GetInstance().SetSelfInfo(self);
        }
    }
}

void SceneLobby::DrawMulti(void)
{
    switch (multiState_)
    {
    case SceneLobby::LOBBY_STATE::SELECT_EQUIPMENT:
    {
        DrawCenterModel();

        const int startButtonHandle = (mainSelectIndex_ == 2)
            ? selectedUIBackHandle_ : selectUIBackHandle_;

        const Vector2 startButtonPosition =
        {
            Application::SCREEN_HALF_X + 350, Application::SCREEN_SIZE_Y - 100
        };

        // ボタン背景描画
        DrawRotaGraph3(startButtonPosition.x, startButtonPosition.y, uiBackWidth_ / 2,
            uiBackHeight_ / 2, 0.4f, 0.3f, 0.0f, startButtonHandle, true);

        // ボタンテキスト描画
        DrawRotaGraph(startButtonPosition.x, startButtonPosition.y, 0.6f, 0.0f,
            uiHandles_.at(static_cast<int>(UI_SINGLE::FORMATION)), true);

        DrawSelectionPanels();

        break;
    }
    case SceneLobby::LOBBY_STATE::SELECT_MODE:
        DrawSelectMode();
        break;

    case SceneLobby::LOBBY_STATE::CONNECTING:
        DrawConnecting();
        break;

    case SceneLobby::LOBBY_STATE::IN_ROOM:
        DrawInRoom();
        break;

    default:
        break;
    }
}

void SceneLobby::DrawSelectMode(void)
{
    const int IP_BOX = (Application::SCREEN_SIZE_X - 400) / 2;

    // パスコード枠が選択中
    const bool isPasscodeSelected = (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::PASSCODE_BOX));
    const bool isHighlight = (isEditing_ || isPasscodeSelected);

    const int BOX_X = static_cast<int>(IP_BOX + 200);
    const int BOX_Y = 280;

    const int currentPasscodeBoxHandle = isHighlight ? selectedMultiHandle_ : selectMultiHandle_;

    // パスコード枠背景描画
    DrawRotaGraph(BOX_X, BOX_Y, 0.4f, 0.0f, currentPasscodeBoxHandle, true);

    // パスコード数字画像の描画処理
    constexpr float NUMBER_SCALE = 0.5f;           // 通常の数字サイズ
    constexpr float EDIT_HIGHLIGHT_SCALE = 0.65f;     // 編集中桁のサイズ
    constexpr int DIGIT_SPACING = 100;              // 桁間隔

    const int START_DIGIT_X = BOX_X - static_cast<int>(DIGIT_SPACING * 1.5f);
    const int DIGIT_Y = BOX_Y;

    for (int digitIndex = 0; digitIndex < 4; ++digitIndex)
    {
        const int currentDigitX = START_DIGIT_X + (digitIndex * DIGIT_SPACING);
        const int numberValue = passcode_[digitIndex];

        const bool isEditingCurrentDigit = (isEditing_ && selectOctet_ == digitIndex);
        const float currentScale = isEditingCurrentDigit ? EDIT_HIGHLIGHT_SCALE : NUMBER_SCALE;

        // 画像での数字描画
        DrawRotaGraph(currentDigitX, DIGIT_Y, currentScale, 0.0, passcodeTextHandles_.at(numberValue), true);
    }

    // ホストボタン判定
    const bool isHostSelected = (!isEditing_ && buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON));
    const int hostBackgroundHandle = isHostSelected ? selectedUIBackHandle_ : selectUIBackHandle_;

    // ボタン背景描画
    DrawRotaGraph3(Application::SCREEN_HALF_X, static_cast<int>(HOST_BUTTON_Y),
        uiBackWidth_ / 2, uiBackHeight_ / 2, 0.4f, 0.3f, 0.0f, hostBackgroundHandle, true);

    // ボタンテキスト描画
    DrawRotaGraph(Application::SCREEN_HALF_X, static_cast<int>(HOST_BUTTON_Y),
        0.6f, 0.0f, uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::ROOM)), true);

    // クライアントボタン判定
    const bool isClientSelected = (!isEditing_ && buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::CLIENT_BUTTON));
    const int clientBackgroundHandle = isClientSelected ? selectedUIBackHandle_ : selectUIBackHandle_;

    // ボタン背景描画
    DrawRotaGraph3(Application::SCREEN_HALF_X, static_cast<int>(CLIENT_BUTTON_Y),
        uiBackWidth_ / 2, uiBackHeight_ / 2, 0.4f, 0.3f, 0.0f, clientBackgroundHandle, true);

    // ボタンテキスト描画
    DrawRotaGraph(Application::SCREEN_HALF_X, static_cast<int>(CLIENT_BUTTON_Y),
        0.6f, 0.0f, uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::JOIN)), true);

#ifdef _DEBUG
    CollisionController::GetInstance().DrawDebug2D();
#endif
}

void SceneLobby::DrawConnecting(void)
{
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);

    DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 1.0f, 0.0f,
        connectTexHandle_, true);
}

void SceneLobby::DrawInRoom(void)
{
    DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
        roomBackHandle_, true);

    int width = 0;
    int height = 0;

    GetGraphSize(selectMultiHandle_, &width, &height);

    int titlePositionX = 300;
    int titlePositionY = 100;

    DrawRotaGraph3(titlePositionX, titlePositionY, width / 2,
        height / 2, 0.3f, 0.3f, 0.0f, selectMultiHandle_, true);

    DrawRotaGraph(titlePositionX, titlePositionY, 0.6f, 0.0f, multiTitleHandle_, true);

    // 参加者データの集約
    std::vector<NET_JOIN_USER> allPlayers;
    NET_JOIN_USER selfUser = NetManager::GetInstance().GetSelfUser();
    allPlayers.push_back(selfUser);

    auto netUsers = NetManager::GetInstance().GetNetUsers();
    for (const auto& pair : netUsers)
    {
        allPlayers.push_back(pair.second);
    }

    // 横並び配置の計算用定数
    constexpr int MAX_SLOT_COUNT = 4;
    constexpr float PANEL_WIDTH = 300.0f;
    constexpr float PANEL_HEIGHT = 700.0f;
    constexpr float PANEL_START_POSITION_Y = 120.0f;

    const float totalWidth = static_cast<float>(Application::SCREEN_SIZE_X);
    const float totalPanelsWidth = PANEL_WIDTH * static_cast<float>(MAX_SLOT_COUNT);
    const float gapWidth = (totalWidth - totalPanelsWidth) / static_cast<float>(MAX_SLOT_COUNT + 1);

    for (int i = 0; i < MAX_SLOT_COUNT; ++i)
    {
        float panelLeft = gapWidth + static_cast<float>(i) * (PANEL_WIDTH + gapWidth);
        float panelTop = PANEL_START_POSITION_Y;
        float panelRight = panelLeft + PANEL_WIDTH;
        float panelBottom = panelTop + PANEL_HEIGHT;

        int panelLeftInteger = static_cast<int>(panelLeft);
        int panelTopInteger = static_cast<int>(panelTop);
        int panelRightInteger = static_cast<int>(panelRight);
        int panelBottomInteger = static_cast<int>(panelBottom);

        if (i < static_cast<int>(allPlayers.size()))
        {
            const NET_JOIN_USER& user = allPlayers.at(i);
            bool isSelf = (i == 0);
            bool isReady = (user.gameState >= GAME_STATE::GOTO_GAME);

            unsigned int panelBackgroundColor = isSelf ? GetColor(35, 45, 70) : GetColor(25, 25, 35);
            unsigned int borderColor = isReady ? COLOR_GREEN : (isSelf ? COLOR_YELLOW : COLOR_WHITE);

            // モデル描画エリア
            int modelAreaLeft = panelLeftInteger + 15;
            int modelAreaTop = panelTopInteger + 45;
            int modelAreaRight = panelRightInteger - 15;
            int modelAreaBottom = panelBottomInteger - 70;

            // 対象プレイヤーのスキンを取得
            const int currentSkinIndex = isSelf ? selectedSkinIndex_ : user.selectedSkinType;

            int lobbyModelHandle = GetLobbySkinModelHandle(currentSkinIndex);

            if (lobbyModelHandle != -1)
            {
                // パネルごとにX位置をずらす
                constexpr float PANEL_MODEL_OFFSET_X = 253.0f;
                const float modelPositionX = -380.0f + (static_cast<float>(i) * PANEL_MODEL_OFFSET_X);
                const float modelPositionY = -250.0f;
                const float modelPositionZ = 0.0f;

                MV1SetPosition(lobbyModelHandle, VGet(modelPositionX, modelPositionY, modelPositionZ));
                MV1SetRotationXYZ(lobbyModelHandle, VGet(0.1f, 0.0f, 0.0f));
                MV1SetScale(lobbyModelHandle, VGet(4.0f, 4.0f, 4.0f));

                MV1DrawModel(lobbyModelHandle);
            }

            int statusImageCenterX = (panelLeftInteger + panelRightInteger) / 2;
            int statusImageCenterY = modelAreaBottom + 35;

            int readyImageHandle = isReady
                ? uiRadyHandles_.at(static_cast<int>(UI_RADY_TEXT::RADY))
                : uiRadyHandles_.at(static_cast<int>(UI_RADY_TEXT::WAITING));

            if (readyImageHandle != -1)
            {
                DrawRotaGraph3(statusImageCenterX, statusImageCenterY, width / 2,
                    height / 2, 0.2f, 0.2f, 0.0f, selectMultiHandle_, true);
                DrawRotaGraph(statusImageCenterX, statusImageCenterY, 0.6, 0.0, readyImageHandle, true);
            }
        }
    }

    // 全員準備完了判定
    bool isAllReady = (selfUser.gameState == GAME_STATE::GOTO_GAME);

    for (const auto& pair : netUsers)
    {
        if (pair.second.gameState != GAME_STATE::GOTO_GAME)
        {
            isAllReady = false;
            break;
        }
    }

    // 全員準備完了時に画像を描画
    if (isAllReady && !netUsers.empty())
    {
        DrawRotaGraph(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 1.0f, 0.0f, allReadyImageHandle_, true);
    }

    // 退室確認ウィンドウを描画
    if (isLeaveWindow_)
    {
        DrawLeaveConfirmWindow();
    }
}

void SceneLobby::MoveToGameScene(std::map<int, NET_JOIN_USER>& _users)
{
    std::vector<SceneGame::PlayerSelectType> playerSelectTypes;

    {
        SceneGame::PlayerSelectType myType;
        myType.job = static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_);
        myType.skin = static_cast<PlayerBase::SKIN_TYPE>(selectedSkinIndex_);
        playerSelectTypes.push_back(myType);
    }

    for (auto iterator = _users.begin(); iterator != _users.end(); ++iterator)
    {
        SceneGame::PlayerSelectType otherType;
        otherType.job = static_cast<PlayerBase::JOB_TYPE>(iterator->second.selectedJobType);
        otherType.skin = static_cast<PlayerBase::SKIN_TYPE>(iterator->second.selectedSkinType);
        playerSelectTypes.push_back(otherType);
    }

    SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(playerSelectTypes));
}

void SceneLobby::SetJobToSKin(void)
{
    if (!IS_MULTI) { return; }

    NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
    self.selectedJobType = selectedJobIndex_;
    self.selectedSkinType = selectedSkinIndex_;
    NetManager::GetInstance().SetSelfInfo(self);
}

void SceneLobby::DrawLeaveConfirmWindow(void)
{
    // 背景を暗くする
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // メインウィンドウ背景の描画
    DrawRotaGraph3(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, uiBackWidth_ / 2,
        uiBackHeight_ / 2, 1.2f, 1.0f, 0.0f, selectUIBackHandle_, true);

    // 部屋を出る画像の描画
    if (leaveRoomTextHandle_ != -1)
    {
        DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y - 80, 0.8f, 0.0f, leaveRoomTextHandle_, true);
    }

    // はいボタン
    const int yesBackgroundHandle = (leaveSelectIndex_ == 0) ? selectedUIBackHandle_ : selectUIBackHandle_;
    const int yesPositionX = Application::SCREEN_HALF_X - 150;
    const int buttonPositionY = Application::SCREEN_HALF_Y + 80;

    // 文字画像
    int yesHandle = (leaveSelectIndex_ == 0)
        ? selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES))
        : noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES));

    if (yesHandle != -1)
    {
        DrawRotaGraph(yesPositionX, buttonPositionY, 0.6f, 0.0f, yesHandle, true);
    }

    // いいえボタン
    const int noBackgroundHandle = (leaveSelectIndex_ == 1) ? selectedUIBackHandle_ : selectUIBackHandle_;
    const int noPositionX = Application::SCREEN_HALF_X + 150;

    // 文字画像
    int noHandle = (leaveSelectIndex_ == 1)
        ? selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO))
        : noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO));

    if (noHandle != -1)
    {
        DrawRotaGraph(noPositionX, buttonPositionY, 0.6f, 0.0f, noHandle, true);
    }
}

void SceneLobby::DrawWeaponWindow(void)
{
    // ウィンドウ背景画像描画
    DrawRotaGraph3(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, uiBackWidth_ / 2,
        uiBackHeight_ / 2, 1.0f, 1.2f, 0.0f, selectUIBackHandle_, true);

    // 選択肢描画用のレイアウト設定
    const float itemCenterX = Application::SCREEN_HALF_X;
    const float itemStartY = Application::SCREEN_HALF_Y - 200.0f;
    const float itemIntervalY = 150.0f;

    constexpr int jobMaximum = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    for (int jobIndex = 0; jobIndex < jobMaximum; ++jobIndex)
    {
        const float currentItemY = itemStartY + (jobIndex * itemIntervalY);
        const bool isSelected = (selectedJobIndex_ == jobIndex);

        auto currentJobType = static_cast<PlayerBase::JOB_TYPE>(jobIndex);
        int optionHandle = GetWeaponUIHandle(currentJobType, isSelected);

        if (optionHandle != -1)
        {
            DrawRotaGraph(static_cast<int>(itemCenterX), static_cast<int>(currentItemY),
                0.6f, 0.0f, optionHandle, true);
        }
    }
}

void SceneLobby::DrawSkinWindow(void)
{
    // ウィンドウ背景画像描画
    DrawRotaGraph3(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, uiBackWidth_ / 2,
        uiBackHeight_ / 2, 1.0f, 1.2f, 0.0f, selectUIBackHandle_, true);

    // 選択肢描画用のレイアウト設定
    const float itemCenterX = Application::SCREEN_HALF_X;
    const float itemStartY = Application::SCREEN_HALF_Y - 200.0f;
    const float itemIntervalY = 150.0f;

    constexpr int skinMaximum = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

    for (int skinIndex = 0; skinIndex < skinMaximum; ++skinIndex)
    {
        const float currentItemY = itemStartY + (skinIndex * itemIntervalY);
        const bool isSelected = (selectedSkinIndex_ == skinIndex);

        auto currentSkinType = static_cast<PlayerBase::SKIN_TYPE>(skinIndex);
        int optionHandle = GetSkinUIHandle(currentSkinType, isSelected);

        if (optionHandle != -1)
        {
            DrawRotaGraph(static_cast<int>(itemCenterX), static_cast<int>(currentItemY),
                0.6f, 0.0f, optionHandle, true);
        }
    }
}

int SceneLobby::GetLobbySkinModelHandle(int _skinIndex) const
{
    if (_skinIndex < 0 || _skinIndex >= static_cast<int>(lobbySkinHandles_.size()))
    {
        return -1;
    }
    return lobbySkinHandles_.at(_skinIndex);
}

int SceneLobby::GetWeaponUIHandle(PlayerBase::JOB_TYPE _jobType, bool _isSelected) const
{
    switch (_jobType)
    {
    case PlayerBase::JOB_TYPE::BOMB:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_BOMB))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_BOMB));

    case PlayerBase::JOB_TYPE::CANNON:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_BIG))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_BIG));

    case PlayerBase::JOB_TYPE::SUPPORT:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_RECOVERY))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_RECOVERY));

    case PlayerBase::JOB_TYPE::RAPID_FIRE:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_RAPID_FIRE))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_RAPID_FIRE));

    default:
        return -1;
    }
}

int SceneLobby::GetSkinUIHandle(PlayerBase::SKIN_TYPE _skinType, bool _isSelected) const
{
    switch (_skinType)
    {
    case PlayerBase::SKIN_TYPE::HUMAN:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_SKIN_MOMO))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_SKIN_MOMO));

    case PlayerBase::SKIN_TYPE::MONKEY:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_SKIN_SARU))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_SKIN_SARU));

    case PlayerBase::SKIN_TYPE::BIRD:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_SKIN_KIGI))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_SKIN_KIGI));

    case PlayerBase::SKIN_TYPE::DOG:
        return _isSelected
            ? uiHandles_.at(static_cast<size_t>(UI_SINGLE::SELECT_SKIN_INU))
            : uiHandles_.at(static_cast<size_t>(UI_SINGLE::NOT_SELECT_SKIN_INU));

    default:
        return -1;
    }
}