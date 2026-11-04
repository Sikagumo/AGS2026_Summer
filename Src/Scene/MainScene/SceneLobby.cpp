#include "SceneLobby.h"

#include <algorithm>

#include "../../Camera/Camera.h"
#include "../../Common/Loading.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/NetManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../SubScene/ScenePause.h"
#include "./SceneGame.h"

namespace
{
	// ファイル内複数箇所で共有する基本定数
	constexpr int INVALID_HANDLE = -1;            // 無効なハンドル・インデックス値
	constexpr int STICK_INTERVAL = 15;            // スティック入力の連続受付間隔
	constexpr int ANIM_IDLE_INDEX = 0;            // 待機アニメーションの登録番号
	constexpr float ANIM_IDLE_SPEED = 30.0f;      // 待機アニメーションの再生速度

	// メイン選択インデックス定数
	constexpr int MAIN_SELECT_WEAPON = 0;         // メイン選択：武器
	constexpr int MAIN_SELECT_SKIN = 1;           // メイン選択：スキン
	constexpr int MAIN_SELECT_READY = 2;          // メイン選択：準備完了・開始

	// 退出確認インデックス定数
	constexpr int LEAVE_SELECT_YES = 0;           // 退出確認：はい
	constexpr int LEAVE_SELECT_NO = 1;            // 退出確認：いいえ
	constexpr int LEAVE_SELECT_COUNT = 2;         // 退出確認の選択肢数

	// 共通UIレイアウト・描画スケール定数
	constexpr int START_BUTTON_OFFSET_X = 350;			 // 開始・編成ボタンのX座標オフセット
	constexpr int START_BUTTON_OFFSET_Y = 100;			 // 開始・編成ボタンのY座標オフセット
	constexpr float START_BTN_BACK_SCALE_X = 0.4f;		 // 開始ボタン背景のX拡大率
	constexpr float START_BTN_BACK_SCALE_Y = 0.3f;		 // 開始ボタン背景のY拡大率
	constexpr float COMMON_TEXT_SCALE = 0.6f;			 // UIテキストの標準描画スケール
	constexpr int FILTER_BLEND_ALPHA = 180;			     // 背景暗転用フィルターのアルファ値
	constexpr float WINDOW_ITEM_START_OFFSET_Y = 200.0f; // ミニウィンドウ内項目の開始Yオフセット
	constexpr float WINDOW_ITEM_INTERVAL_Y = 150.0f;     // ミニウィンドウ内項目のY間隔
	constexpr float WINDOW_BACK_SCALE_X = 1.0f;			 // ミニウィンドウ背景のX拡大率
	constexpr float WINDOW_BACK_SCALE_Y = 1.2f;			 // ミニウィンドウ背景のY拡大率
	constexpr int IP_BOX_WIDTH = 400;					 // パスコードボックスの基準幅
	constexpr int PASSCODE_BOX_POS_Y = 280;				 // パスコードボックスのY座標
	constexpr int LEAVE_BTN_OFFSET_X = 150;				 // 退出確認ボタンのXオフセット
	constexpr int LEAVE_BTN_OFFSET_Y = 80;				 // 退出確認ボタンのYオフセット
}

SceneLobby::SceneLobby(bool _isMulti)
	: SceneBase()
	, isMulti_(_isMulti)
	, mainSelectIndex_(MAIN_SELECT_WEAPON)
	, selectedJobIndex_(0)
	, selectedSkinIndex_(0)
	, inputIntervalCounter_(0)
	, prevMousePos_(0.0f, 0.0f)
	, isMouseMode_(true)
	, selectState_(SELECT_STATE::MAIN)
	, uiHandles_()
	, weaponInfoHandle_()
	, backgroundHandle_(INVALID_HANDLE)
	, roomBackHandle_(INVALID_HANDLE)
	, selectUIBackHandle_(INVALID_HANDLE)
	, selectedUIBackHandle_(INVALID_HANDLE)
	, uiBackWidth_(0)
	, uiBackHeight_(0)
	, uiTexHandles_()
	, uiReadyHandles_()
	, selectConfTextHandles_()
	, noSelectConfTextHandles_()
	, passcodeTextHandles_()
	, selectMultiHandle_(INVALID_HANDLE)
	, selectedMultiHandle_(INVALID_HANDLE)
	, multiTitleHandle_(INVALID_HANDLE)
	, connectTexHandle_(INVALID_HANDLE)
	, allReadyImageHandle_(INVALID_HANDLE)
	, leaveRoomTextHandle_(INVALID_HANDLE)
	, cursorCollider_(nullptr)
	, mainUiCollisions_()
	, readyButtonCollision_(nullptr)
	, weaponUiCollisions_()
	, skinUiCollisions_()
	, multiUiCollisions_()
	, leaveYesCollider_(nullptr)
	, leaveNoCollider_(nullptr)
	, previewModelHandle_(INVALID_HANDLE)
	, currentModelIndex_(INVALID_HANDLE)
	, previewModelRotY_(0.0f)
	, animController_(nullptr)
	, lobbySkinHandles_()
	, lobbyAnimControllers_()
	, multiState_(LOBBY_STATE::SELECT_MODE)
	, buttonSelectIndex_(0)
	, passcode_{ 0, 0, 0, 0 }
	, selectOctet_(0)
	, isEditing_(false)
	, myReadyState_(false)
	, isLeaveWindow_(false)
	, leaveSelectIndex_(LEAVE_SELECT_NO)
{
	uiHandles_.fill(INVALID_HANDLE);
	weaponInfoHandle_.fill(INVALID_HANDLE);
	uiTexHandles_.fill(INVALID_HANDLE);
	uiReadyHandles_.fill(INVALID_HANDLE);
	selectConfTextHandles_.fill(INVALID_HANDLE);
	noSelectConfTextHandles_.fill(INVALID_HANDLE);
	passcodeTextHandles_.fill(INVALID_HANDLE);
	lobbySkinHandles_.fill(INVALID_HANDLE);
}

SceneLobby::~SceneLobby(void)
{
}

void SceneLobby::Load(void)
{
	auto& resourceManager = ResourceManager::GetInstance();

	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_SELECT, uiHandles_.data());

	// 背景・UI枠画像の読み込み
	backgroundHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_BACKGROUND_SELCET);
	roomBackHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_BACKGROUND_MALUTI);
	selectUIBackHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_SELECT_UI_BACK);
	selectedUIBackHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_SELECTED_UI_BACK);
	selectMultiHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_SELECT_MULTI_BACK);
	selectedMultiHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_SELECTED_MULTI_BACK);
	leaveRoomTextHandle_ = resourceManager.LoadHandleId(ResourceManager::SRC::IMG_TEXT_LEAVE_ROOM);

	// 確認用YES/NOテキストの読み込み
	constexpr size_t PAUSE_TEX_COUNT = 4;
	constexpr size_t PAUSE_YES_INDEX = 2;
	constexpr size_t PAUSE_NO_INDEX = 3;

	std::array<int, PAUSE_TEX_COUNT> tempSelectHandles{};
	std::array<int, PAUSE_TEX_COUNT> tempNormalHandles{};

	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_SELECT_PUSE_TEX, tempSelectHandles.data());
	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_POUSE_TEX, tempNormalHandles.data());

	selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES)) = tempSelectHandles.at(PAUSE_YES_INDEX);
	selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO)) = tempSelectHandles.at(PAUSE_NO_INDEX);
	noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES)) = tempNormalHandles.at(PAUSE_YES_INDEX);
	noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO)) = tempNormalHandles.at(PAUSE_NO_INDEX);

	// 武器説明・マルチ関連画像の読み込み
	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_PLAYER_WEAPON_INFO, weaponInfoHandle_.data());

	constexpr int CONECT_TEX_CONNECTING_IDX = 0;
	constexpr int CONECT_TEX_TITLE_IDX = 1;
	constexpr int MULTI_TEX_ALL_READY_IDX = 4;

	multiTitleHandle_ = resourceManager.LoadHandleIdsOnce(ResourceManager::SRC::IMGS_CONECT_TEX, CONECT_TEX_TITLE_IDX);
	connectTexHandle_ = resourceManager.LoadHandleIdsOnce(ResourceManager::SRC::IMGS_CONECT_TEX, CONECT_TEX_CONNECTING_IDX);
	allReadyImageHandle_ = resourceManager.LoadHandleIdsOnce(ResourceManager::SRC::IMGS_MULTI_TEX, MULTI_TEX_ALL_READY_IDX);

	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_LOBBY_UI_TEX, uiTexHandles_.data());
	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_TIMER, passcodeTextHandles_.data());
	resourceManager.LoadHandleIds(ResourceManager::SRC::IMGS_MULTI_TEX, uiReadyHandles_.data());

	// サウンドの読み込み
	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_LOBBY_SELECT,
		resourceManager.LoadHandleId(ResourceManager::SRC::SE_LOBBY_SELCET));
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_LOBBY,
		resourceManager.LoadHandleId(ResourceManager::SRC::BGM_LOBBY));

	// ロビー表示用スキンモデルの読み込み
	lobbySkinHandles_.at(static_cast<size_t>(PlayerBase::SKIN_TYPE::HUMAN)) = resourceManager.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_HUMAN);
	lobbySkinHandles_.at(static_cast<size_t>(PlayerBase::SKIN_TYPE::DOG)) = resourceManager.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_DOG);
	lobbySkinHandles_.at(static_cast<size_t>(PlayerBase::SKIN_TYPE::MONKEY)) = resourceManager.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_MONKEY);
	lobbySkinHandles_.at(static_cast<size_t>(PlayerBase::SKIN_TYPE::BIRD)) = resourceManager.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER_BIRD);

	const int idleAnimHandle = resourceManager.LoadHandleId(ResourceManager::SRC::ANIM_IDLE);

	for (size_t i = 0; i < lobbySkinHandles_.size(); ++i)
	{
		if (lobbySkinHandles_.at(i) != INVALID_HANDLE)
		{
			lobbyAnimControllers_.at(i) = std::make_unique<AnimationController>(lobbySkinHandles_.at(i));
			lobbyAnimControllers_.at(i)->AddExternal(ANIM_IDLE_INDEX, idleAnimHandle, ANIM_IDLE_SPEED);
			lobbyAnimControllers_.at(i)->Play(ANIM_IDLE_INDEX, true);
		}
	}
}

void SceneLobby::EndLoad(void)
{
}

void SceneLobby::Initialize(void)
{
	NetManager::GetInstance().Stop();

	isLeaveWindow_ = false;
	leaveSelectIndex_ = LEAVE_SELECT_NO;
	myReadyState_ = false;

	selectedJobIndex_ = 0;
	selectedSkinIndex_ = 0;
	previewModelRotY_ = 0.0f;

	GetGraphSize(selectUIBackHandle_, &uiBackWidth_, &uiBackHeight_);

	SetMouseDispFlag(true);

	// カーソル用コライダー登録
	constexpr float CURSOR_RADIUS = 1.0f;
	cursorCollider_ = std::make_unique<Collider2DCircle>(Vector2F(0.0f, 0.0f), CURSOR_RADIUS, Collider2DBase::TAG_2D::MOUSE_CURSOR);
	CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

	// シングル・マルチ共通で装備選択UIを初期化
	InitUISingle();

	if (isMulti_)
	{
		multiState_ = LOBBY_STATE::SELECT_EQUIPMENT;
		passcode_.fill(0);
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
	// メインUI位置・サイズ設定
	constexpr float MAIN_PANEL_WIDTH = 300.0f;
	constexpr float MAIN_PANEL_HEIGHT = 450.0f;
	constexpr float MAIN_PANEL_OFFSET_X = 500.0f;
	constexpr float READY_BUTTON_WIDTH = 500.0f;
	constexpr float READY_BUTTON_HEIGHT = 150.0f;

	const Vector2F leftPanelPos(
		static_cast<float>(Application::SCREEN_HALF_X) - MAIN_PANEL_OFFSET_X,
		static_cast<float>(Application::SCREEN_HALF_Y)
	);

	const Vector2F rightPanelPos(
		static_cast<float>(Application::SCREEN_HALF_X) + MAIN_PANEL_OFFSET_X,
		static_cast<float>(Application::SCREEN_HALF_Y)
	);

	const Vector2F readyButtonPos(
		static_cast<float>(Application::SCREEN_HALF_X + START_BUTTON_OFFSET_X),
		static_cast<float>(Application::SCREEN_SIZE_Y - START_BUTTON_OFFSET_Y)
	);

	mainUiCollisions_.at(MAIN_SELECT_WEAPON) = std::make_unique<Collider2DBox>(
		leftPanelPos, MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT, Collider2DBase::TAG_2D::UI_MAIN_WEAPON
	);

	mainUiCollisions_.at(MAIN_SELECT_SKIN) = std::make_unique<Collider2DBox>(
		rightPanelPos, MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT, Collider2DBase::TAG_2D::UI_MAIN_SKIN
	);

	readyButtonCollision_ = std::make_unique<Collider2DBox>(
		readyButtonPos, READY_BUTTON_WIDTH, READY_BUTTON_HEIGHT, Collider2DBase::TAG_2D::UI_MAIN_READY
	);

	// ミニウィンドウ内コライダー設定
	constexpr float WINDOW_WIDTH = 500.0f;
	constexpr float ITEM_HEIGHT = 80.0f;
	constexpr float WINDOW_TOP_OFFSET_Y = 210.0f;

	const float windowTopY = static_cast<float>(Application::SCREEN_HALF_Y) - WINDOW_TOP_OFFSET_Y;

	// 武器ミニウィンドウ内のコライダー生成
	const int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);
	for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
	{
		const float currentItemPosY = windowTopY + (static_cast<float>(jobIndex) * WINDOW_ITEM_INTERVAL_Y);
		const Vector2F itemCenterPos(static_cast<float>(Application::SCREEN_HALF_X), currentItemPosY);

		weaponUiCollisions_.at(static_cast<size_t>(jobIndex)) = std::make_unique<Collider2DBox>(
			itemCenterPos, WINDOW_WIDTH, ITEM_HEIGHT, Collider2DBase::TAG_2D::UI_WINDOW_WEAPON
		);
	}

	// スキンミニウィンドウ内のコライダー生成
	const int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);
	for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
	{
		const float currentItemPosY = windowTopY + (static_cast<float>(skinIndex) * WINDOW_ITEM_INTERVAL_Y);
		const Vector2F itemCenterPos(static_cast<float>(Application::SCREEN_HALF_X), currentItemPosY);

		skinUiCollisions_.at(static_cast<size_t>(skinIndex)) = std::make_unique<Collider2DBox>(
			itemCenterPos, WINDOW_WIDTH, ITEM_HEIGHT, Collider2DBase::TAG_2D::UI_WINDOW_SKIN
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
	constexpr float PASSCODE_BOX_HEIGHT = 60.0f;
	constexpr float MODE_BTN_COLLIDER_SCALE = 0.2f;
	constexpr float LEAVE_BTN_COLLIDER_SCALE = 0.4f;

	const int ipBoxX = (Application::SCREEN_SIZE_X - IP_BOX_WIDTH) / 2;

	// パスコード入力ボックスのコライダー
	const Vector2F passcodePos(static_cast<float>(ipBoxX + (IP_BOX_WIDTH / 2)), static_cast<float>(PASSCODE_BOX_POS_Y));
	multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::PASSCODE_BOX)) = std::make_unique<Collider2DBox>(
		passcodePos, static_cast<float>(IP_BOX_WIDTH), PASSCODE_BOX_HEIGHT, Collider2DBase::TAG_2D::NONE
	);

	// モード選択ボタンの当たり判定サイズ
	const float colliderWidth = static_cast<float>(uiBackWidth_) * MODE_BTN_COLLIDER_SCALE;
	const float colliderHeight = static_cast<float>(uiBackHeight_) * MODE_BTN_COLLIDER_SCALE;

	// HOSTボタンのコライダー
	const Vector2F hostPos(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(HOST_BUTTON_Y));
	multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::HOST_BUTTON)) = std::make_unique<Collider2DBox>(
		hostPos, colliderWidth, colliderHeight, Collider2DBase::TAG_2D::NONE
	);

	// CLIENTボタンのコライダー
	const Vector2F clientPos(static_cast<float>(Application::SCREEN_HALF_X), static_cast<float>(CLIENT_BUTTON_Y));
	multiUiCollisions_.at(static_cast<size_t>(MULTI_UI_TYPE::CLIENT_BUTTON)) = std::make_unique<Collider2DBox>(
		clientPos, colliderWidth, colliderHeight, Collider2DBase::TAG_2D::NONE
	);

	auto& collisionController = CollisionController::GetInstance();
	for (auto& collider : multiUiCollisions_)
	{
		if (collider != nullptr)
		{
			collisionController.RegisterCollider2D(collider.get());
		}
	}

	// 退出確認ボタンのコライダー
	const float buttonColliderWidth = static_cast<float>(uiBackWidth_) * LEAVE_BTN_COLLIDER_SCALE;
	const float buttonColliderHeight = static_cast<float>(uiBackHeight_) * LEAVE_BTN_COLLIDER_SCALE;

	const Vector2F yesPos(
		static_cast<float>(Application::SCREEN_HALF_X - LEAVE_BTN_OFFSET_X),
		static_cast<float>(Application::SCREEN_HALF_Y + LEAVE_BTN_OFFSET_Y)
	);
	leaveYesCollider_ = std::make_unique<Collider2DBox>(
		yesPos, buttonColliderWidth, buttonColliderHeight, Collider2DBase::TAG_2D::NONE
	);

	const Vector2F noPos(
		static_cast<float>(Application::SCREEN_HALF_X + LEAVE_BTN_OFFSET_X),
		static_cast<float>(Application::SCREEN_HALF_Y + LEAVE_BTN_OFFSET_Y)
	);
	leaveNoCollider_ = std::make_unique<Collider2DBox>(
		noPos, buttonColliderWidth, buttonColliderHeight, Collider2DBase::TAG_2D::NONE
	);

	collisionController.RegisterCollider2D(leaveYesCollider_.get());
	collisionController.RegisterCollider2D(leaveNoCollider_.get());
}

void SceneLobby::Update(void)
{
	if (Loading::GetInstance()->IsLoading())
	{
		return;
	}

	auto& inputMgr = KeyConfInputManager::GetInstance();

	// マウス位置を取得しモード判定を行う
	const Vector2 mousePos = inputMgr.GetMousePosition();
	const Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

	if (mousePosF.x != prevMousePos_.x || mousePosF.y != prevMousePos_.y ||
		(GetMouseInput() & (MOUSE_INPUT_LEFT | MOUSE_INPUT_RIGHT)) != 0)
	{
		isMouseMode_ = true;
		SetMouseDispFlag(true);
	}

	if (inputMgr.IsPressed("UP") || inputMgr.IsPressed("DOWN") ||
		inputMgr.IsPressed("LEFT") || inputMgr.IsPressed("RIGHT") ||
		inputMgr.IsTriggerDown("OK") || inputMgr.IsTriggerDown("CANCEL") ||
		inputMgr.IsTriggerDown("PAUSE"))
	{
		isMouseMode_ = false;
		SetMouseDispFlag(false);
	}

	prevMousePos_ = mousePosF;

	if (cursorCollider_ != nullptr && isMouseMode_)
	{
		cursorCollider_->SetCenterPos(mousePosF);
	}

	for (auto& animController : lobbyAnimControllers_)
	{
		if (animController != nullptr)
		{
			animController->Update();
		}
	}

	if (isMulti_)
	{
		UpdateMulti();
	}
	else
	{
		UpdateSingle();
	}

	// モデル選択の変更チェック
	UpdatePreviewModel();

	// プレビュー用3Dモデルの位置・姿勢を更新
	if (previewModelHandle_ != INVALID_HANDLE)
	{
		constexpr VECTOR PREVIEW_MODEL_POS = { 0.0f, -250.0f, 0.0f };
		constexpr VECTOR PREVIEW_MODEL_SCALE = { 5.0f, 5.0f, 5.0f };
		constexpr float PREVIEW_ROT_SPEED_Y = 0.01f;

		MV1SetPosition(previewModelHandle_, PREVIEW_MODEL_POS);
		MV1SetScale(previewModelHandle_, PREVIEW_MODEL_SCALE);

		previewModelRotY_ += PREVIEW_ROT_SPEED_Y;
		MV1SetRotationXYZ(previewModelHandle_, VGet(0.0f, previewModelRotY_, 0.0f));

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

	if (isMulti_)
	{
		DrawMulti();
	}
	else
	{
		DrawCenterModel();

		const int startButtonHandle = (mainSelectIndex_ == MAIN_SELECT_READY)
			? selectedUIBackHandle_
			: selectUIBackHandle_;

		const Vector2 startButtonPos =
		{
			Application::SCREEN_HALF_X + START_BUTTON_OFFSET_X,
			Application::SCREEN_SIZE_Y - START_BUTTON_OFFSET_Y
		};

		constexpr double WEAPON_IMAGE_SCALE = 0.5;
		constexpr int WEAPON_IMAGE_OFFSET_Y = 250;
		const Vector2 weaponImagePos =
		{
			Application::SCREEN_HALF_X,
			Application::SCREEN_SIZE_Y - WEAPON_IMAGE_OFFSET_Y
		};

		// 選択中の武器画像を描画
		const int weaponHandle = GetWeaponUIHandle(static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_), true);
		if (weaponHandle != INVALID_HANDLE)
		{
			DrawRotaGraph(weaponImagePos.x, weaponImagePos.y, WEAPON_IMAGE_SCALE, 0.0, weaponHandle, true);
		}

		// ボタン背景描画
		DrawRotaGraph3(
			startButtonPos.x,
			startButtonPos.y,
			uiBackWidth_ / 2,
			uiBackHeight_ / 2,
			START_BTN_BACK_SCALE_X,
			START_BTN_BACK_SCALE_Y,
			0.0f,
			startButtonHandle,
			true
		);

		// ボタンテキスト描画
		DrawRotaGraph(
			startButtonPos.x,
			startButtonPos.y,
			COMMON_TEXT_SCALE,
			0.0,
			uiHandles_.at(static_cast<size_t>(UI_SINGLE::GAME_START)),
			true
		);

		DrawSelectionPanels();
	}
}

void SceneLobby::Release(void)
{
}

void SceneLobby::DrawCenterModel(void)
{
	if (previewModelHandle_ != INVALID_HANDLE)
	{
		MV1DrawModel(previewModelHandle_);
	}
}

void SceneLobby::DrawSelectionPanels(void)
{
	constexpr float PANEL_BACK_SCALE_Y = 0.8f;

	const int leftPanelHandle = (mainSelectIndex_ == MAIN_SELECT_WEAPON) ? selectedUIBackHandle_ : selectUIBackHandle_;
	const int rightPanelHandle = (mainSelectIndex_ == MAIN_SELECT_SKIN) ? selectedUIBackHandle_ : selectUIBackHandle_;

	// 左右パネル背景を描画
	DrawRotaGraph3(
		LEFT_PANEL_X,
		PANEL_START_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		BUTTON_SCALE,
		PANEL_BACK_SCALE_Y,
		0.0f,
		leftPanelHandle,
		true
	);

	DrawRotaGraph3(
		RIGHT_PANEL_X,
		PANEL_START_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		BUTTON_SCALE,
		PANEL_BACK_SCALE_Y,
		0.0f,
		rightPanelHandle,
		true
	);

	// 左右パネル文字を描画
	DrawRotaGraph(
		LEFT_PANEL_X,
		PANEL_START_Y,
		COMMON_TEXT_SCALE,
		0.0,
		uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::WEAPON)),
		true
	);

	DrawRotaGraph(
		RIGHT_PANEL_X,
		PANEL_START_Y,
		COMMON_TEXT_SCALE,
		0.0,
		uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::SKIN)),
		true
	);

	// ミニウィンドウが開いている場合は暗めの半透明フィルターを表示
	if (selectState_ != SELECT_STATE::MAIN)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, FILTER_BLEND_ALPHA);
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
		DrawWeaponWindow();
	}
	else if (selectState_ == SELECT_STATE::SKIN_WINDOW)
	{
		DrawSkinWindow();
	}
}

void SceneLobby::UpdateSingle(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	if (keyConfInputManager.IsTriggerDown("PAUSE"))
	{
		SceneManager::GetInstance().PushScene(std::make_shared<ScenePause>());
	}

	// スティック用のインターバルタイマー更新
	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_--;
	}

	if (!keyConfInputManager.IsPressed("UP") && !keyConfInputManager.IsPressed("DOWN") &&
		!keyConfInputManager.IsPressed("LEFT") && !keyConfInputManager.IsPressed("RIGHT"))
	{
		inputIntervalCounter_ = 0;
	}

	switch (selectState_)
	{
	case SELECT_STATE::MAIN:
		UpdateSelectMain();
		break;

	case SELECT_STATE::WEAPON_WINDOW:
		UpdateSelectWeaponWindow();
		break;

	case SELECT_STATE::SKIN_WINDOW:
		UpdateSelectSkinWindow();
		break;

	default:
		break;
	}
}

void SceneLobby::UpdateSelectMain(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();
	auto& collisionController = CollisionController::GetInstance();

	if (isMouseMode_ && cursorCollider_ != nullptr)
	{
		bool isHoverWeapon = false;
		bool isHoverSkin = false;
		bool isHoverReady = false;

		if (mainUiCollisions_.at(MAIN_SELECT_WEAPON) != nullptr)
		{
			isHoverWeapon = collisionController.CheckCollision2D(
				cursorCollider_.get(),
				mainUiCollisions_.at(MAIN_SELECT_WEAPON).get()
			);
		}

		if (mainUiCollisions_.at(MAIN_SELECT_SKIN) != nullptr)
		{
			isHoverSkin = collisionController.CheckCollision2D(
				cursorCollider_.get(),
				mainUiCollisions_.at(MAIN_SELECT_SKIN).get()
			);
		}

		if (readyButtonCollision_ != nullptr)
		{
			isHoverReady = collisionController.CheckCollision2D(
				cursorCollider_.get(),
				readyButtonCollision_.get()
			);
		}

		if (isHoverWeapon)
		{
			mainSelectIndex_ = MAIN_SELECT_WEAPON;
		}
		else if (isHoverSkin)
		{
			mainSelectIndex_ = MAIN_SELECT_SKIN;
		}
		else if (isHoverReady)
		{
			mainSelectIndex_ = MAIN_SELECT_READY;
		}
	}

	// スティック/キーボード入力でのUI選択切り替え
	if (inputIntervalCounter_ == 0)
	{
		if (keyConfInputManager.IsPressed("RIGHT") || keyConfInputManager.IsPressed("LEFT"))
		{
			if (mainSelectIndex_ == MAIN_SELECT_WEAPON)
			{
				mainSelectIndex_ = MAIN_SELECT_SKIN;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
			else if (mainSelectIndex_ == MAIN_SELECT_SKIN)
			{
				mainSelectIndex_ = MAIN_SELECT_WEAPON;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
		}
		else if (keyConfInputManager.IsPressed("DOWN"))
		{
			if (mainSelectIndex_ == MAIN_SELECT_WEAPON || mainSelectIndex_ == MAIN_SELECT_SKIN)
			{
				mainSelectIndex_ = MAIN_SELECT_READY;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
		}
		else if (keyConfInputManager.IsPressed("UP"))
		{
			if (mainSelectIndex_ == MAIN_SELECT_READY)
			{
				mainSelectIndex_ = MAIN_SELECT_WEAPON;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
		}
	}

	// 決定入力時の処理
	if (keyConfInputManager.IsTriggerDown("OK"))
	{
		if (mainSelectIndex_ == MAIN_SELECT_WEAPON)
		{
			selectState_ = SELECT_STATE::WEAPON_WINDOW;
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
		}
		else if (mainSelectIndex_ == MAIN_SELECT_SKIN)
		{
			selectState_ = SELECT_STATE::SKIN_WINDOW;
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
		}
		else if (mainSelectIndex_ == MAIN_SELECT_READY)
		{
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);

			if (isMulti_)
			{
				multiState_ = LOBBY_STATE::SELECT_MODE;
				buttonSelectIndex_ = static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON);
			}
			else
			{
				const auto selectedJob = static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_);
				const auto selectedSkin = static_cast<PlayerBase::SKIN_TYPE>(selectedSkinIndex_);
				const std::vector<SceneGame::PlayerSelectType> jobs = { SceneGame::PlayerSelectType{ selectedJob, selectedSkin } };

				SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(jobs));
			}
		}
	}
}

void SceneLobby::UpdateSelectWeaponWindow(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();
	auto& collisionController = CollisionController::GetInstance();
	const int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

	if (inputIntervalCounter_ == 0)
	{
		if (keyConfInputManager.IsPressed("DOWN"))
		{
			selectedJobIndex_ = (selectedJobIndex_ + 1) % jobMax;
			inputIntervalCounter_ = STICK_INTERVAL;
		}
		else if (keyConfInputManager.IsPressed("UP"))
		{
			selectedJobIndex_ = (selectedJobIndex_ - 1 + jobMax) % jobMax;
			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	if (isMouseMode_)
	{
		for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
		{
			auto& weaponCollider = weaponUiCollisions_.at(static_cast<size_t>(jobIndex));
			if (cursorCollider_ != nullptr && weaponCollider != nullptr)
			{
				const bool isHoverItem = collisionController.CheckCollision2D(
					cursorCollider_.get(),
					weaponCollider.get()
				);

				if (isHoverItem)
				{
					selectedJobIndex_ = jobIndex;

					if (keyConfInputManager.IsTriggerDown("OK") || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
					{
						selectState_ = SELECT_STATE::MAIN;
						SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
						SetJobToSKin();
						return;
					}
				}
			}
		}
	}

	if (keyConfInputManager.IsTriggerDown("OK"))
	{
		selectState_ = SELECT_STATE::MAIN;
		SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
		SetJobToSKin();
		return;
	}

	if (keyConfInputManager.IsTriggerDown("CANCEL"))
	{
		selectState_ = SELECT_STATE::MAIN;
	}
}

void SceneLobby::UpdateSelectSkinWindow(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();
	auto& collisionController = CollisionController::GetInstance();
	const int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

	if (inputIntervalCounter_ == 0)
	{
		if (keyConfInputManager.IsPressed("DOWN"))
		{
			selectedSkinIndex_ = (selectedSkinIndex_ + 1) % skinMax;
			inputIntervalCounter_ = STICK_INTERVAL;
		}
		else if (keyConfInputManager.IsPressed("UP"))
		{
			selectedSkinIndex_ = (selectedSkinIndex_ - 1 + skinMax) % skinMax;
			inputIntervalCounter_ = STICK_INTERVAL;
		}
	}

	if (isMouseMode_)
	{
		for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
		{
			auto& skinCollider = skinUiCollisions_.at(static_cast<size_t>(skinIndex));
			if (cursorCollider_ != nullptr && skinCollider != nullptr)
			{
				const bool isHoverItem = collisionController.CheckCollision2D(
					cursorCollider_.get(),
					skinCollider.get()
				);

				if (isHoverItem)
				{
					selectedSkinIndex_ = skinIndex;

					if (keyConfInputManager.IsTriggerDown("OK") || (GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
					{
						selectState_ = SELECT_STATE::MAIN;
						SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
						SetJobToSKin();
						return;
					}
				}
			}
		}
	}

	if (keyConfInputManager.IsTriggerDown("OK"))
	{
		selectState_ = SELECT_STATE::MAIN;
		SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
		SetJobToSKin();
		return;
	}

	if (keyConfInputManager.IsTriggerDown("CANCEL"))
	{
		selectState_ = SELECT_STATE::MAIN;
	}
}

void SceneLobby::UpdatePreviewModel(void)
{
	if (currentModelIndex_ == selectedSkinIndex_)
	{
		return;
	}

	if (previewModelHandle_ != INVALID_HANDLE)
	{
		MV1DeleteModel(previewModelHandle_);
		previewModelHandle_ = INVALID_HANDLE;
		animController_.reset();
	}

	currentModelIndex_ = selectedSkinIndex_;

	ResourceManager::SRC modelSrc = ResourceManager::SRC::NONE;

	switch (static_cast<PlayerBase::SKIN_TYPE>(currentModelIndex_))
	{
	case PlayerBase::SKIN_TYPE::HUMAN:
		modelSrc = ResourceManager::SRC::MODEL_PLAYER_HUMAN;
		break;

	case PlayerBase::SKIN_TYPE::DOG:
		modelSrc = ResourceManager::SRC::MODEL_PLAYER_DOG;
		break;

	case PlayerBase::SKIN_TYPE::MONKEY:
		modelSrc = ResourceManager::SRC::MODEL_PLAYER_MONKEY;
		break;

	case PlayerBase::SKIN_TYPE::BIRD:
		modelSrc = ResourceManager::SRC::MODEL_PLAYER_BIRD;
		break;

	default:
		break;
	}

	if (modelSrc != ResourceManager::SRC::NONE)
	{
		previewModelHandle_ = ResourceManager::GetInstance().LoadModelDuplicate(modelSrc);

		if (previewModelHandle_ != INVALID_HANDLE)
		{
			animController_ = std::make_unique<AnimationController>(previewModelHandle_);
			const int idleAnimHandle = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::ANIM_IDLE);
			animController_->AddExternal(ANIM_IDLE_INDEX, idleAnimHandle, ANIM_IDLE_SPEED);
			animController_->Play(ANIM_IDLE_INDEX, true);
		}
	}
}

void SceneLobby::UpdateMulti(void)
{
	switch (multiState_)
	{
	case LOBBY_STATE::SELECT_EQUIPMENT:
		UpdateSingle();
		break;

	case LOBBY_STATE::SELECT_MODE:
		UpdateSelectMode();
		break;

	case LOBBY_STATE::CONNECTING:
		UpdateConnecting();
		break;

	case LOBBY_STATE::IN_ROOM:
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

	if (inputIntervalCounter_ > 0)
	{
		inputIntervalCounter_--;
	}

	if (!keyConfInputManager.IsPressed("UP") && !keyConfInputManager.IsPressed("DOWN") &&
		!keyConfInputManager.IsPressed("LEFT") && !keyConfInputManager.IsPressed("RIGHT"))
	{
		inputIntervalCounter_ = 0;
	}

	const int digitsCount = static_cast<int>(PASSCODE_DIGITS);

	// パスコード編集中の操作
	if (isEditing_)
	{
		if (inputIntervalCounter_ == 0)
		{
			if (keyConfInputManager.IsPressed("RIGHT"))
			{
				selectOctet_ = (selectOctet_ + 1) % digitsCount;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
			else if (keyConfInputManager.IsPressed("LEFT"))
			{
				selectOctet_ = (selectOctet_ - 1 + digitsCount) % digitsCount;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
			else if (keyConfInputManager.IsPressed("UP"))
			{
				passcode_.at(static_cast<size_t>(selectOctet_)) = (passcode_.at(static_cast<size_t>(selectOctet_)) + 1) % PASSCODE_NUM_MAX;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
			else if (keyConfInputManager.IsPressed("DOWN"))
			{
				passcode_.at(static_cast<size_t>(selectOctet_)) = (passcode_.at(static_cast<size_t>(selectOctet_)) - 1 + PASSCODE_NUM_MAX) % PASSCODE_NUM_MAX;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
		}

		if (keyConfInputManager.IsTriggerDown("OK") || keyConfInputManager.IsTriggerDown("CANCEL"))
		{
			isEditing_ = false;
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);
		}
		return;
	}

	// パッド/キーボードでの上下選択移動
	const int multiUiMax = static_cast<int>(MULTI_UI_TYPE::MAX);
	bool isInputted = false;

	if (inputIntervalCounter_ == 0)
	{
		if (keyConfInputManager.IsPressed("UP"))
		{
			buttonSelectIndex_ = (buttonSelectIndex_ - 1 + multiUiMax) % multiUiMax;
			inputIntervalCounter_ = STICK_INTERVAL;
			isInputted = true;
		}
		else if (keyConfInputManager.IsPressed("DOWN"))
		{
			buttonSelectIndex_ = (buttonSelectIndex_ + 1) % multiUiMax;
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
				const bool isHover = collisionController.CheckCollision2D(
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
	if (keyConfInputManager.IsTriggerDown("OK"))
	{
		constexpr int DIGIT_MULT_THOUSAND = 1000;
		constexpr int DIGIT_MULT_HUNDRED = 100;
		constexpr int DIGIT_MULT_TEN = 10;
		constexpr float CONNECTION_TIMEOUT = 15.0f;

		if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::PASSCODE_BOX))
		{
			isEditing_ = true;
			selectOctet_ = 0;
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);
			return;
		}
		else if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON))
		{
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);

			const int roomWordId =
				passcode_.at(0) * DIGIT_MULT_THOUSAND +
				passcode_.at(1) * DIGIT_MULT_HUNDRED +
				passcode_.at(2) * DIGIT_MULT_TEN +
				passcode_.at(3);

			auto& netManager = NetManager::GetInstance();
			netManager.SetRoomWordId(roomWordId);
			netManager.Run(NET_MODE::HOST);
			netManager.SetConnectionTimeout(CONNECTION_TIMEOUT);

			multiState_ = LOBBY_STATE::CONNECTING;
			return;
		}
		else if (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::CLIENT_BUTTON))
		{
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LOBBY_SELECT);

			const int roomWordId =
				passcode_.at(0) * DIGIT_MULT_THOUSAND +
				passcode_.at(1) * DIGIT_MULT_HUNDRED +
				passcode_.at(2) * DIGIT_MULT_TEN +
				passcode_.at(3);

			constexpr unsigned char BROADCAST_IP_OCTET = 255;
			const IPDATA hostIp = { BROADCAST_IP_OCTET, BROADCAST_IP_OCTET, BROADCAST_IP_OCTET, BROADCAST_IP_OCTET };

			auto& netManager = NetManager::GetInstance();
			netManager.SetHostIp(hostIp);
			netManager.SetRoomWordId(roomWordId);
			netManager.Run(NET_MODE::CLIENT);
			netManager.SetConnectionTimeout(CONNECTION_TIMEOUT);

			multiState_ = LOBBY_STATE::CONNECTING;
			return;
		}
	}

	if (keyConfInputManager.IsTriggerDown("CANCEL"))
	{
		multiState_ = LOBBY_STATE::SELECT_EQUIPMENT;
	}
}

void SceneLobby::UpdateConnecting(void)
{
	if (NetManager::GetInstance().IsHost())
	{
		multiState_ = LOBBY_STATE::IN_ROOM;
		return;
	}

	const auto users = NetManager::GetInstance().GetNetUsers();
	if (!users.empty())
	{
		multiState_ = LOBBY_STATE::IN_ROOM;
	}
}

void SceneLobby::UpdateInRoom(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

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

		if (isHoverYes)
		{
			leaveSelectIndex_ = LEAVE_SELECT_YES;
		}
		else if (isHoverNo)
		{
			leaveSelectIndex_ = LEAVE_SELECT_NO;
		}

		if (inputIntervalCounter_ > 0)
		{
			inputIntervalCounter_--;
		}

		if (!keyConfInputManager.IsPressed("LEFT") && !keyConfInputManager.IsPressed("RIGHT"))
		{
			inputIntervalCounter_ = 0;
		}

		if (inputIntervalCounter_ == 0)
		{
			if (keyConfInputManager.IsPressed("RIGHT") || keyConfInputManager.IsPressed("LEFT"))
			{
				leaveSelectIndex_ = (leaveSelectIndex_ + 1) % LEAVE_SELECT_COUNT;
				inputIntervalCounter_ = STICK_INTERVAL;
			}
		}

		if (keyConfInputManager.IsTriggerDown("OK"))
		{
			SoundManager::GetInstance().Play(SoundManager::SOUND::SE_UI_SELECT);

			if (leaveSelectIndex_ == LEAVE_SELECT_YES)
			{
				NetManager::GetInstance().Send(NET_DATA_TYPE::LEAVE_ROOM);
				NetManager::GetInstance().Stop();
				isLeaveWindow_ = false;
				Initialize();
				return;
			}
			else
			{
				isLeaveWindow_ = false;
				return;
			}
		}

		if (keyConfInputManager.IsTriggerDown("CANCEL"))
		{
			isLeaveWindow_ = false;
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

	if (keyConfInputManager.IsTriggerDown("CANCEL"))
	{
		isLeaveWindow_ = true;
		leaveSelectIndex_ = LEAVE_SELECT_NO;
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

	if (isAllReady && NetManager::GetInstance().IsHost() && !users.empty())
	{
		if (keyConfInputManager.IsTriggerDown("OK"))
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
		if (keyConfInputManager.IsTriggerDown("OK"))
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
	case LOBBY_STATE::SELECT_EQUIPMENT:
		DrawCenterModel();

		DrawRotaGraph3(
			Application::SCREEN_HALF_X + START_BUTTON_OFFSET_X,
			Application::SCREEN_SIZE_Y - START_BUTTON_OFFSET_Y,
			uiBackWidth_ / 2,
			uiBackHeight_ / 2,
			START_BTN_BACK_SCALE_X,
			START_BTN_BACK_SCALE_Y,
			0.0f,
			(mainSelectIndex_ == MAIN_SELECT_READY) ? selectedUIBackHandle_ : selectUIBackHandle_,
			true
		);

		DrawRotaGraph(
			Application::SCREEN_HALF_X + START_BUTTON_OFFSET_X,
			Application::SCREEN_SIZE_Y - START_BUTTON_OFFSET_Y,
			COMMON_TEXT_SCALE,
			0.0,
			uiHandles_.at(static_cast<size_t>(UI_SINGLE::FORMATION)),
			true
		);

		DrawSelectionPanels();
		break;

	case LOBBY_STATE::SELECT_MODE:
		DrawSelectMode();
		break;

	case LOBBY_STATE::CONNECTING:
		DrawConnecting();
		break;

	case LOBBY_STATE::IN_ROOM:
		DrawInRoom();
		break;

	default:
		break;
	}
}

void SceneLobby::DrawSelectMode(void)
{
	constexpr double PASSCODE_BOX_SCALE = 0.4;
	constexpr double NUMBER_SCALE = 0.5;
	constexpr double EDIT_HIGHLIGHT_SCALE = 0.65;
	constexpr int DIGIT_SPACING = 100;
	constexpr float DIGIT_START_OFFSET_FACTOR = 1.5f;

	const int ipBoxX = (Application::SCREEN_SIZE_X - IP_BOX_WIDTH) / 2;

	const bool isPasscodeSelected = (buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::PASSCODE_BOX));
	const bool isHighlight = (isEditing_ || isPasscodeSelected);

	const int boxX = ipBoxX + (IP_BOX_WIDTH / 2);
	const int boxY = PASSCODE_BOX_POS_Y;
	const int currentPasscodeBoxHandle = isHighlight ? selectedMultiHandle_ : selectMultiHandle_;

	// パスコード枠背景描画
	DrawRotaGraph(boxX, boxY, PASSCODE_BOX_SCALE, 0.0, currentPasscodeBoxHandle, true);

	// パスコード数字画像の描画
	const int startDigitX = boxX - static_cast<int>(static_cast<float>(DIGIT_SPACING) * DIGIT_START_OFFSET_FACTOR);
	const int digitY = boxY;
	const int digitsCount = static_cast<int>(PASSCODE_DIGITS);

	for (int digitIndex = 0; digitIndex < digitsCount; ++digitIndex)
	{
		const int currentDigitX = startDigitX + (digitIndex * DIGIT_SPACING);
		const int numberValue = passcode_.at(static_cast<size_t>(digitIndex));
		const bool isEditingCurrentDigit = (isEditing_ && selectOctet_ == digitIndex);
		const double currentScale = isEditingCurrentDigit ? EDIT_HIGHLIGHT_SCALE : NUMBER_SCALE;

		DrawRotaGraph(currentDigitX, digitY, currentScale, 0.0, passcodeTextHandles_.at(static_cast<size_t>(numberValue)), true);
	}

	// ホストボタン描画
	const bool isHostSelected = (!isEditing_ && buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::HOST_BUTTON));
	const int hostBackgroundHandle = isHostSelected ? selectedUIBackHandle_ : selectUIBackHandle_;

	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		HOST_BUTTON_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		START_BTN_BACK_SCALE_X,
		START_BTN_BACK_SCALE_Y,
		0.0f,
		hostBackgroundHandle,
		true
	);

	DrawRotaGraph(
		Application::SCREEN_HALF_X,
		HOST_BUTTON_Y,
		COMMON_TEXT_SCALE,
		0.0,
		uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::ROOM)),
		true
	);

	// クライアントボタン描画
	const bool isClientSelected = (!isEditing_ && buttonSelectIndex_ == static_cast<int>(MULTI_UI_TYPE::CLIENT_BUTTON));
	const int clientBackgroundHandle = isClientSelected ? selectedUIBackHandle_ : selectUIBackHandle_;

	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		CLIENT_BUTTON_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		START_BTN_BACK_SCALE_X,
		START_BTN_BACK_SCALE_Y,
		0.0f,
		clientBackgroundHandle,
		true
	);

	DrawRotaGraph(
		Application::SCREEN_HALF_X,
		CLIENT_BUTTON_Y,
		COMMON_TEXT_SCALE,
		0.0,
		uiTexHandles_.at(static_cast<size_t>(UI_MAIN_TEXT::JOIN)),
		true
	);

#ifdef _DEBUG
	CollisionController::GetInstance().DrawDebug2D();
#endif
}

void SceneLobby::DrawConnecting(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);
	DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 1.0, 0.0, connectTexHandle_, true);
}

void SceneLobby::DrawInRoom(void)
{
	int width = 0;
	int height = 0;
	GetGraphSize(selectMultiHandle_, &width, &height);

	constexpr int TITLE_POS_X = 300;
	constexpr int TITLE_POS_Y = 100;
	constexpr float TITLE_BACK_SCALE = 0.3f;

	DrawRotaGraph3(
		TITLE_POS_X,
		TITLE_POS_Y,
		width / 2,
		height / 2,
		TITLE_BACK_SCALE,
		TITLE_BACK_SCALE,
		0.0f,
		selectMultiHandle_,
		true
	);

	DrawRotaGraph(TITLE_POS_X, TITLE_POS_Y, COMMON_TEXT_SCALE, 0.0, multiTitleHandle_, true);

	// 参加者データの集約
	std::vector<NET_JOIN_USER> allPlayers;
	const NET_JOIN_USER selfUser = NetManager::GetInstance().GetSelfUser();
	allPlayers.push_back(selfUser);

	const auto netUsers = NetManager::GetInstance().GetNetUsers();
	for (const auto& pair : netUsers)
	{
		if (static_cast<int>(allPlayers.size()) >= MAX_PLAYERS)
		{
			break;
		}
		allPlayers.push_back(pair.second);
	}

	// 横並び配置の計算用定数
	constexpr int MAX_SLOT_COUNT = 4;
	constexpr float PANEL_WIDTH = 300.0f;
	constexpr float PANEL_HEIGHT = 700.0f;
	constexpr float PANEL_START_POS_Y = 120.0f;
	constexpr int MODEL_AREA_BOTTOM_MARGIN = 70;
	constexpr int STATUS_IMG_OFFSET_Y = 35;
	constexpr float STATUS_BACK_SCALE = 0.2f;

	constexpr float PANEL_MODEL_START_X = -380.0f;
	constexpr float PANEL_MODEL_OFFSET_X = 253.0f;
	constexpr float PANEL_MODEL_POS_Y = -250.0f;
	constexpr float PANEL_MODEL_ROT_X = 0.1f;
	constexpr VECTOR PANEL_MODEL_SCALE = { 4.0f, 4.0f, 4.0f };

	const float totalWidth = static_cast<float>(Application::SCREEN_SIZE_X);
	const float totalPanelsWidth = PANEL_WIDTH * static_cast<float>(MAX_SLOT_COUNT);
	const float gapWidth = (totalWidth - totalPanelsWidth) / static_cast<float>(MAX_SLOT_COUNT + 1);

	for (int i = 0; i < MAX_SLOT_COUNT; ++i)
	{
		const float panelLeft = gapWidth + static_cast<float>(i) * (PANEL_WIDTH + gapWidth);
		const float panelTop = PANEL_START_POS_Y;
		const float panelRight = panelLeft + PANEL_WIDTH;
		const float panelBottom = panelTop + PANEL_HEIGHT;

		const int panelLeftInt = static_cast<int>(panelLeft);
		const int panelRightInt = static_cast<int>(panelRight);
		const int panelBottomInt = static_cast<int>(panelBottom);

		if (i < static_cast<int>(allPlayers.size()))
		{
			const NET_JOIN_USER& user = allPlayers.at(static_cast<size_t>(i));
			const bool isSelf = (i == 0);
			const bool isReady = (user.gameState >= GAME_STATE::GOTO_GAME);

			const int modelAreaBottom = panelBottomInt - MODEL_AREA_BOTTOM_MARGIN;
			const int currentSkinIndex = isSelf ? selectedSkinIndex_ : user.selectedSkinType;
			const int lobbyModelHandle = GetLobbySkinModelHandle(currentSkinIndex);

			if (lobbyModelHandle != INVALID_HANDLE)
			{
				const float modelPosX = PANEL_MODEL_START_X + (static_cast<float>(i) * PANEL_MODEL_OFFSET_X);
				MV1SetPosition(lobbyModelHandle, VGet(modelPosX, PANEL_MODEL_POS_Y, 0.0f));
				MV1SetRotationXYZ(lobbyModelHandle, VGet(PANEL_MODEL_ROT_X, 0.0f, 0.0f));
				MV1SetScale(lobbyModelHandle, PANEL_MODEL_SCALE);

				MV1DrawModel(lobbyModelHandle);
			}

			const int statusImageCenterX = (panelLeftInt + panelRightInt) / 2;
			const int statusImageCenterY = modelAreaBottom + STATUS_IMG_OFFSET_Y;

			const int readyImageHandle = isReady
				? uiReadyHandles_.at(static_cast<size_t>(UI_READY_TEXT::READY))
				: uiReadyHandles_.at(static_cast<size_t>(UI_READY_TEXT::WAITING));

			if (readyImageHandle != INVALID_HANDLE)
			{
				DrawRotaGraph3(
					statusImageCenterX,
					statusImageCenterY,
					width / 2,
					height / 2,
					STATUS_BACK_SCALE,
					STATUS_BACK_SCALE,
					0.0f,
					selectMultiHandle_,
					true
				);
				DrawRotaGraph(statusImageCenterX, statusImageCenterY, COMMON_TEXT_SCALE, 0.0, readyImageHandle, true);
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

	if (isAllReady && !netUsers.empty())
	{
		DrawRotaGraph(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 1.0, 0.0, allReadyImageHandle_, true);
	}

	if (isLeaveWindow_)
	{
		DrawLeaveConfirmWindow();
	}
}

void SceneLobby::MoveToGameScene(std::map<int, NET_JOIN_USER>& _users)
{
	std::vector<SceneGame::PlayerSelectType> playerSelectTypes;

	// 自分自身の選択情報
	SceneGame::PlayerSelectType myType{};
	myType.job = static_cast<PlayerBase::JOB_TYPE>(selectedJobIndex_);
	myType.skin = static_cast<PlayerBase::SKIN_TYPE>(selectedSkinIndex_);
	playerSelectTypes.push_back(myType);

	// 他の参加プレイヤーの選択情報
	for (const auto& pair : _users)
	{
		if (static_cast<int>(playerSelectTypes.size()) >= MAX_PLAYERS)
		{
			break;
		}

		SceneGame::PlayerSelectType otherType{};
		otherType.job = static_cast<PlayerBase::JOB_TYPE>(pair.second.selectedJobType);
		otherType.skin = static_cast<PlayerBase::SKIN_TYPE>(pair.second.selectedSkinType);
		playerSelectTypes.push_back(otherType);
	}

	SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>(playerSelectTypes));
}

void SceneLobby::SetJobToSKin(void)
{
	if (!isMulti_)
	{
		return;
	}

	NET_JOIN_USER self = NetManager::GetInstance().GetSelfUser();
	self.selectedJobType = selectedJobIndex_;
	self.selectedSkinType = selectedSkinIndex_;
	NetManager::GetInstance().SetSelfInfo(self);
}

void SceneLobby::DrawLeaveConfirmWindow(void)
{
	constexpr float LEAVE_WINDOW_SCALE_X = 1.2f;
	constexpr float LEAVE_WINDOW_SCALE_Y = 1.0f;
	constexpr int LEAVE_TEXT_OFFSET_Y = 80;
	constexpr double LEAVE_TEXT_SCALE = 0.8;

	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FILTER_BLEND_ALPHA);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// メインウィンドウ背景の描画
	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		LEAVE_WINDOW_SCALE_X,
		LEAVE_WINDOW_SCALE_Y,
		0.0f,
		selectUIBackHandle_,
		true
	);

	// 部屋を出る確認テキスト画像の描画
	if (leaveRoomTextHandle_ != INVALID_HANDLE)
	{
		DrawRotaGraph(
			Application::SCREEN_HALF_X,
			Application::SCREEN_HALF_Y - LEAVE_TEXT_OFFSET_Y,
			LEAVE_TEXT_SCALE,
			0.0,
			leaveRoomTextHandle_,
			true
		);
	}

	const int yesPosX = Application::SCREEN_HALF_X - LEAVE_BTN_OFFSET_X;
	const int noPosX = Application::SCREEN_HALF_X + LEAVE_BTN_OFFSET_X;
	const int buttonPosY = Application::SCREEN_HALF_Y + LEAVE_BTN_OFFSET_Y;

	// 「はい」文字画像描画
	const int yesHandle = (leaveSelectIndex_ == LEAVE_SELECT_YES)
		? selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES))
		: noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::YES));

	if (yesHandle != INVALID_HANDLE)
	{
		DrawRotaGraph(yesPosX, buttonPosY, COMMON_TEXT_SCALE, 0.0, yesHandle, true);
	}

	// 「いいえ」文字画像描画
	const int noHandle = (leaveSelectIndex_ == LEAVE_SELECT_NO)
		? selectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO))
		: noSelectConfTextHandles_.at(static_cast<size_t>(CONFIRM_TEXT::NO));

	if (noHandle != INVALID_HANDLE)
	{
		DrawRotaGraph(noPosX, buttonPosY, COMMON_TEXT_SCALE, 0.0, noHandle, true);
	}
}

void SceneLobby::DrawWeaponWindow(void)
{
	// ウィンドウ背景画像描画
	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		WINDOW_BACK_SCALE_X,
		WINDOW_BACK_SCALE_Y,
		0.0f,
		selectUIBackHandle_,
		true
	);

	constexpr float WEAPON_ITEM_OFFSET_X = 265.0f;
	constexpr double WEAPON_INFO_SCALE = 0.435;
	constexpr int WEAPON_INFO_OFFSET_X = 563;

	const float itemCenterX = static_cast<float>(Application::SCREEN_HALF_X) - WEAPON_ITEM_OFFSET_X;
	const float itemStartY = static_cast<float>(Application::SCREEN_HALF_Y) - WINDOW_ITEM_START_OFFSET_Y;
	const int jobMax = static_cast<int>(PlayerBase::JOB_TYPE::MAX);

	for (int jobIndex = 0; jobIndex < jobMax; ++jobIndex)
	{
		const float currentItemY = itemStartY + (static_cast<float>(jobIndex) * WINDOW_ITEM_INTERVAL_Y);
		const bool isSelected = (selectedJobIndex_ == jobIndex);
		const auto currentJobType = static_cast<PlayerBase::JOB_TYPE>(jobIndex);
		const int optionHandle = GetWeaponUIHandle(currentJobType, isSelected);

		if (optionHandle != INVALID_HANDLE)
		{
			DrawRotaGraph(static_cast<int>(itemCenterX), static_cast<int>(currentItemY), COMMON_TEXT_SCALE, 0.0, optionHandle, true);
		}
	}

	// 武器説明の描画
	const int infoCenterX = static_cast<int>(itemCenterX) + WEAPON_INFO_OFFSET_X;
	DrawRotaGraph(
		infoCenterX,
		Application::SCREEN_HALF_Y,
		WEAPON_INFO_SCALE,
		0.0,
		weaponInfoHandle_.at(static_cast<size_t>(selectedJobIndex_)),
		true
	);
}

void SceneLobby::DrawSkinWindow(void)
{
	// ウィンドウ背景画像描画
	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		uiBackWidth_ / 2,
		uiBackHeight_ / 2,
		WINDOW_BACK_SCALE_X,
		WINDOW_BACK_SCALE_Y,
		0.0f,
		selectUIBackHandle_,
		true
	);

	const float itemCenterX = static_cast<float>(Application::SCREEN_HALF_X);
	const float itemStartY = static_cast<float>(Application::SCREEN_HALF_Y) - WINDOW_ITEM_START_OFFSET_Y;
	const int skinMax = static_cast<int>(PlayerBase::SKIN_TYPE::MAX);

	for (int skinIndex = 0; skinIndex < skinMax; ++skinIndex)
	{
		const float currentItemY = itemStartY + (static_cast<float>(skinIndex) * WINDOW_ITEM_INTERVAL_Y);
		const bool isSelected = (selectedSkinIndex_ == skinIndex);
		const auto currentSkinType = static_cast<PlayerBase::SKIN_TYPE>(skinIndex);
		const int optionHandle = GetSkinUIHandle(currentSkinType, isSelected);

		if (optionHandle != INVALID_HANDLE)
		{
			DrawRotaGraph(static_cast<int>(itemCenterX), static_cast<int>(currentItemY), COMMON_TEXT_SCALE, 0.0, optionHandle, true);
		}
	}
}

int SceneLobby::GetLobbySkinModelHandle(int _skinIndex) const
{
	if (_skinIndex < 0 || _skinIndex >= static_cast<int>(lobbySkinHandles_.size()))
	{
		return INVALID_HANDLE;
	}
	return lobbySkinHandles_.at(static_cast<size_t>(_skinIndex));
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
		return INVALID_HANDLE;
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
		return INVALID_HANDLE;
	}
}