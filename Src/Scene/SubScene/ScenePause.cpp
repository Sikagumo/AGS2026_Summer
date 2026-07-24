#include "ScenePause.h"

#include <DxLib.h>

#include "../../Manager/Generic/SceneManager.h" 
#include "../MainScene/SceneTitle.h"   
#include "../../Manager/Generic/KeyConfInputManager.h" 
#include "../../Application.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"

ScenePause::ScenePause(void)
	: currentPhase_(PHASE::APPEAR)
	, frame_(0)
	, selectedIndex_(0)
	, isYes_(false)
	, yesNoTitle_("")
{
}

ScenePause::~ScenePause(void)
{
}

void ScenePause::Load(void)
{
	SceneBase::Load();
}

void ScenePause::EndLoad(void)
{
	SceneBase::EndLoad();
}

void ScenePause::Initialize(void)
{
	SceneBase::Initialize();

	yesNoItems_ = { "はい", "いいえ" };
	menuItems_ = { "ゲームに戻る", "タイトルに戻る" };

	auto& collisionController = CollisionController::GetInstance();

	// マウスカーソル用の円コライダー
	cursorCollider_ = std::make_unique<Collider2DCircle>(
		Vector2F(0.0f, 0.0f), 10.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);

	Vector2F gameButtonCenter(MENU_LEFT_OFFSET + BUTTON_WIDTH / 2.0f,
		MENU_TOP_OFFSET + BUTTON_HEIGHT / 2.0f); // ゲームに戻るボタンの中心座標
	returnGameButtonCollider_ = std::make_unique<Collider2DBox>(
		gameButtonCenter, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON);

	Vector2F titleButtonCenter(MENU_LEFT_OFFSET + BUTTON_WIDTH / 2.0f,
		MENU_TOP_OFFSET + MENU_ITEM_HEIGHT + BUTTON_HEIGHT / 2.0f); // タイトルに戻るボタンの中心座標
	returnTitleButtonCollider_ = std::make_unique<Collider2DBox>(
		titleButtonCenter, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::EXIT_UTTON);

	// YES/NOダイアログ用のボタンの中心座標を計算
	int centerX = Application::SCREEN_HALF_X;
	int centerY = Application::SCREEN_HALF_Y;
	int top = centerY - 150 / 2;
	int left = centerX - 300 / 2;
	int drawX = left + 50;
	int drawY = top + 60; // top + 20(タイトル) + 40(余白) = 60

	Vector2F yesButtonCenter(static_cast<float>(drawX + 30), static_cast<float>(drawY + 15)); // 「はい」の中心座標
	yesButtonCollider_ = std::make_unique<Collider2DBox>(
		yesButtonCenter, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT, Collider2DBase::TAG_2D::SELECT_SINGLE_BOMB);

	Vector2F noButtonCenter(static_cast<float>(drawX + 100 + 40), static_cast<float>(drawY + 15)); // 「いいえ」の中心座標
	noButtonCollider_ = std::make_unique<Collider2DBox>(
		noButtonCenter, YES_NO_BUTTON_WIDTH, YES_NO_BUTTON_HEIGHT, Collider2DBase::TAG_2D::SELECT_SINGLE_BIG);

	// コライダーの登録
	collisionController.RegisterCollider2D(cursorCollider_.get());
	collisionController.RegisterCollider2D(returnGameButtonCollider_.get());
	collisionController.RegisterCollider2D(returnTitleButtonCollider_.get());
	collisionController.RegisterCollider2D(yesButtonCollider_.get());
	collisionController.RegisterCollider2D(noButtonCollider_.get());

	// YES/NO用のタグとマウスカーソルの当たり判定を有効化する
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::SELECT_SINGLE_BOMB, true);
	collisionController.SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::SELECT_SINGLE_BIG, true);
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

	Vector2 mousePosition = inputManager.GetMousePosition();
	Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)); // float型に変換したマウス座標
	cursorCollider_->SetCenterPos(mousePositionF);

	using TAG = Collider2DBase::TAG_2D;

	if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SOLO_PLAY_BUTTON))
	{
		selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_GAME);
	}
	else if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::EXIT_UTTON))
	{
		selectedIndex_ = static_cast<int>(MENU_ITEM::RETURN_TITLE);
	}

	// キーボード・パッド操作
	if (inputManager.isTrigerDown("UP"))
	{
		selectedIndex_ = (selectedIndex_ + static_cast<int>(MENU_ITEM::COUNT) - 1) % static_cast<int>(MENU_ITEM::COUNT);
	}

	if (inputManager.isTrigerDown("DOWN"))
	{
		selectedIndex_ = (selectedIndex_ + 1) % static_cast<int>(MENU_ITEM::COUNT);
	}

	// ポーズボタンが押されたら閉じる
	if (inputManager.isTrigerDown("PAUSE"))
	{
		currentPhase_ = PHASE::DISAPPEAR;
		return;
	}

	bool isMouseClicked = false;
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SOLO_PLAY_BUTTON) ||
			collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::EXIT_UTTON))
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
		yesNoTitle_ = "タイトルに戻る";
		currentPhase_ = PHASE::YES_NO;
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

	// マウス座標の更新
	Vector2 mousePosition = inputManager.GetMousePosition();
	Vector2F mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	cursorCollider_->SetCenterPos(mousePositionF);

	// マウスホバーによる状態の変更
	if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SELECT_SINGLE_BOMB))
	{
		isYes_ = true;
	}
	else if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SELECT_SINGLE_BIG))
	{
		isYes_ = false;
	}

	// キーボードによる状態の変更
	if (inputManager.isTrigerDown("LEFT") || inputManager.isTrigerDown("RIGHT"))
	{
		isYes_ = !isYes_;
	}

	// 左クリックの入力判定
	bool isMouseClicked = false;
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		if (collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SELECT_SINGLE_BOMB) ||
			collisionController.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::SELECT_SINGLE_BIG))
		{
			isMouseClicked = true;
		}
	}

	// 決定処理
	if (inputManager.isTrigerDown("OK") || isMouseClicked)
	{
		if (isYes_)
		{
			// 「はい」の処理を専用関数へ
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

	auto centerX = Application::SCREEN_HALF_X; // 画面の中心X座標
	auto centerY = Application::SCREEN_HALF_Y; // 画面の中心Y座標

	constexpr int YES_NO_FRAME_HEIGHT = 150; // YES/NO枠の縦幅
	constexpr int YES_NO_FRAME_WIDTH = 300;  // YES/NO枠の横幅

	int top = centerY - YES_NO_FRAME_HEIGHT / 2; // 上端の座標
	int bottom = centerY + YES_NO_FRAME_HEIGHT / 2; // 下端の座標
	int left = centerX - YES_NO_FRAME_WIDTH / 2; // 左端の座標
	int right = centerX + YES_NO_FRAME_WIDTH / 2; // 右端の座標

	DrawBox(left, top, right, bottom, 0x000000, true);
	DrawBoxAA(static_cast<float>(left), static_cast<float>(top), static_cast<float>(right), static_cast<float>(bottom), 0xffffff, false, 3.0f);

	int drawX = left + 50; // 描画X座標
	int drawY = top + 20;  // 描画Y座標

	DrawFormatString(drawX, drawY, 0xffffff, "%s", yesNoTitle_.c_str());
	drawY = drawY + 40;

	int index = 0; // 選択インデックス

	if (!isYes_)
	{
		index = 1;
	}

	for (size_t i = 0; i < yesNoItems_.size(); ++i)
	{
		uint32_t itemColor = 0xffffff; // 項目の色
		if (i == static_cast<size_t>(index))
		{
			DrawString(drawX - 30, drawY, "⇒", 0xff0000);
			itemColor = 0xff00ff;
		}

		DrawFormatString(drawX, drawY, itemColor, "%s", yesNoItems_[i].c_str());
		drawX = drawX + 100;
	}
}

void ScenePause::DrawFrame(float rate)
{
	auto centerY = Application::SCREEN_HALF_Y; // 画面の中心Y座標
	auto height = (Application::SCREEN_SIZE_Y - MARGIN_SIZE) / 2; // 枠の高さ

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBoxAA(
		static_cast<float>(MARGIN_SIZE),
		static_cast<float>(centerY - height * rate),
		static_cast<float>(Application::SCREEN_SIZE_X - MARGIN_SIZE),
		static_cast<float>(centerY + height * rate),
		0x000000, true, 1.0f);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawBoxAA(
		static_cast<float>(MARGIN_SIZE),
		static_cast<float>(centerY - height * rate),
		static_cast<float>(Application::SCREEN_SIZE_X - MARGIN_SIZE),
		static_cast<float>(centerY + height * rate),
		0xffffff, false, 3.0f);
}

void ScenePause::DrawMenu(void)
{
	constexpr uint32_t MENU_ITEM_COLOR = 0xffffffff; // 選択されていない項目の色
	constexpr uint32_t INDICATOR_COLOR = 0xffffaaaa; // 選択矢印の色
	constexpr int MENU_INDENT_SIZE = 10;             // 選択時のインデント量
	constexpr uint32_t SELECTED_COLOR = 0xffffaaff;  // 選択されている項目の色

	int drawY = MENU_TOP_OFFSET; // 描画Y座標

	for (size_t i = 0; i < menuItems_.size(); ++i)
	{
		int drawX = MENU_LEFT_OFFSET; // 描画X座標
		auto itemColor = MENU_ITEM_COLOR; // 描画カラー

		if (selectedIndex_ == static_cast<int>(i))
		{
			DrawString(drawX - 30, drawY, "⇒", INDICATOR_COLOR);
			drawX = drawX + MENU_INDENT_SIZE;
			itemColor = SELECTED_COLOR;
		}
		DrawFormatString(drawX, drawY, itemColor, "%s", menuItems_[i].c_str());

		drawY = drawY + MENU_ITEM_HEIGHT;
	}
}