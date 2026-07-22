#include "SceneLobby.h"

#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/NetManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Camera/Camera.h"
#include "../../Application.h"
#include "./SceneGame.h"
#include "../../Common/Loading.h"

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
    , readyImageHandle_(-1)
    , waitingImageHandle_(-1)
    , previewModelHandle_(-1)
    , currentModelIndex_(-1)
    , mainSelectIndex_(0)
    , selectState_(SELECT_STATE::MAIN)
{
}

SceneLobby::~SceneLobby(void)
{
}

void SceneLobby::Load(void)
{
    ResourceManager::GetInstance()
        .LoadHandleIds(ResourceManager::SRC::IMGS_SELECT, uiHandles_.data());
}

void SceneLobby::EndLoad(void)
{
}

void SceneLobby::Initialize(void)
{
    NetManager::GetInstance().Stop();

    selectedJobIndex_ = 0;
    selectedSkinIndex_ = 0;

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
        buttonSelectIndex_ = -1;
        isEditing_ = false;

        InitUIMulti();
    }
    else
    {
        SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);
    }

    // 初回のモデル読み込み
    UpdatePreviewModel();
}

void SceneLobby::InitUISingle(void)
{
    // メイン用コライダーの生成
    
    // 左パネル（武器選択用）
    Vector2F leftPanelPos(150.0f, 300.0f);
    mainUiCollisions_.at(0) = std::make_unique<Collider2DBox>(
        leftPanelPos, 300.0f, 400.0f, Collider2DBase::TAG_2D::NONE
    );

    // 右パネル（見た目選択用）
    Vector2F rightPanelPos(1100.0f, 300.0f);
    mainUiCollisions_.at(1) = std::make_unique<Collider2DBox>(
        rightPanelPos, 300.0f, 400.0f, Collider2DBase::TAG_2D::NONE
    );


    // 武器ミニウィンドウ内のコライダー生成
    int screenWidth = Application::SCREEN_SIZE_X;
    int screenHeight = Application::SCREEN_SIZE_Y;
    int windowX = (screenWidth - 600) / 2;
    int windowY = (screenHeight - 400) / 2;

    int itemStartX = windowX + 40;
    int itemStartY = windowY + 70;
    int itemIntervalY = 75;

    constexpr int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
    {
        int currentItemY = itemStartY + (jobIndex * itemIntervalY);
        // 各行の枠の中心座標とサイズを指定
        Vector2F itemCenterPos(static_cast<float>(itemStartX + (600 - 80) / 2), static_cast<float>(currentItemY + 30));

        weaponUiCollisions_.at(static_cast<size_t>(jobIndex)) = std::make_unique<Collider2DBox>(
            itemCenterPos, 520.0f, 60.0f, Collider2DBase::TAG_2D::NONE
        );
    }


    // スキンミニウィンドウ内のコライダー生成
    constexpr int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

    for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
    {
        int currentItemY = itemStartY + (skinIndex * itemIntervalY);
        Vector2F itemCenterPos(static_cast<float>(itemStartX + (600 - 80) / 2), static_cast<float>(currentItemY + 30));

        skinUiCollisions_.at(static_cast<size_t>(skinIndex)) = std::make_unique<Collider2DBox>(
            itemCenterPos, 520.0f, 60.0f, Collider2DBase::TAG_2D::NONE
        );
    }


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
}

void SceneLobby::InitUIMulti(void)
{
    int screenWidth = Application::SCREEN_SIZE_X;
    int ipBoxX = (screenWidth - 400) / 2;
    int buttonStartX = (screenWidth - (180 * 2 + 20)) / 2;

    // パスコード入力ボックスのコライダー
    Vector2F passcodePosition(static_cast<float>(ipBoxX + 200), 280.0f);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::PASSCODE_BOX))
        = std::make_unique<Collider2DBox>(passcodePosition, 400.0f, 60.0f, Collider2DBase::TAG_2D::NONE);

    // HOSTボタンのコライダー
    Vector2F hostPosition(static_cast<float>(buttonStartX + 90), 430.0f);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::HOST_BUTTON))
        = std::make_unique<Collider2DBox>(hostPosition, 180.0f, 60.0f, Collider2DBase::TAG_2D::NONE);

    // CLIENTボタンのコライダー
    Vector2F clientPosition(static_cast<float>(buttonStartX + 290), 430.0f);
    multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::CLIENT_BUTTON))
        = std::make_unique<Collider2DBox>(clientPosition, 180.0f, 60.0f, Collider2DBase::TAG_2D::NONE);

    for (auto& collider : multiUiCollisions_)
    {
        if (collider != nullptr)
        {
            CollisionController::GetInstance().RegisterCollider2D(collider.get());
        }
    }
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
        VECTOR modelPos = VGet(0.0f, -200.0f, 0.0f);
        MV1SetPosition(previewModelHandle_, modelPos);

        // スケール調整
        MV1SetScale(previewModelHandle_, VGet(5.0f, 5.0f, 5.0f));

        // 自動Y軸回転
        static float rotY = 0.0f;
        rotY += 0.01f;
        MV1SetRotationXYZ(previewModelHandle_, VGet(0.0f, rotY, 0.0f));
    }
}

void SceneLobby::Draw(void)
{
    if (IS_MULTI)
    {
        DrawMulti();
    }
    else
    {
        DrawSelectionPanels();
        DrawCenterModel();

        const Vector2 startButtonPosition = { Application::SCREEN_SIZE_X / 2 - 100, 620 };
        DrawGraph(startButtonPosition.x, startButtonPosition.y, uiHandles_.at(static_cast<int>(UI_SINGLE::GAME_START)), true);

#ifdef _DEBUG
        CollisionController::GetInstance().DrawDebug2D();
#endif
    }
}

void SceneLobby::Release(void)
{
    if (previewModelHandle_ != -1)
    {
        MV1DeleteModel(previewModelHandle_);
        previewModelHandle_ = -1;
    }
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
    // 1. 左右のメインUIを描画
   // DrawRotaGraph(LEFT_PANEL_X, PANEL_START_Y, BUTTON_SCALE, 0.0, weaponMainHandle_, true);
   // DrawRotaGraph(RIGHT_PANEL_X, PANEL_START_Y, BUTTON_SCALE, 0.0, skinMainHandle_, true);

    // 2. ミニウィンドウが開いている場合は暗めの半透明フィルターを表示
    if (selectState_ != SELECT_STATE::MAIN)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), true); // 画面全体を暗く
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // 3. 各ミニウィンドウの描画
    if (selectState_ == SELECT_STATE::WEAPON_WINDOW)
    {
        DrawWeaponWindow(); // 画面中央に武器選択ウィンドウを描画
    }
    else if (selectState_ == SELECT_STATE::SKIN_WINDOW)
    {
        DrawSkinWindow();   // 画面中央にスキン選択ウィンドウを描画
    }
}

void SceneLobby::UpdateSingle(void)
{
    auto& keyConfInputManager = KeyConfInputManager::GetInstance();
    auto& collisionController = CollisionController::GetInstance();

    Vector2F stick = keyConfInputManager.GetLeftStickRaw();
    constexpr float THRESHOLD = 0.5f;
    constexpr int STICK_INTERVAL = 15;

    // スティック用のインターバルタイマー更新
    if (inputIntervalCounter_ > 0)
    {
        inputIntervalCounter_--;
    }

    switch (selectState_)
    {
    case SELECT_STATE::MAIN:
    {
        // マウスカーソルと左右メインUIの当たり判定
        bool isHoverWeapon = false;
        bool isHoverSkin = false;

        // ヌルチェックを行ってから当たり判定を実行
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
        }

        // マウスホバーがあればインデックスを更新
        if (isHoverWeapon)
        {
            mainSelectIndex_ = 0;
        }
        else if (isHoverSkin)
        {
            mainSelectIndex_ = 1;
        }

        // キーボード/パッドの左右入力で選択切り替え
        if (inputIntervalCounter_ == 0 && std::abs(stick.x) > THRESHOLD)
        {
            if (stick.x > 0.0f)
            {
                mainSelectIndex_ = 1;
            }
            else
            {
                mainSelectIndex_ = 0;
            }
            inputIntervalCounter_ = STICK_INTERVAL;
        }

        // 決定（OK）入力時の処理
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            if (mainSelectIndex_ == 0)
            {
                selectState_ = SELECT_STATE::WEAPON_WINDOW;
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            }
            else if (mainSelectIndex_ == 1)
            {
                selectState_ = SELECT_STATE::SKIN_WINDOW;
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            }
        }

        // 武器・見た目の両方が決定済みでゲーム開始ボタンを押した場合の遷移
        if (keyConfInputManager.isTrigerDown("START"))
        {
            if (IS_MULTI)
            {
                multiState_ = LOBBY_STATE::SELECT_MODE;
            }
            else
            {
                auto selectedJob = static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_);
                auto selectedSkin = static_cast<PlayerBase::SKIN_TYPE>(selectedSkinIndex_);

                auto jobs = { SceneGame::PlayerSelectType(selectedJob, selectedSkin) };
                SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(jobs));
            }
        }
        break;
    }

    case SELECT_STATE::WEAPON_WINDOW:
    {
        constexpr int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

        // キーボード/パッドの上下入力で武器選択移動
        if (inputIntervalCounter_ == 0 && std::abs(stick.y) > THRESHOLD)
        {
            if (stick.y < 0.0f)
            {
                selectedJobIndex_ = (selectedJobIndex_ + 1) % jobMax;
            }
            else
            {
                selectedJobIndex_ = (selectedJobIndex_ - 1 + jobMax) % jobMax;
            }
            inputIntervalCounter_ = STICK_INTERVAL;
        }

        // 画面中央武器ミニウィンドウ内のコライダー判定
        for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
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
                        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
                        return;
                    }
                }
            }
        }

        // パッド/キーボードでの「OK」決定
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            selectState_ = SELECT_STATE::MAIN;
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
        }

        // キャンセル入力でウィンドウを閉じる
        if (keyConfInputManager.isTrigerDown("CANCEL"))
        {
            selectState_ = SELECT_STATE::MAIN;
        }
        break;
    }

    case SELECT_STATE::SKIN_WINDOW:
    {
        constexpr int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

        // キーボード/パッドの上下入力でスキン選択移動
        if (inputIntervalCounter_ == 0 && std::abs(stick.y) > THRESHOLD)
        {
            if (stick.y < 0.0f)
            {
                selectedSkinIndex_ = (selectedSkinIndex_ + 1) % skinMax;
            }
            else
            {
                selectedSkinIndex_ = (selectedSkinIndex_ - 1 + skinMax) % skinMax;
            }
            inputIntervalCounter_ = STICK_INTERVAL;
        }

        // 画面中央スキンミニウィンドウ内のコライダー判定
        for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
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
                        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
                        return;
                    }
                }
            }
        }

        // パッド/キーボードでのOK決定
        if (keyConfInputManager.isTrigerDown("OK"))
        {
            selectState_ = SELECT_STATE::MAIN;
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
        }

        // キャンセル入力でウィンドウを閉じる
        if (keyConfInputManager.isTrigerDown("CANCEL"))
        {
            selectState_ = SELECT_STATE::MAIN;
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

    // IP・パスコード編集モード中の操作
    if (isEditing_)
    {
        if (keyConfInputManager.isTrigerDown("CANCEL"))
        {
            isEditing_ = false;
            return;
        }

        if (keyConfInputManager.isTrigerDown("LEFT"))
        {
            selectOctet_ = (selectOctet_ + 3) % 4;
        }

        if (keyConfInputManager.isTrigerDown("RIGHT"))
        {
            selectOctet_ = (selectOctet_ + 1) % 4;
        }

        if (keyConfInputManager.isTrigerDown("UP"))
        {
            passcode_[selectOctet_] = (passcode_[selectOctet_] + 1) % 10;
        }

        if (keyConfInputManager.isTrigerDown("DOWN"))
        {
            passcode_[selectOctet_] = (passcode_[selectOctet_] + 9) % 10;
        }

        if (keyConfInputManager.isTrigerDown("OK"))
        {
            isEditing_ = false;
        }
        return;
    }

    // CollisionControllerを経由して2Dコライダーの当たり判定を行う
    auto& collisionController = CollisionController::GetInstance();

    bool isHoverPasscode = collisionController.CheckCollision2D(
        multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::PASSCODE_BOX)).get(),
        cursorCollider_.get()
    );

    bool isHoverHost = collisionController.CheckCollision2D(
        multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::HOST_BUTTON)).get(),
        cursorCollider_.get()
    );

    bool isHoverClient = collisionController.CheckCollision2D(
        multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::CLIENT_BUTTON)).get(),
        cursorCollider_.get()
    );

    buttonSelectIndex_ = -1;

    if (isHoverHost)
    {
        buttonSelectIndex_ = 0;
    }
    else if (isHoverClient)
    {
        buttonSelectIndex_ = 1;
    }

    // キーコンフィグの "OK" トリガーで決定処理を行う
    if (keyConfInputManager.isTrigerDown("OK"))
    {
        if (isHoverPasscode)
        {
            isEditing_ = true;
            selectOctet_ = 0;
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            return;
        }

        int roomWord = passcode_[0] * 1000 + passcode_[1] * 100 +
            passcode_[2] * 10 + passcode_[3];

        NetManager::GetInstance().SetRoomWordId(roomWord);

        if (buttonSelectIndex_ == 0)
        {
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            NetManager::GetInstance().Run(NET_MODE::HOST);
            multiState_ = LOBBY_STATE::CONNECTING;
        }
        else if (buttonSelectIndex_ == 1)
        {
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            IPDATA hostIp;
            hostIp.d1 = 255;
            hostIp.d2 = 255;
            hostIp.d3 = 255;
            hostIp.d4 = 255;

            NetManager::GetInstance().SetHostIp(hostIp);
            NetManager::GetInstance().Run(NET_MODE::CLIENT);
            multiState_ = LOBBY_STATE::CONNECTING;
        }
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

    if (keyConfInputManager.isTrigerDown("CANCEL"))
    {
        NetManager::GetInstance().Stop();
        Initialize();
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

    // ホストかつ全員準備完了時の出撃、または通常の準備完了切替を "OK" キーで行う
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
        DrawSelectionPanels();
        DrawCenterModel();
        break;

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
    int screenWidth = Application::SCREEN_SIZE_X;
    int ipBoxX = (screenWidth - 400) / 2;
    int buttonStartX = (screenWidth - (180 * 2 + 20)) / 2;

    auto& collisionController = CollisionController::GetInstance();
    bool isHoverPasscode = collisionController.CheckCollision2D(
        multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::PASSCODE_BOX)).get(),
        cursorCollider_.get()
    );

    DrawBox(ipBoxX, 250, ipBoxX + 400, 310, GetColor(30, 30, 30), true);
    DrawBox(ipBoxX, 250, ipBoxX + 400, 310, (isEditing_ || isHoverPasscode) ? COLOR_YELLOW : COLOR_WHITE, false);
    std::string passString = std::to_string(passcode_[0]) + std::to_string(passcode_[1]) +
        std::to_string(passcode_[2]) + std::to_string(passcode_[3]);
    DrawString(ipBoxX + 20, 272, ("Room Passcode: " + passString).c_str(), COLOR_WHITE);

    if (isEditing_)
    {
        DrawFormatString(ipBoxX + 20, 320, COLOR_YELLOW, "編集中: %d桁目", selectOctet_ + 1);
    }

    // HOSTボタン
    DrawBox(buttonStartX, 400, buttonStartX + 180, 460, GetColor(0, 100, 0), true);
    DrawBox(buttonStartX, 400, buttonStartX + 180, 460, (!isEditing_ && buttonSelectIndex_ == 0) ?
        COLOR_YELLOW : COLOR_WHITE, false);
    DrawString(buttonStartX + 70, 422, "HOST", COLOR_WHITE);

    // CLIENTボタン
    int clientX = buttonStartX + 200;
    DrawBox(clientX, 400, clientX + 180, 460, GetColor(0, 0, 150), true);
    DrawBox(clientX, 400, clientX + 180, 460, (!isEditing_ && buttonSelectIndex_ == 1)
        ? COLOR_YELLOW : COLOR_WHITE, false);
    DrawString(clientX + 65, 422, "CLIENT", COLOR_WHITE);

#ifdef _DEBUG
    CollisionController::GetInstance().DrawDebug2D();
#endif
}

void SceneLobby::DrawConnecting(void)
{
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(10, 10, 15), true);
    DrawString(100, 100, "ホストへの接続を待機しています...", COLOR_YELLOW);
    DrawString(100, 130, "[CANCELキー] キャンセル", COLOR_WHITE);
}

void SceneLobby::DrawInRoom(void)
{
    // 背景
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(10, 10, 18), true);

    // タイトル枠
    const int titleBoxX = 100;
    const int titleBoxY = 30;
    const int titleBoxWidth = Application::SCREEN_SIZE_X - (titleBoxX * 2);
    const int titleBoxHeight = 60;

    DrawBox(titleBoxX, titleBoxY, titleBoxX + titleBoxWidth, titleBoxY + titleBoxHeight, GetColor(25, 30, 50), true);
    DrawBox(titleBoxX, titleBoxY, titleBoxX + titleBoxWidth, titleBoxY + titleBoxHeight, COLOR_WHITE, false);
    DrawString(titleBoxX + 20, titleBoxY + 20, "LOBBY ROOM - PLAYER SELECT", COLOR_WHITE);

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
    constexpr float PANEL_WIDTH = 240.0f;
    constexpr float PANEL_HEIGHT = 380.0f;
    constexpr float PANEL_START_Y = 120.0f;

    const float totalWidth = static_cast<float>(Application::SCREEN_SIZE_X);
    const float totalPanelsWidth = PANEL_WIDTH * static_cast<float>(MAX_SLOT_COUNT);
    const float gapX = (totalWidth - totalPanelsWidth) / static_cast<float>(MAX_SLOT_COUNT + 1);

    for (int i = 0; i < MAX_SLOT_COUNT; ++i)
    {
        float panelLeft = gapX + static_cast<float>(i) * (PANEL_WIDTH + gapX);
        float panelTop = PANEL_START_Y;
        float panelRight = panelLeft + PANEL_WIDTH;
        float panelBottom = panelTop + PANEL_HEIGHT;

        int panelLeftI = static_cast<int>(panelLeft);
        int panelTopI = static_cast<int>(panelTop);
        int panelRightI = static_cast<int>(panelRight);
        int panelBottomI = static_cast<int>(panelBottom);

        if (i < static_cast<int>(allPlayers.size()))
        {
            const NET_JOIN_USER& user = allPlayers.at(i);
            bool isSelf = (i == 0);
            bool isReady = (user.gameState >= GAME_STATE::GOTO_GAME);

            unsigned int panelBgColor = isSelf ? GetColor(35, 45, 70) : GetColor(25, 25, 35);
            unsigned int borderColor = isReady ? COLOR_GREEN : (isSelf ? COLOR_YELLOW : COLOR_WHITE);

            // 外枠パネル
            DrawBox(panelLeftI, panelTopI, panelRightI, panelBottomI, panelBgColor, true);
            DrawBox(panelLeftI, panelTopI, panelRightI, panelBottomI, borderColor, false);

            // プレイヤー名・ロール
            std::string roleName = (user.mode == NET_MODE::HOST) ? "HOST" : "CLIENT";
            DrawFormatString(panelLeftI + 15, panelTopI + 15, COLOR_WHITE, "P%d : %s", i + 1, roleName.c_str());

            if (isSelf)
            {
                DrawString(panelRightI - 55, panelTopI + 15, "(YOU)", COLOR_YELLOW);
            }

            // -------------------------------------------------------------
            // 3Dモデル・立ち絵表示エリア
            // -------------------------------------------------------------
            int modelAreaLeft = panelLeftI + 15;
            int modelAreaTop = panelTopI + 45;
            int modelAreaRight = panelRightI - 15;
            int modelAreaBottom = panelBottomI - 70; // 下部に画像配置スペースを確保

            DrawBox(modelAreaLeft, modelAreaTop, modelAreaRight, modelAreaBottom, GetColor(15, 15, 20), true);
            DrawBox(modelAreaLeft, modelAreaTop, modelAreaRight, modelAreaBottom, COLOR_GRAY, false);

            if (isSelf)
            {
                int selectedImageHandle = uiHandles_.at(selectedJobIndex_ * 2);
                int centerX = (modelAreaLeft + modelAreaRight) / 2;
                int centerY = (modelAreaTop + modelAreaBottom) / 2;

                DrawRotaGraph(centerX, centerY, 0.5, 0.0, selectedImageHandle, true);
            }
            else
            {
                DrawString(modelAreaLeft + 20, (modelAreaTop + modelAreaBottom) / 2, "3D MODEL", COLOR_GRAY);
            }

            // -------------------------------------------------------------
            // 準備完了ステータス画像エリア（モデルの下）
            // -------------------------------------------------------------
            int statusImageCenterX = (panelLeftI + panelRightI) / 2;
            int statusImageCenterY = modelAreaBottom + 35; // モデルエリアの直下に配置

            // 読み込んだREADY画像 / WAITING画像のハンドルを指定
            int readyImageHandle = isReady ? readyImageHandle_ : waitingImageHandle_;

            if (readyImageHandle != -1)
            {
                // 画像が読み込まれている場合は中央揃えで描画
                DrawRotaGraph(statusImageCenterX, statusImageCenterY, 1.0, 0.0, readyImageHandle, true);
            }
            else
            {
                // 画像読み込み前・テスト用の仮DrawBox枠
                int statusBoxWidth = 180;
                int statusBoxHeight = 40;
                int statusBoxLeft = statusImageCenterX - (statusBoxWidth / 2);
                int statusBoxTop = statusImageCenterY - (statusBoxHeight / 2);

                unsigned int statusBgColor = isReady ? GetColor(0, 100, 0) : GetColor(60, 60, 60);
                DrawBox(statusBoxLeft, statusBoxTop, statusBoxLeft + statusBoxWidth, statusBoxTop + statusBoxHeight, statusBgColor, true);
                DrawBox(statusBoxLeft, statusBoxTop, statusBoxLeft + statusBoxWidth, statusBoxTop + statusBoxHeight, borderColor, false);

                std::string statusText = isReady ? "READY !!" : "WAITING...";
                DrawString(statusBoxLeft + 45, statusBoxTop + 12, statusText.c_str(), COLOR_WHITE);
            }
        }
        else
        {
            // 未参加（空き）スロット枠
            DrawBox(panelLeftI, panelTopI, panelRightI, panelBottomI, GetColor(15, 15, 20), true);
            DrawBox(panelLeftI, panelTopI, panelRightI, panelBottomI, COLOR_GRAY, false);

            DrawFormatString(panelLeftI + 15, panelTopI + 15, COLOR_GRAY, "P%d : EMPTY", i + 1);
        }
    }

    // 操作ガイド（最下部）
    int bottomGuideY = 530;
    DrawBox(100, bottomGuideY, Application::SCREEN_SIZE_X - 100, bottomGuideY + 50, GetColor(20, 20, 30), true);
    DrawBox(100, bottomGuideY, Application::SCREEN_SIZE_X - 100, bottomGuideY + 50, COLOR_WHITE, false);

    DrawString(120, bottomGuideY + 16, "[OKキー] 準備完了(READY)切替", COLOR_YELLOW);
    DrawString(420, bottomGuideY + 16, "[CANCELキー] 退出", COLOR_WHITE);

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

    if (isAllReady && NetManager::GetInstance().IsHost() && !netUsers.empty())
    {
        DrawString(700, bottomGuideY + 16, "【出撃可能! OKキーで開始】", COLOR_GREEN);
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
        otherType.job = PlayerBase::JOB_TYPE::BOMB;
        otherType.skin = PlayerBase::SKIN_TYPE::DOG;
        playerSelectTypes.push_back(otherType);
    }

    SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(playerSelectTypes));
}

void SceneLobby::DrawWeaponWindow(void)
{
    // ウィンドウ全体のサイズと位置計算（画面中央に配置）
    int screenWidth = Application::SCREEN_SIZE_X;
    int screenHeight = Application::SCREEN_SIZE_Y;

    int windowWidth = 600;
    int windowHeight = 400;
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // ウィンドウ背景と外枠の描画
    DrawBox(windowX, windowY, windowX + windowWidth, windowY + windowHeight, GetColor(20, 20, 20), true);
    DrawBox(windowX, windowY, windowX + windowWidth, windowY + windowHeight, COLOR_WHITE, false);

    // タイトル描画
    DrawString(windowX + 20, windowY + 20, "WEAPON SELECT", COLOR_WHITE);

    // 武器リストの描画
    int itemStartX = windowX + 40;
    int itemStartY = windowY + 70;
    int itemWidth = windowWidth - 80;
    int itemHeight = 60;
    int itemIntervalY = 75;

    constexpr int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
    {
        int currentItemY = itemStartY + (jobIndex * itemIntervalY);

        // 選択中のアイテムは黄色枠、それ以外は白枠
        bool isSelected = (selectedJobIndex_ == jobIndex);
        unsigned int borderColor = isSelected ? COLOR_YELLOW : COLOR_WHITE;
        unsigned int fillColor = isSelected ? GetColor(60, 60, 20) : GetColor(40, 40, 40);

        // アイテム枠の描画
        DrawBox(itemStartX, currentItemY, itemStartX + itemWidth, currentItemY + itemHeight, fillColor, true);
        DrawBox(itemStartX, currentItemY, itemStartX + itemWidth, currentItemY + itemHeight, borderColor, false);

        // テキスト表示（後で画像描画にする場合はここを変更）
        std::string jobNameText = "Weapon Type " + std::to_string(jobIndex + 1);
        DrawString(itemStartX + 20, currentItemY + 20, jobNameText.c_str(), COLOR_WHITE);
    }
}

void SceneLobby::DrawSkinWindow(void)
{
    // ウィンドウ全体のサイズと位置計算（画面中央に配置）
    int screenWidth = Application::SCREEN_SIZE_X;
    int screenHeight = Application::SCREEN_SIZE_Y;

    int windowWidth = 600;
    int windowHeight = 400;
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // ウィンドウ背景と外枠の描画
    DrawBox(windowX, windowY, windowX + windowWidth, windowY + windowHeight, GetColor(20, 20, 20), true);
    DrawBox(windowX, windowY, windowX + windowWidth, windowY + windowHeight, COLOR_WHITE, false);

    // タイトル描画
    DrawString(windowX + 20, windowY + 20, "SKIN SELECT", COLOR_WHITE);

    // スキンリストの描画
    int itemStartX = windowX + 40;
    int itemStartY = windowY + 70;
    int itemWidth = windowWidth - 80;
    int itemHeight = 60;
    int itemIntervalY = 75;

    constexpr int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

    for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
    {
        int currentItemY = itemStartY + (skinIndex * itemIntervalY);

        // 選択中のアイテムは黄色枠、それ以外は白枠
        bool isSelected = (selectedSkinIndex_ == skinIndex);
        unsigned int borderColor = isSelected ? COLOR_YELLOW : COLOR_WHITE;
        unsigned int fillColor = isSelected ? GetColor(60, 60, 20) : GetColor(40, 40, 40);

        // アイテム枠の描画
        DrawBox(itemStartX, currentItemY, itemStartX + itemWidth, currentItemY + itemHeight, fillColor, true);
        DrawBox(itemStartX, currentItemY, itemStartX + itemWidth, currentItemY + itemHeight, borderColor, false);

        // テキスト表示（後で画像描画にする場合はここを変更）
        std::string skinNameText = "Skin Type " + std::to_string(skinIndex + 1);
        DrawString(itemStartX + 20, currentItemY + 20, skinNameText.c_str(), COLOR_WHITE);
    }
}