#include "SceneLobby.h"

#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
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
}
