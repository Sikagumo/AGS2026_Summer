#include "SceneLobby.h"

#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Camera/Camera.h"
#include "../../Application.h"

SceneLobby::SceneLobby(bool _isMulti)
	: SceneBase::SceneBase()
	, isMulti_(_isMulti)
{
}


void SceneLobby::Load(void)
{
}

void SceneLobby::EndLoad(void)
{
}

void SceneLobby::Initialize(void)
{
    if (isMulti_)
    {
    }
    else
    {
        InitUISingle();
        SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);
    }
}
void SceneLobby::InitUISingle(void)
{
    // UIボタンの配置計算設定
    constexpr Vector2F BUTTON_SIZE = { 400.0f, 250.0f };
    const Vector2F UI_POS = { Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y + 20 };
    const float INTERVAL_Y = 100.0f;

    int uiType = 0;

    // 
    const Vector2F POS_SELECT_BOMB = Vector2F(UI_POS.x, UI_POS.y);

    uiType = static_cast<int>(UI_SINGLE::BOMB);

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
    const Vector2F POS_SELECT_BIG = Vector2F(UI_POS.x, UI_POS.y + INTERVAL_Y);
    uiCollisions_.at(static_cast<int>(UI_SINGLE::BIG))
        = std::make_unique<Collider2DBox>(POS_SELECT_BIG, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_BIG);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(static_cast<int>(UI_SINGLE::BIG)).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
            , Collider2DBase::TAG_2D::SELECT_SINGLE_BIG, true);

    // 
    const Vector2F POS_SELECT_RECOVERY = Vector2F(UI_POS.x, UI_POS.y + (INTERVAL_Y * 2));
    uiCollisions_.at(static_cast<int>(UI_SINGLE::RECOVERY))
        = std::make_unique<Collider2DBox>(POS_SELECT_RECOVERY, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RECOVERY);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(static_cast<int>(UI_SINGLE::RECOVERY)).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RECOVERY, true);

    // 連射
    const Vector2F POS_SELECT_RAPIDFIRE = Vector2F(UI_POS.x, UI_POS.y + (INTERVAL_Y * 3));
    uiCollisions_.at(static_cast<int>(UI_SINGLE::RAPID_FIRE))
        = std::make_unique<Collider2DBox>(POS_SELECT_RAPIDFIRE, BUTTON_SIZE.x, BUTTON_SIZE.y
            , Collider2DBase::TAG_2D::SELECT_SINGLE_RAPIDFIRE);
    CollisionController::GetInstance()
        .RegisterCollider2D(uiCollisions_.at(static_cast<int>(UI_SINGLE::RAPID_FIRE)).get());
    CollisionController::GetInstance()
        .SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR,
        Collider2DBase::TAG_2D::SELECT_SINGLE_RAPIDFIRE, true);
}


void SceneLobby::Update(void)
{
	if (isMulti_)
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
}

void SceneLobby::Release(void)
{
}


void SceneLobby::UpdateSingle(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();
	Vector2F stick = keyConfInputManager.GetLeftStickRaw();

    // スティック入力による選択インデックスの更新
    int inputIntervalCounter = 0;
    constexpr float THRESHOLD = 0.5f;
    constexpr int STICK_TINERVAL = 15;
    constexpr int MENU_MAX = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

    if (isSelectMenu_)
    {

        if (inputIntervalCounter > 0)
        {
            inputIntervalCounter--;
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
            inputIntervalCounter = STICK_TINERVAL;
        }
        /*
        // マウスが動いたときはパッドの選択カーソルも追従させる
        for (int i = 0; i < MENU_MAX; ++i)
        {
            if (CollisionController::GetInstance()
                    .IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR
                        , uiCollisions_.at(selectedIdx_)))
            {
                selectedIdx_ = i;
                break;
            }
        }
        
        // 決定処理
        for (int i = 0; i < MENU_MAX; ++i)
        {
            bool isTarget = CollisionController::GetInstance().IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, buttonTags[i]) || (selectedIdx_ == i);

            if (isTarget && keyConfInputManager.isTrigerDown("OK"))
            {
                isSelectMenu_ = false;

                // 効果音再生
                SoundManager::GetInstance()
                    .Play(SoundManager::SOUND::SE_SELECT);
            }
        }
        */
    }

	//if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_W)
}

void SceneLobby::UpdateMulti(void)
{
}
