#include "ScenePause.h"

#include <DxLib.h>
#include <cmath>

#include "../../Manager/Generic/SceneManager.h" 
#include "../MainScene/SceneTitle.h"
#include "../../Manager/Generic/KeyConfInputManager.h" 
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"

ScenePause::ScenePause(void)
	: currentPhase_(PHASE::APPEAR)
	, frame_(0)
	, selectedIndex_(0)
	, isYes_(false)
	, inputIntervalCounter_(0)
	, selectTextHandles_{}
	, noSelectTextHandles_{}
	, backGroundHandle_(-1)
	, isPhaseChanged_(false)
{
	for (auto& handle : selectTextHandles_)
	{
		handle = -1;
	}
	for (auto& handle : noSelectTextHandles_)
	{
		handle = -1;
	}
}

ScenePause::~ScenePause(void)
{
}

void ScenePause::Load(void)
{
	SceneBase::Load();

	
	ResourceManager::GetInstance().LoadHandleIds(
		ResourceManager::SRC::IMGS_SELECT_PUSE_TEX, selectTextHandles_.data());


	ResourceManager::GetInstance().LoadHandleIds(
		ResourceManager::SRC::IMGS_POUSE_TEX, noSelectTextHandles_.data());

	backGroundHandle_ = ResourceManager::GetInstance().
		LoadHandleId(ResourceManager::SRC::IMG_SELECT_UI_BACK);
}

void ScenePause::EndLoad(void)
{
	SceneBase::EndLoad();
}

void ScenePause::Initialize(void)
{
	SceneBase::Initialize();
	inputIntervalCounter_ = 0;

	auto& collisionController = CollisionController::GetInstance();

	// マウスカーソル用の円コライダー
	cursorCollider_ = std::make_unique<Collider2DCircle>(
		Vector2F(0.0f, 0.0f), 10.0f, Collider2DBase::TAG_2D::PAUSE_MAOUSE_CURSOR);

	Vector2F gameIn(Application::SCREEN_HALF_X, (Application::SCREEN_HALF_Y - 50));

	returnGameButtonCollider_ = std::make_unique<Collider2DBox>(gameIn,
		BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::PAUSE_GAME_BUTTON);

	Vector2F returnTitle(Application::SCREEN_HALF_X, (Application::SCREEN_HALF_Y + 50));

	returnTitleButtonCollider_ = std::make_unique<Collider2DBox>(returnTitle,
		BUTTON_WIDTH, BUTTON_HEIGHT,Collider2DBase::TAG_2D::PAUSE_TITLE_BUTTON);

	// YES/NOダイアログ用のボタンの中心座標を計算
	int centerX = Application::SCREEN_HALF_X;
	int centerY = Application::SCREEN_HALF_Y;
	int top = centerY - 150 / 2;
	int left = centerX - 300 / 2;
	int drawX = left + 50;
	int drawY = top + 60;

	Vector2F yesButtonCenter(static_cast<float>(drawX + 20), static_cast<float>(drawY + 15));
	yesButtonCollider_ = std::make_unique<Collider2DBox>(
		yesButtonCenter, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT,
		Collider2DBase::TAG_2D::PAUSE_YESNO_BOTTON);

	Vector2F noButtonCenter(static_cast<float>(drawX + 200), static_cast<float>(drawY + 15));
	noButtonCollider_ = std::make_unique<Collider2DBox>(
		noButtonCenter, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT, 
		Collider2DBase::TAG_2D::SELECT_SINGLE_BIG);

	// コライダーの登録
	collisionController.RegisterCollider2D(cursorCollider_.get());
	collisionController.RegisterCollider2D(returnGameButtonCollider_.get());
	collisionController.RegisterCollider2D(returnTitleButtonCollider_.get());
	collisionController.RegisterCollider2D(yesButtonCollider_.get());
	collisionController.RegisterCollider2D(noButtonCollider_.get());

	// YES/NO用のタグとマウスカーソルの当たり判定を有効化する
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MAOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_GAME_BUTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MAOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_TITLE_BUTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MAOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_YESNO_BOTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MAOUSE_CURSOR,
		Collider2DBase::TAG_2D::SELECT_SINGLE_BIG, true);
}

void ScenePause::Update(void)
{
	switch (currentPhase_)
	{
	case ScenePause::PHASE::APPEAR:
		ProcessAppear();
		break;

	case ScenePause::PHASE::NORMAL:
		ProcessNormal();
		break;

	case ScenePause::PHASE::DISAPPEAR:
		ProcessDisappear();
		break;

	case ScenePause::PHASE::YES_NO:
		ProcessYesNo();
		break;
	}
}

void ScenePause::Draw(void)
{
	switch (currentPhase_)
	{
	case ScenePause::PHASE::APPEAR:
		DrawAppear();
		break;

	case ScenePause::PHASE::NORMAL:
		DrawNormal();
		break;

	case ScenePause::PHASE::DISAPPEAR:
		DrawAppear();
		break;

	case ScenePause::PHASE::YES_NO:
		DrawYesNo();
		break;
	}

#ifdef _DEBUG
	CollisionController::GetInstance().DrawDebug2D();
#endif
}

void ScenePause::Release(void)
{
	auto& collisionController = CollisionController::GetInstance();
	collisionController.UnregisterCollider2D(cursorCollider_.get());
	collisionController.UnregisterCollider2D(returnGameButtonCollider_.get());
	collisionController.UnregisterCollider2D(returnTitleButtonCollider_.get());
	collisionController.UnregisterCollider2D(yesButtonCollider_.get());
	collisionController.UnregisterCollider2D(noButtonCollider_.get());
}

void ScenePause::ProcessAppear(void)
{
	frame_ = frame_ + 1;
	if (frame_ >= EXPAND_INTERVAL)
	{
		currentPhase_ = PHASE::NORMAL;
		return;
	}
}

void ScenePause::ProcessNormal(void)
{
	auto& inputManager = KeyConfInputManager::GetInstance();
	auto& collisionController = CollisionController::GetInstance();
	using TAG = Collider2DBase::TAG_2D;

	Vector2 mousePosition = inputManager.GetMousePosition();
	Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	cursorCollider_->SetCenterPos(mousePositionF);

	// マウスによるホバー処理
	if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_GAME_BUTTON))
	{
		selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_GAME);
	}
	else if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_TITLE_BUTTON))
	{
		selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_TITLE);
	}

	// インターバルのカウントダウン
	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_ = inputIntervalCounter_ - 1;
	}

	// キーボード・パッド・スティック操作
	if (inputIntervalCounter_ <= 0)
	{
		if (inputManager.isPressed("UP"))
		{
			selectedIndex_ = (selectedIndex_ + static_cast<int>(MENU_ITEM::COUNT) - 1) % 
				static_cast<int>(MENU_ITEM::COUNT);

			inputIntervalCounter_ = STICK_INTERVAL;
		}
		else if (inputManager.isPressed("DOWN"))
		{
			selectedIndex_ = (selectedIndex_ + 1) % static_cast<int>(MENU_ITEM::COUNT);

			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	// 上下の入力が全くない場合はインターバルをリセットする
	if (inputManager.isPressed("UP") == false && inputManager.isPressed("DOWN") == false)
	{
		inputIntervalCounter_ = 0;
	}

	if (inputManager.isTrigerDown("PAUSE") || inputManager.isTrigerDown("CANCEL"))
	{
		currentPhase_ = PHASE::DISAPPEAR;
		return;
	}

	bool isMouseClicked = false;
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_GAME_BUTTON) ||
			collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_TITLE_BUTTON))
		{
			isMouseClicked = true;
		}
	}

	if (inputManager.isTrigerDown("OK") || isMouseClicked)
	{
		ProcessMenuState();
	}
}

void ScenePause::ProcessMenuState(void)
{
	switch (static_cast<MENU_ITEM>(selectedIndex_))
	{
	case MENU_ITEM::RETURN_GAME:
		currentPhase_ = PHASE::DISAPPEAR;
		break;

	case MENU_ITEM::RETURN_TITLE:
		currentPhase_ = PHASE::YES_NO;
		isPhaseChanged_ = true;
		break;

	default:
		break;
	}
}

void ScenePause::ProcessDisappear(void)
{
	frame_ = frame_ - 1;
	if (frame_ <= 0)
	{
		SceneManager::GetInstance().PopScene();
		return;
	}
}

void ScenePause::ProcessYesNo(void)
{
	auto& inputManager = KeyConfInputManager::GetInstance();
	auto& collisionController = CollisionController::GetInstance();
	using TAG = Collider2DBase::TAG_2D;

	Vector2 mousePosition = inputManager.GetMousePosition();
	Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	cursorCollider_->SetCenterPos(mousePositionF);

	if (isPhaseChanged_)
	{
		isPhaseChanged_ = false;
		return;
	}

	// マウスによるホバー処理
	if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_YESNO_BOTTON))
	{
		isYes_ = true;
	}
	else if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::SELECT_SINGLE_BIG))
	{
		isYes_ = false;
	}

	// インターバルのカウントダウン
	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_ = inputIntervalCounter_ - 1;
	}

	// キーボード・パッド・スティック操作
	if (inputIntervalCounter_ <= 0)
	{
		if (inputManager.isPressed("LEFT") || inputManager.isPressed("RIGHT"))
		{
			isYes_ = !isYes_;

			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	// 左右の入力が全くない場合はインターバルをリセットする
	if (inputManager.isPressed("LEFT") == false && inputManager.isPressed("RIGHT") == false)
	{
		inputIntervalCounter_ = 0;
	}

	if (inputManager.isTrigerDown("CANCEL"))
	{
		currentPhase_ = PHASE::NORMAL;
		return;
	}

	bool isMouseClicked = false;
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		if (collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::PAUSE_YESNO_BOTTON) ||
			collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MAOUSE_CURSOR, TAG::SELECT_SINGLE_BIG))
		{
			isMouseClicked = true;
		}
	}

	if (inputManager.isTrigerDown("OK") || isMouseClicked)
	{
		if (isYes_)
		{
			ExecuteYesAction();
		}
		else
		{
			currentPhase_ = PHASE::NORMAL;
		}
	}
}

void ScenePause::ExecuteYesAction(void)
{
	SceneManager::GetInstance().ChangeScene(std::make_shared<SceneTitle>());
}

void ScenePause::DrawAppear(void)
{
	DrawFrame(static_cast<float>(frame_) / static_cast<float>(EXPAND_INTERVAL));
}

void ScenePause::DrawNormal(void)
{
	DrawFrame(1.0f);
	DrawMenu();
}

void ScenePause::DrawYesNo(void)
{
	DrawNormal();

	int drawY = Application::SCREEN_HALF_Y;
	int drawX = Application::SCREEN_HALF_X- 100;


	DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y,
		0.7f, 0.0f, backGroundHandle_, true);

	// はい描画
	int yesHandle = GetTextUIHandle(PAUSE_TEXT_UI::YES, isYes_);
	if (yesHandle != -1)
	{
		DrawRotaGraph(drawX, drawY, 0.6f, 0.0f, yesHandle, true);
	}


	// いいえ描画
	drawX = drawX + 200;

	int noHandle = GetTextUIHandle(PAUSE_TEXT_UI::NO, !isYes_);
	if (noHandle != -1)
	{
		DrawRotaGraph(drawX, drawY, 0.6f, 0.0f, noHandle, true);
	}

}

void ScenePause::DrawFrame(float rate)
{
	int width = 0;
	int height = 0;

	float uiBackScaleX = 1.0f;

	float uiBackScaleY = 1.5f;
	
	GetGraphSize(backGroundHandle_, &width, &height);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph3(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, width / 2, height / 2,
		uiBackScaleX, uiBackScaleY, 0.0f, backGroundHandle_, true);
}

void ScenePause::DrawMenu(void)
{

	int drawY = MENU_TOP_OFFSET; 

	for (size_t i = 0; i < static_cast<size_t>(MENU_ITEM::COUNT); ++i)
	{
		auto currentMenuItem = static_cast<MENU_ITEM>(i);
		bool isSelected = (selectedIndex_ == static_cast<int>(i));

		PAUSE_TEXT_UI textType = PAUSE_TEXT_UI::RETURN_GAME;
		if (currentMenuItem == MENU_ITEM::RETURN_TITLE)
		{
			textType = PAUSE_TEXT_UI::RETURN_TITLE;
		}

		int uiHandle = GetTextUIHandle(textType, isSelected);

		if (uiHandle != -1)
		{
			DrawRotaGraph(Application::SCREEN_HALF_X, drawY,
				0.6f, 0.0f, uiHandle, true);
		}

		drawY = drawY + MENU_ITEM_HEIGHT;
	}
}

int ScenePause::GetTextUIHandle(PAUSE_TEXT_UI textType, bool isSelected) const
{
	size_t index = static_cast<size_t>(textType);

	if (isSelected)
	{
		if (index < selectTextHandles_.size())
		{
			return selectTextHandles_.at(index);
		}
	}
	else
	{
		if (index < noSelectTextHandles_.size())
		{
			return noSelectTextHandles_.at(index);
		}
	}

	return -1;
}