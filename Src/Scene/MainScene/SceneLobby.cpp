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

// UIボタンの配置計算設定
const float INTERVAL_Y = 150.0f;

constexpr float BUTTON_SCALE = 0.6f;
constexpr Vector2F BUTTON_SIZE = { 600.0f * BUTTON_SCALE, 250.0f * BUTTON_SCALE };
const Vector2F UI_POS = { 100.0f, 100.0f };

const Vector2F POS_SELECT_BOMB = Vector2F(UI_POS.x, UI_POS.y);
const Vector2F POS_SELECT_CANNON = Vector2F(UI_POS.x, UI_POS.y + INTERVAL_Y);
const Vector2F POS_SELECT_RECOVERY = Vector2F(UI_POS.x, UI_POS.y + (INTERVAL_Y * 2));
const Vector2F POS_SELECT_RAPIDFIRE = Vector2F(UI_POS.x, UI_POS.y + (INTERVAL_Y * 3));

SceneLobby::SceneLobby(bool _isMulti)
	: SceneBase::SceneBase()
	, IS_MULTI(_isMulti)
    , cursorCollider_(nullptr)
    , inputIntervalCounter_(0)
    , selectOctet_(0)
    , isEditing_(false)
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
    selectedIdx_ = 0;

    if (IS_MULTI)
    {
        multiState_ = LOBBY_STATE::SELECT_MODE;
        passcode_[0] = 0; 
        passcode_[1] = 0;
        passcode_[2] = 0;
        passcode_[3] = 0;
        buttonSelectIndex_ = 0; 
        isEditing_ = false;
    }
    else
    {
        SetMouseDispFlag(true);

        InitUISingle();
        SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);
    }
}
void SceneLobby::InitUISingle(void)
{
    int uiType = 0;

    // マウスカーソル用のコライダー生成（半径1の円）
    cursorCollider_ = std::make_unique<Collider2DCircle>
        (Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);

    CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

    // 

    uiCollisionTags_.at(uiType) = Collider2DBase::TAG_2D::SELECT_SINGLE_BOMB;
    uiCollisions_.at(uiType)
        = std::make_unique<Collider2DBox>(POS_SELECT_BOMB, BUTTON_SIZE.x, BUTTON_SIZE.y
                , Collider2DBase::TAG_2D::SELECT_SINGLE_BOMB);

    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(uiType).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
            , Collider2DBase::TAG_2D::SELECT_SINGLE_BOMB, true);

    // 
    uiType++;
    uiCollisions_.at(uiType)
        = std::make_unique<Collider2DBox>(POS_SELECT_CANNON, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_BIG);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(uiType).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
            , Collider2DBase::TAG_2D::SELECT_SINGLE_BIG, true);

    // 
    uiType++;
    uiCollisions_.at(uiType)
        = std::make_unique<Collider2DBox>(POS_SELECT_RECOVERY, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RECOVERY);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(uiType).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RECOVERY, true);

    // 連射
    uiType++;
    uiCollisions_.at(uiType)
        = std::make_unique<Collider2DBox>(POS_SELECT_RAPIDFIRE, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RAPIDFIRE);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(uiType).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::SELECT_SINGLE_RAPIDFIRE, true);
}


void SceneLobby::Update(void)
{
    if (Loading::GetInstance()->IsLoading()) { return; }

	if (IS_MULTI)
	{
		UpdateMulti();
	}
	else
	{
		UpdateSingle();
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
        std::array<Vector2F, 4> UI_POS
            = { POS_SELECT_BOMB, POS_SELECT_CANNON, POS_SELECT_RECOVERY, POS_SELECT_RAPIDFIRE };
        //= { POS_SELECT_BOMB, POS_SELECT_RAPIDFIRE, POS_SELECT_CANNON, POS_SELECT_RECOVERY};

        for (int i = 0; i < uiCollisions_.size(); i++)
        {
            // 選択中の職業の画像以外は未選択の画像を表示
            int imageNum = i + ((selectedIdx_ != i) ? 1 : 0);

            DrawRotaGraph(UI_POS.at(i).x, UI_POS.at(i).y, BUTTON_SCALE,
                0.0, uiHandles_.at(imageNum), true);

#ifdef _DEBUG
            CollisionController::GetInstance().DrawDebug2D();
#endif
        }

        const Vector2 pos = {};
        DrawGraph(pos.x, pos.y, uiHandles_.at(static_cast<int>(UI_SINGLE::GAME_START)), true);

    }
 }

void SceneLobby::Release(void)
{
}


void SceneLobby::UpdateSingle(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();
	Vector2F stick = keyConfInputManager.GetLeftStickRaw();

    // スティック入力による選択インデックスの更新
    constexpr float THRESHOLD = 0.5f;
    constexpr int STICK_TINERVAL = 15;
    constexpr int MENU_MAX = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    // マウス座標の更新
    Vector2 mousePos = keyConfInputManager.GetMousePosition();
    Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    cursorCollider_->SetCenterPos(mousePosF);

    if (isSelectMenu_)
    {

        if (inputIntervalCounter_ > 0)
        {
            inputIntervalCounter_--;
        }
        else if (std::abs(stick.y) > THRESHOLD)
        {
            if (stick.y < 0.0f)
            {
                selectedIdx_ = ((selectedIdx_ + 1) % MENU_MAX);
            }
            else
            {
                selectedIdx_ = ((selectedIdx_ - 1 + MENU_MAX) % MENU_MAX);
            }
            inputIntervalCounter_ = STICK_TINERVAL;
        }
        
        // マウスが動いたときはパッドの選択カーソルも追従させる
        for (int i = 0; i < MENU_MAX; ++i)
        {
            if (CollisionController::GetInstance()
                    .IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
                        , uiCollisionTags_.at(selectedIdx_)))
            {
                selectedIdx_ = i;
                break;
            }
        }

        // 決定処理
        for (int i = 0; i < MENU_MAX; ++i)
        {
            bool isTarget = CollisionController::GetInstance()
                                .IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, uiCollisionTags_[i])
                                                            || (selectedIdx_ == i);

            if (isTarget && keyConfInputManager.isTrigerDown("OK"))
            {
                isSelectMenu_ = false;

                // 効果音再生
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
            }
        }
    }
    else if (!SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_SELECT))
    {
        // 効果音終了時に決定処理を実行
        auto jobs = { SceneGame::PlayerSelectType(PlayerBase::JOB_TYPE::SUPPORT, PlayerBase::SKIN_TYPE::DOG) };
        SceneManager::GetInstance()
            .ChangeScene(std::make_shared<SceneGame>(jobs));
    }
}

void SceneLobby::UpdateMulti(void)
{
    switch (multiState_)
    {
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

    // IPアドレス編集モード
    if (isEditing_)
    {
        if (KeyConfInputManager::GetInstance().isTrigerDown("CANCEL"))
        {
            isEditing_ = false;
            return;
        }

        if (KeyConfInputManager::GetInstance().isTrigerDown("LEFT"))
        {
            selectOctet_ = (selectOctet_ + 3) % 4;
        }

        if (KeyConfInputManager::GetInstance().isTrigerDown("RIGHT"))
        {
            selectOctet_ = (selectOctet_ + 1) % 4;
        }

        if (KeyConfInputManager::GetInstance().isTrigerDown("UP"))
        {
            passcode_[selectOctet_] = (passcode_[selectOctet_] + 1) % 10;
        }

        if (KeyConfInputManager::GetInstance().isTrigerDown("DOWN"))
        {
            passcode_[selectOctet_] = (passcode_[selectOctet_] + 9) % 10;
        }

        if (KeyConfInputManager::GetInstance().isTrigerDown("OK"))
        {
            isEditing_ = false;
        }
    }

    // モード選択(左右キー)
    if (KeyConfInputManager::GetInstance().isTrigerDown("LEFT")
        || KeyConfInputManager::GetInstance().isTrigerDown("RIGHT"))
    {
        buttonSelectIndex_ = (buttonSelectIndex_ == 0) ? 1 : 0;
    }

    // クライアント選択時、上キーでIP編集へ
    if (buttonSelectIndex_ == 1 && KeyConfInputManager::GetInstance().isTrigerDown("UP"))
    {
        isEditing_ = true;
        selectOctet_ = 0;
    }

    // 決定キーで通信開始
    if (KeyConfInputManager::GetInstance().isTrigerDown("OK"))
    {
        int roomWord = passcode_[0] * 1000 + passcode_[1] * 100 + 
            passcode_[2] * 10 + passcode_[3];

        NetManager::GetInstance().SetRoomWordId(roomWord);

        // ホスト
        if (buttonSelectIndex_ == 0)
        {
            NetManager::GetInstance().Run(NET_MODE::HOST);
            multiState_ = LOBBY_STATE::CONNECTING;
        }
        //クライアント
        else if (buttonSelectIndex_ == 1)
        {
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
    // ホストなら即時遷移
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
    // クライアント側：ホストからの遷移命令を受信して遷移
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

    // キャンセルキーで退出
    if (KeyConfInputManager::GetInstance().isTrigerDown("DEBUG_CANCEL"))
    {
        NetManager::GetInstance().Stop();
        Initialize();
        return;
    }

    // 全員の準備完了状態をチェック
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

    // ホスト側：全員準備完了時にOKキーで出撃
    if (isAllReady && NetManager::GetInstance().IsHost() && !users.empty())
    {
        if (KeyConfInputManager::GetInstance().isTrigerDown("OK"))
        {
            self.gameState = GAME_STATE::GAME_PLAYING;
            NetManager::GetInstance().SetSelfInfo(self);

            // 全員にゲーム開始命令を送信
            NetManager::GetInstance().Send(NET_DATA_TYPE::GO_GAME_SCENE);

            MoveToGameScene(users);
            return;
        }
    }
    else
    {
        // 通常の準備完了切り替え
        if (KeyConfInputManager::GetInstance().isTrigerDown("OK"))
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
    int btnStartX = (screenWidth - (180 * 2 + 20)) / 2;

    // IPアドレスボックス
    DrawBox(ipBoxX, 250, ipBoxX + 400, 310, GetColor(30, 30, 30), true);
    DrawBox(ipBoxX, 250, ipBoxX + 400, 310, isEditing_ ? COLOR_YELLOW : COLOR_WHITE, false);
    std::string passStr = std::to_string(passcode_[0]) + std::to_string(passcode_[1]) +
        std::to_string(passcode_[2]) + std::to_string(passcode_[3]);
    DrawString(ipBoxX + 20, 272, ("Room Passcode: " + passStr).c_str(), COLOR_WHITE);

    if (isEditing_)
    {
        DrawFormatString(ipBoxX + 20, 320, COLOR_YELLOW, "編集中: %d桁目", selectOctet_ + 1);
    }

    // HOSTボタン
    DrawBox(btnStartX, 400, btnStartX + 180, 460, GetColor(0, 100, 0), TRUE);
    DrawBox(btnStartX, 400, btnStartX + 180, 460, (!isEditing_ && buttonSelectIndex_ == 0) ? 
        COLOR_YELLOW : COLOR_WHITE, false);
    DrawString(btnStartX + 70, 422, "HOST", COLOR_WHITE);

    // CLIENTボタン
    int clientX = btnStartX + 200;
    DrawBox(clientX, 400, clientX + 180, 460, GetColor(0, 0, 150), true);
    DrawBox(clientX, 400, clientX + 180, 460, (!isEditing_ && buttonSelectIndex_ == 1)
        ? COLOR_YELLOW : COLOR_WHITE, false);
    DrawString(clientX + 65, 422, "CLIENT", COLOR_WHITE);
}

void SceneLobby::DrawConnecting(void)
{
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(10, 10, 15), true);
    DrawString(100, 100, "ホストへの接続を待機しています...", COLOR_YELLOW);
    DrawString(100, 130, "[BackSpace] キャンセル", COLOR_WHITE);
}

void SceneLobby::DrawInRoom(void)
{
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(10, 10, 15), true);
    DrawString(100, 50, "■ ルーム内プレイヤー一覧", COLOR_GREEN);

    int drawCount = 0;

    // まず「自分」を一番上に描画する
    NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
    std::string myRole = NetManager::GetInstance().IsHost() ? "HOST" : "CLIENT";
    std::string myStatus = (self.gameState >= GAME_STATE::GOTO_GAME) ? "READY!!" : "Waiting...";
    unsigned int myStatusCol = (self.gameState >= GAME_STATE::GOTO_GAME) ? COLOR_GREEN : COLOR_WHITE;

    // ポート番号が -1 なら AUTO にする処理
    std::string myPortStr = (self.port == -1) ? "AUTO" : std::to_string(self.port);

    int y = 120 + drawCount * 40;
    DrawFormatString(100, y, COLOR_WHITE, "Player %d : IP = %d.%d.%d.%d:%s",
        drawCount + 1, self.ip.d1, self.ip.d2, self.ip.d3, self.ip.d4, myPortStr.c_str());
    DrawFormatString(500, y, COLOR_YELLOW, "[%s] (YOU)", myRole.c_str());
    DrawFormatString(650, y, myStatusCol, myStatus.c_str());
    drawCount++;

    // 続いて「接続してきた相手」を描画する
    for (const auto& pair : NetManager::GetInstance().GetNetUsers())
    {
        if (drawCount >= MAX_PLAYERS) break;

        const NET_JOIN_USER& user = pair.second;
        std::string role = (user.mode == NET_MODE::HOST) ? "HOST" : "CLIENT";
        std::string status = (user.gameState >= GAME_STATE::GOTO_GAME) ? "READY!!" : "Waiting...";
        unsigned int statusCol = (user.gameState >= GAME_STATE::GOTO_GAME) ? COLOR_GREEN : COLOR_WHITE;

        // ポート番号が -1 なら AUTO にする処理
        std::string userPortStr = (user.port == -1) ? "AUTO" : std::to_string(user.port);

        y = 120 + drawCount * 40;
        DrawFormatString(100, y, COLOR_WHITE, "Player %d : IP = %d.%d.%d.%d:%s",
            drawCount + 1, user.ip.d1, user.ip.d2, user.ip.d3, user.ip.d4, userPortStr.c_str());
        DrawFormatString(500, y, COLOR_YELLOW, "[%s]", role.c_str());
        DrawFormatString(650, y, statusCol, status.c_str());
        drawCount++;
    }

    // 余ったスロットを「空き」として描画する
    for (int i = drawCount; i < MAX_PLAYERS; ++i)
    {
        int emptyY = 120 + i * 40;
        DrawFormatString(100, emptyY, COLOR_GRAY, "Player %d : ---- Empty ----", i + 1);
    }

    DrawString(100, 400, "[Enter] 準備完了(READY)を切り替え", COLOR_YELLOW);
    DrawString(100, 430, "[BackSpace] 退出する", COLOR_WHITE);

    auto users = NetManager::GetInstance().GetNetUsers(); 
    bool isAllReady = (self.gameState == GAME_STATE::GOTO_GAME);

    for (const auto& pair : users)
    {
        if (pair.second.gameState != GAME_STATE::GOTO_GAME)
        {
            isAllReady = false;
            break;
        }
    }

    if (isAllReady && NetManager::GetInstance().IsHost() && !users.empty())
    {
        DrawString(100, 480, "出撃 [OKキーで開始]", GetColor(255, 255, 0));
    }
}

void SceneLobby::MoveToGameScene(std::map<int, NET_JOIN_USER>& _users)
{
    // 自分
    std::vector<PlayerBase::JOB_TYPE> playerJobs;
    playerJobs.push_back(PlayerBase::JOB_TYPE::CANNON);
    
    // 他人
    for (int i = 0; i < _users.size(); ++i) 
    {
        playerJobs.push_back(PlayerBase::JOB_TYPE::BOMB);
    }
    SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(playerJobs));
}
