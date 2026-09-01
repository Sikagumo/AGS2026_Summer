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
	, isPhaseChanged_(false)
	, selectTextHandles_()
	, noSelectTextHandles_()
	, backGroundHandle_(-1)
	, cursorCollider_(nullptr)
	, returnGameButtonCollider_(nullptr)
	, returnTitleButtonCollider_(nullptr)
	, yesButtonCollider_(nullptr)
	, noButtonCollider_(nullptr)
{
	selectTextHandles_.fill(-1);
	noSelectTextHandles_.fill(-1);
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
		Vector2F(0.0f, 0.0f), 10.0f, Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR);

	const Vector2F GAME_IN(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(Application::SCREEN_HALF_Y - 50));

	returnGameButtonCollider_ = std::make_unique<Collider2DBox>(GAME_IN,
		BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::PAUSE_GAME_BUTTON);

	const Vector2F RETURN_TITLE(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(Application::SCREEN_HALF_Y + 50));

	returnTitleButtonCollider_ = std::make_unique<Collider2DBox>(RETURN_TITLE,
		BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::PAUSE_TITLE_BUTTON);

	// YES/NOダイアログ用のボタンの中心座標を計算
	const int CENTER_X = Application::SCREEN_HALF_X;
	const int CENTER_Y = Application::SCREEN_HALF_Y;
	const int TOP = CENTER_Y - 150 / 2;
	const int LEFT = CENTER_X - 300 / 2;
	const int DRAW_X = LEFT + 50;
	const int DRAW_Y = TOP + 60;

	const Vector2F YES_BUTTON_CENTER(static_cast<float>(DRAW_X + 20), static_cast<float>(DRAW_Y + 15));
	yesButtonCollider_ = std::make_unique<Collider2DBox>(
		YES_BUTTON_CENTER, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT,
		Collider2DBase::TAG_2D::PAUSE_YESNO_BUTTON);

	const Vector2F NO_BUTTON_CENTER(static_cast<float>(DRAW_X + 200), static_cast<float>(DRAW_Y + 15));
	noButtonCollider_ = std::make_unique<Collider2DBox>(
		NO_BUTTON_CENTER, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT,
		Collider2DBase::TAG_2D::SELECT_SINGLE_BIG);

	// コライダーの登録
	collisionController.RegisterCollider2D(cursorCollider_.get());
	collisionController.RegisterCollider2D(returnGameButtonCollider_.get());
	collisionController.RegisterCollider2D(returnTitleButtonCollider_.get());
	collisionController.RegisterCollider2D(yesButtonCollider_.get());
	collisionController.RegisterCollider2D(noButtonCollider_.get());

	// YES/NO用のタグとマウスカーソルの当たり判定を有効化する
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_GAME_BUTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_TITLE_BUTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR,
		Collider2DBase::TAG_2D::PAUSE_YESNO_BUTTON, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR,
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

	const Vector2 MOUSE_POSITION = inputManager.GetMousePosition();
	const Vector2F MOUSE_POSITION_F(static_cast<float>(MOUSE_POSITION.x), static_cast<float>(MOUSE_POSITION.y));
	cursorCollider_->SetCenterPos(MOUSE_POSITION_F);

	// マウスによるホバー処理
	bool isHoverGame = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_GAME_BUTTON);
	bool isHoverTitle = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_TITLE_BUTTON);

	if (isHoverGame)
	{
		selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_GAME);
	}
	else if (isHoverTitle)
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
		if (isHoverGame || isHoverTitle)
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

	const Vector2 MOUSE_POSITION = inputManager.GetMousePosition();
	const Vector2F MOUSE_POSITION_F(static_cast<float>(MOUSE_POSITION.x), static_cast<float>(MOUSE_POSITION.y));
	cursorCollider_->SetCenterPos(MOUSE_POSITION_F);

	if (isPhaseChanged_)
	{
		isPhaseChanged_ = false;
		return;
	}

	// マウスによるホバー処理
	bool isHoverYes = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_YESNO_BUTTON);
	bool isHoverNo = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::SELECT_SINGLE_BIG);

	if (isHoverYes)
	{
		isYes_ = true;
	}
	else if (isHoverNo)
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
		if (isHoverYes || isHoverNo)
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

	const int DRAW_Y = Application::SCREEN_HALF_Y;
	int drawX = Application::SCREEN_HALF_X - 100;

	DrawRotaGraph(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(Application::SCREEN_HALF_Y),
		0.7f, 0.0f, backGroundHandle_, true);

	// はい描画
	const int YES_HANDLE = GetTextUIHandle(PAUSE_TEXT_UI::YES, isYes_);
	if (YES_HANDLE != -1)
	{
		DrawRotaGraph(static_cast<float>(drawX), static_cast<float>(DRAW_Y), 0.6f, 0.0f, YES_HANDLE, true);
	}

	// いいえ描画
	drawX = drawX + 200;

	const int NO_HANDLE = GetTextUIHandle(PAUSE_TEXT_UI::NO, !isYes_);
	if (NO_HANDLE != -1)
	{
		DrawRotaGraph(static_cast<float>(drawX), static_cast<float>(DRAW_Y), 0.6f, 0.0f, NO_HANDLE, true);
	}
}

void ScenePause::DrawFrame(float _rate)
{
	int width = 0;
	int height = 0;

	constexpr float UI_BACK_SCALE_X = 1.0f;
	constexpr float UI_BACK_SCALE_Y = 1.5f;

	GetGraphSize(backGroundHandle_, &width, &height);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph3(
		static_cast<float>(Application::SCREEN_HALF_X),
		static_cast<float>(Application::SCREEN_HALF_Y),
		static_cast<float>(width / 2),
		static_cast<float>(height / 2),
		UI_BACK_SCALE_X,
		UI_BACK_SCALE_Y,
		0.0f,
		backGroundHandle_,
		true
	);
}

void ScenePause::DrawMenu(void)
{
	int drawY = MENU_TOP_OFFSET;

	for (size_t i = 0; i < static_cast<size_t>(MENU_ITEM::COUNT); ++i)
	{
		const auto CURRENT_MENU_ITEM = static_cast<MENU_ITEM>(i);
		const bool IS_SELECTED = (selectedIndex_ == static_cast<int>(i));

		PAUSE_TEXT_UI textType = PAUSE_TEXT_UI::RETURN_GAME;
		if (CURRENT_MENU_ITEM == MENU_ITEM::RETURN_TITLE)
		{
			textType = PAUSE_TEXT_UI::RETURN_TITLE;
		}

		const int UI_HANDLE = GetTextUIHandle(textType, IS_SELECTED);

		if (UI_HANDLE != -1)
		{
			DrawRotaGraph(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(drawY),
				0.6f, 0.0f, UI_HANDLE, true);
		}

		drawY = drawY + MENU_ITEM_HEIGHT;
	}
}

int ScenePause::GetTextUIHandle(PAUSE_TEXT_UI _textType, bool _isSelected) const
{
	const size_t INDEX = static_cast<size_t>(_textType);

	if (_isSelected)
	{
		if (INDEX < selectTextHandles_.size())
		{
			return selectTextHandles_.at(INDEX);
		}
	}
	else
	{
		if (INDEX < noSelectTextHandles_.size())
		{
			return noSelectTextHandles_.at(INDEX);
		}
	}

	return -1;
}