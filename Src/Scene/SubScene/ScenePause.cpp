#include "ScenePause.h"

#include <cmath>
#include <DxLib.h>

#include "../../Manager/Generic/SceneManager.h" 
#include "../MainScene/SceneTitle.h"
#include "../../Manager/Generic/KeyConfInputManager.h" 
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1;								 // 無効なリソースハンドルを示す値
	constexpr float CURSOR_RADIUS = 10.0f;							 // マウスカーソル用円コライダーの半径
	constexpr Vector2F CURSOR_OUTOF_SCREEN = { -1000.0f, -1000.0f }; // 画面外にカーソルを退避させる座標
	constexpr float MOUSE_MOVE_THRESHOLD = 2.0f;					 // マウス移動判定のしきい値
	constexpr float GAME_BUTTON_OFFSET_Y = 50.0f;					 // ゲームに戻るボタンのY方向オフセット
	constexpr float TITLE_BUTTON_OFFSET_Y = 50.0f;					 // タイトルに戻るボタンのY方向オフセット
	constexpr int DIALOG_WIDTH = 300;						         // YES/NOダイアログの幅
	constexpr int DIALOG_HEIGHT = 150;								 // YES/NOダイアログの高さ
	constexpr int DIALOG_DRAW_OFFSET_X = 50;						 // ダイアログ内の描画Xオフセット
	constexpr int DIALOG_DRAW_OFFSET_Y = 60;						 // ダイアログ内の描画Yオフセット
	constexpr float YES_BUTTON_OFFSET_X = 20.0f;					 // 「はい」ボタンのXオフセット
	constexpr float YES_BUTTON_OFFSET_Y = 15.0f;					 // 「はい」ボタンのYオフセット
	constexpr float NO_BUTTON_OFFSET_X = 200.0f;				     // 「いいえ」ボタンのXオフセット
	constexpr float NO_BUTTON_OFFSET_Y = 15.0f;						 // 「いいえ」ボタンのYオフセット
	constexpr unsigned int BLEND_ALPHA_HALF = 128;					 // 半透明ブレンドのアルファ値
	constexpr int BACKGROUND_COLOR_BLACK = 0x000000;				 // 背景を描画する黒色のカラーコード
	constexpr float BACKGROUND_SCALE = 0.7f;						 // 背景ダイアログのスケール
	constexpr float TEXT_SCALE = 0.6f;								 // テキストUIのスケール
	constexpr float UI_BACK_SCALE_X = 1.0f;							 // 背景枠のX方向拡大率
	constexpr float UI_BACK_SCALE_Y = 1.5f;							 // 背景枠のY方向拡大率
	constexpr int YES_NO_TEXT_OFFSET_X = 100;						 // YES/NOテキスト表示時のX方向オフセット
	constexpr int NO_BUTTON_X_SPACING = 200;				         // 「いいえ」ボタンの配置間隔
}

ScenePause::ScenePause(void)
	: currentPhase_(PHASE::APPEAR)
	, frame_(0)
	, selectedIndex_(0)
	, isYes_(false)
	, inputIntervalCounter_(0)
	, isPhaseChanged_(false)
	, prevMousePos_(0.0f, 0.0f)
	, isMouseMode_(true)
	, selectTextHandles_()
	, noSelectTextHandles_()
	, backGroundHandle_(INVALID_HANDLE)
	, cursorCollider_(nullptr)
	, returnGameButtonCollider_(nullptr)
	, returnTitleButtonCollider_(nullptr)
	, yesButtonCollider_(nullptr)
	, noButtonCollider_(nullptr)
{
	selectTextHandles_.fill(INVALID_HANDLE);
	noSelectTextHandles_.fill(INVALID_HANDLE);
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

	SetMouseDispFlag(true);

	const auto mousePos = KeyConfInputManager::GetInstance().GetMousePosition();
	prevMousePos_ = Vector2F(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

	auto& collisionController = CollisionController::GetInstance();

	// マウスカーソル用の円コライダー
	cursorCollider_ = std::make_unique<Collider2DCircle>(
		Vector2F(0.0f, 0.0f), CURSOR_RADIUS, Collider2DBase::TAG_2D::PAUSE_MOUSE_CURSOR);

	const Vector2F gameIn(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(Application::SCREEN_HALF_Y - GAME_BUTTON_OFFSET_Y));

	returnGameButtonCollider_ = std::make_unique<Collider2DBox>(gameIn,
		BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::PAUSE_GAME_BUTTON);

	const Vector2F returnTitle(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(Application::SCREEN_HALF_Y + TITLE_BUTTON_OFFSET_Y));

	returnTitleButtonCollider_ = std::make_unique<Collider2DBox>(returnTitle,
		BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::PAUSE_TITLE_BUTTON);

	// YES/NOダイアログ用のボタンの中心座標を計算
	const int centerX = Application::SCREEN_HALF_X;
	const int centerY = Application::SCREEN_HALF_Y;
	const int top = centerY - DIALOG_HEIGHT / 2;
	const int left = centerX - DIALOG_WIDTH / 2;
	const int drawX = left + DIALOG_DRAW_OFFSET_X;
	const int drawY = top + DIALOG_DRAW_OFFSET_Y;

	const Vector2F yesButtonCenter(static_cast<float>(drawX) + YES_BUTTON_OFFSET_X, static_cast<float>(drawY) + YES_BUTTON_OFFSET_Y);
	yesButtonCollider_ = std::make_unique<Collider2DBox>(
		yesButtonCenter, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT,
		Collider2DBase::TAG_2D::PAUSE_YESNO_BUTTON);

	const Vector2F noButtonCenter(static_cast<float>(drawX) + NO_BUTTON_OFFSET_X, static_cast<float>(drawY) + NO_BUTTON_OFFSET_Y);
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
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	// 現在のマウス座標を取得
	const Vector2 mousePosition = keyConfInputManager.GetMousePosition();
	const Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

	// マウスが動いた、またはクリックされたらマウスモードへ
	if (std::abs(mousePositionF.x - prevMousePos_.x) > MOUSE_MOVE_THRESHOLD ||
		std::abs(mousePositionF.y - prevMousePos_.y) > MOUSE_MOVE_THRESHOLD ||
		(GetMouseInput() & (MOUSE_INPUT_LEFT | MOUSE_INPUT_RIGHT)) != 0)
	{
		isMouseMode_ = true;
		SetMouseDispFlag(true);
		prevMousePos_ = mousePositionF;
	}

	// コントローラーやキーボードのUI操作入力があったらコントローラーモードへ
	if (keyConfInputManager.IsPressed("UP") || keyConfInputManager.IsPressed("DOWN") ||
		keyConfInputManager.IsPressed("LEFT") || keyConfInputManager.IsPressed("RIGHT") ||
		keyConfInputManager.IsTriggerDown("OK") || keyConfInputManager.IsTriggerDown("CANCEL") ||
		keyConfInputManager.IsTriggerDown("PAUSE"))
	{
		isMouseMode_ = false;
		SetMouseDispFlag(false);
	}

	// モードに応じてカーソルコライダーの位置を更新
	if (cursorCollider_ != nullptr)
	{
		if (isMouseMode_)
		{
			cursorCollider_->SetCenterPos(mousePositionF);
		}
		else
		{
			cursorCollider_->SetCenterPos(CURSOR_OUTOF_SCREEN);
		}
	}

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

	const Vector2 mousePosition = inputManager.GetMousePosition();
	const Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	cursorCollider_->SetCenterPos(mousePositionF);

	// マウスによるホバー処理
	bool isHoverGame = false;
	bool isHoverTitle = false;

	if (isMouseMode_)
	{
		isHoverGame = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_GAME_BUTTON);
		isHoverTitle = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_TITLE_BUTTON);

		if (isHoverGame)
		{
			selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_GAME);
		}
		else if (isHoverTitle)
		{
			selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_TITLE);
		}
	}

	// インターバルのカウントダウン
	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_ = inputIntervalCounter_ - 1;
	}

	// キーボード・パッド・スティック操作
	if (inputIntervalCounter_ <= 0)
	{
		if (inputManager.IsPressed("UP"))
		{
			selectedIndex_ = (selectedIndex_ + static_cast<int>(MENU_ITEM::COUNT) - 1) %
				static_cast<int>(MENU_ITEM::COUNT);

			inputIntervalCounter_ = STICK_INTERVAL;
		}
		else if (inputManager.IsPressed("DOWN"))
		{
			selectedIndex_ = (selectedIndex_ + 1) % static_cast<int>(MENU_ITEM::COUNT);

			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	// 上下の入力が全くない場合はインターバルをリセットする
	if (inputManager.IsPressed("UP") == false && inputManager.IsPressed("DOWN") == false)
	{
		inputIntervalCounter_ = 0;
	}

	if (inputManager.IsTriggerDown("PAUSE") || inputManager.IsTriggerDown("CANCEL"))
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

	if (inputManager.IsTriggerDown("OK") || isMouseClicked)
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

	const Vector2 mousePosition = inputManager.GetMousePosition();
	const Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	cursorCollider_->SetCenterPos(mousePositionF);

	if (isPhaseChanged_)
	{
		isPhaseChanged_ = false;
		return;
	}

	bool isHoverYes = false;
	bool isHoverNo = false;

	if (isMouseMode_)
	{
		isHoverYes = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::PAUSE_YESNO_BUTTON);
		isHoverNo = collisionController.IsTagCollidingWithTag2D(TAG::PAUSE_MOUSE_CURSOR, TAG::SELECT_SINGLE_BIG);

		if (isHoverYes)
		{
			isYes_ = true;
		}
		else if (isHoverNo)
		{
			isYes_ = false;
		}
	}

	// インターバルのカウントダウン
	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_ = inputIntervalCounter_ - 1;
	}

	// キーボード・パッド・スティック操作
	if (inputIntervalCounter_ <= 0)
	{
		if (inputManager.IsPressed("LEFT") || inputManager.IsPressed("RIGHT"))
		{
			isYes_ = !isYes_;

			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	// 左右の入力が全くない場合はインターバルをリセットする
	if (inputManager.IsPressed("LEFT") == false && inputManager.IsPressed("RIGHT") == false)
	{
		inputIntervalCounter_ = 0;
	}

	if (inputManager.IsTriggerDown("CANCEL"))
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

	if (inputManager.IsTriggerDown("OK") || isMouseClicked)
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

	const int drawY = Application::SCREEN_HALF_Y;
	int drawX = Application::SCREEN_HALF_X - YES_NO_TEXT_OFFSET_X;

	DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y,
		BACKGROUND_SCALE, 0.0f, backGroundHandle_, true);

	// はい描画
	const int yesHandle = GetTextUIHandle(PAUSE_TEXT_UI::YES, isYes_);
	if (yesHandle != INVALID_HANDLE)
	{
		DrawRotaGraph(drawX, drawY, TEXT_SCALE, 0.0f, yesHandle, true);
	}

	// いいえ描画
	drawX = drawX + NO_BUTTON_X_SPACING;

	const int noHandle = GetTextUIHandle(PAUSE_TEXT_UI::NO, !isYes_);
	if (noHandle != INVALID_HANDLE)
	{
		DrawRotaGraph(drawX, drawY, TEXT_SCALE, 0.0f, noHandle, true);
	}
}

void ScenePause::DrawFrame(float _rate)
{
	int width = 0;
	int height = 0;

	GetGraphSize(backGroundHandle_, &width, &height);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_HALF);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BACKGROUND_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		width / 2,
		height / 2,
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
		const auto currentMenuItem = static_cast<MENU_ITEM>(i);
		const bool isSelected = (selectedIndex_ == static_cast<int>(i));

		PAUSE_TEXT_UI textType = PAUSE_TEXT_UI::RETURN_GAME;
		if (currentMenuItem == MENU_ITEM::RETURN_TITLE)
		{
			textType = PAUSE_TEXT_UI::RETURN_TITLE;
		}

		const int uiHandle = GetTextUIHandle(textType, isSelected);

		if (uiHandle != INVALID_HANDLE)
		{
			DrawRotaGraph(Application::SCREEN_HALF_X,drawY,
				TEXT_SCALE, 0.0f, uiHandle, true);
		}

		drawY = drawY + MENU_ITEM_HEIGHT;
	}
}

int ScenePause::GetTextUIHandle(PAUSE_TEXT_UI _textType, bool _isSelected) const
{
	const size_t index = static_cast<size_t>(_textType);

	if (_isSelected)
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

	return INVALID_HANDLE;
}