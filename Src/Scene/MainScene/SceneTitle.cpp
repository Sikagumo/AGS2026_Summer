#include "SceneTitle.h"

#include <cmath>

#include "../../Application.h"
#include "../../Camera/Camera.h"
#include "../../Common/Loading.h"
#include "../../ImGUI/GuiController.h"
#include "../../Manager/Decoration/EffectManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/TimeManager.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Shader/ShaderController.h"
#include "../../Shader/ShaderParameters.h"
#include "../../Utility/UtilityMath.h"
#include "../SubScene/SceneOperation.h"
#include "SceneGame.h"
#include "SceneLobby.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1;                          // 無効なリソースハンドルを示す値
	constexpr float LOADING_PROGRESS_INIT = 10.0f;              // ローディング進捗初期値
	constexpr float CURSOR_RADIUS = 1.0f;                       // カーソル用円コライダーの半径
	constexpr float BUTTON_WIDTH = 250.0f;                      // メニューボタンの幅
	constexpr float BUTTON_HEIGHT = 50.0f;                      // メニューボタンの高さ
	constexpr float TITLE_BUTTON_START_Y_OFFSET = 20.0f;        // ボタン配置の開始Yオフセット
	constexpr float BUTTON_INTERVAL_Y = 100.0f;                 // ボタン間の縦方向の間隔
	constexpr Vector2F CURSOR_OUTOF_SCREEN = { -1000.0f, -1000.0f }; // 画面外にカーソルを退避させる座標
	constexpr int STICK_INPUT_INTERVAL = 15;                    // スティック入力の連続受付間隔
	constexpr float TIME_INCREMENT = 0.02f;                     // 経過時間の増加量
	constexpr float PEACH_DRAW_SCALE = 0.5f;                    // 桃の描画スケール
	constexpr float WAVE_DRAW_SCALE = 1.0f;                     // 波の描画スケール
	constexpr float ONISIMA_DRAW_SCALE = 1.0f;                  // 鬼ヶ島の描画スケール
	constexpr float BOBBING_AMPLITUDE = 20.0f;                  // 上下フロートの振幅
	constexpr float BOBBING_SPEED = 3.0f;                       // 上下フロートの速度
	constexpr float LIGHT_VECTOR_DEFAULT = 0.5f;                // ライトベクトルのデフォルト値
	constexpr float AMBIENT_RATE_DEFAULT = 0.8f;                // アンビエント光の割合
	constexpr float WAVE_SPEED_DEFAULT = 3.0f;                  // 波の移動速度
	constexpr float WAVE_FORCE_DEFAULT = 0.015f;                // 波の強さ
	constexpr int ONISIMA_POS_X_OFFSET = 100;                   // 鬼ヶ島描画のX方向オフセット
	constexpr int ONISIMA_POS_Y = 30;                           // 鬼ヶ島描画のY方向位置
	constexpr int WAVE_POS_Y = 20;                              // 波描画のY方向位置
	constexpr int PEACH_POS_X = -20;                            // 桃描画のX方向位置
	constexpr int TITLE_IMAGE_Y = 0;                            // タイトル画像描画のY方向位置
	constexpr float MENU_BUTTON_SCALE = 0.5f;                   // メニューボタンの描画スケール
}

SceneTitle::SceneTitle(void)
	: imageTitle_(INVALID_HANDLE)
	, imageMenu_()
	, buttonTags()
	, peachHandle_(INVALID_HANDLE)
	, peachNormalHandle_(INVALID_HANDLE)
	, waveHandle_(INVALID_HANDLE)
	, waveNormalHandle_(INVALID_HANDLE)
	, oniSimaHandle_(INVALID_HANDLE)
	, oniSimaNormalHandle_(INVALID_HANDLE)
	, titleNormalHandle_(INVALID_HANDLE)
	, backgroundHandle_(INVALID_HANDLE)
	, backNormalHandle_(INVALID_HANDLE)
	, time_(0.0f)
	, selectedIdx_(0)
	, inputIntervalCounter_(0)
	, prevMousePos_(0.0f, 0.0f)
	, isMouseMode_(true)
	, peachGui_(nullptr)
	, waveGui_(nullptr)
	, oniSimaGui_(nullptr)
	, titleGui_(nullptr)
	, cursorCollider_(nullptr)
	, soloPlayButtonCollider_(nullptr)
	, multiPlayButtonCollider_(nullptr)
	, optionButtonCollider_(nullptr)
	, exitButtonCollider_(nullptr)
	, peachCollider_(nullptr)
	, waveCollider_(nullptr)
	, oniSimaCollider_(nullptr)
	, isSelectMenu_(true)
{
	for (size_t i = 0; i < imageMenu_.size(); ++i)
	{
		imageMenu_[i] = INVALID_HANDLE;
	}
}

void SceneTitle::Load(void)
{
	SceneBase::Load();

	Loading::GetInstance()->SetProgress(LOADING_PROGRESS_INIT);

	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_SEA,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_SEA));
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE_THUNDER,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_TITLE_SELECT));

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_UI_SELECT,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_UI_SELECT));

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_UI_CANCEL,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_UI_CANCEL));

	imageTitle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_TITLE);
	titleNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_NOTMALMAP_TITLE);

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_TITLE_TEXT, imageMenu_.data());

	peachHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_PEACH);
	peachNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_PEACH);

	waveHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_WAVE);
	waveNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_WAVE);

	oniSimaHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_ONIGASIMA);
	oniSimaNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_NOMALMAP_ONIGASIMA);

	backgroundHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_BUCGROUND_TITLE);
	backNormalHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_BUCG_NORMALMAP_TITLE);

	TimeManager::GetInstance().Reset();
}

void SceneTitle::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneTitle::Initialize(void)
{
	if (Loading::GetInstance()->IsLoading())
	{
		return;
	}

	SetMouseDispFlag(true);

	const auto mousePos = KeyConfInputManager::GetInstance().GetMousePosition();
	prevMousePos_ = Vector2F(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

	// マウスカーソル用のコライダー生成
	cursorCollider_ = std::make_unique<Collider2DCircle>(Vector2F(0.0f, 0.0f), CURSOR_RADIUS, Collider2DBase::TAG_2D::MOUSE_CURSOR);
	CollisionController::GetInstance().RegisterCollider2D(cursorCollider_.get());

	InitUI();

	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);

	isSelectMenu_ = true;

	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_SEA);
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_SELECT));
}

void SceneTitle::InitUI(void)
{
	const float startY = Application::SCREEN_HALF_Y + TITLE_BUTTON_START_Y_OFFSET;
	const float centerX = Application::SCREEN_HALF_X;

	// ソロプレイボタン
	const Vector2F posSolo(centerX, startY + (BUTTON_INTERVAL_Y * 0.0f));
	soloPlayButtonCollider_ = std::make_unique<Collider2DBox>(posSolo, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON);
	CollisionController::GetInstance().RegisterCollider2D(soloPlayButtonCollider_.get());
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON, true);

	// マルチプレイボタン
	const Vector2F posMulti(centerX, startY + (BUTTON_INTERVAL_Y * 1.0f));
	multiPlayButtonCollider_ = std::make_unique<Collider2DBox>(posMulti, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON);
	CollisionController::GetInstance().RegisterCollider2D(multiPlayButtonCollider_.get());
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON, true);

	// 設定ボタン
	const Vector2F posOption(centerX, startY + (BUTTON_INTERVAL_Y * 2.0f));
	optionButtonCollider_ = std::make_unique<Collider2DBox>(posOption, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::OPTION_BUTTON);
	CollisionController::GetInstance().RegisterCollider2D(optionButtonCollider_.get());
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::OPTION_BUTTON, true);

	// 終了ボタン
	const Vector2F posExit(centerX, startY + (BUTTON_INTERVAL_Y * 3.0f));
	exitButtonCollider_ = std::make_unique<Collider2DBox>(posExit, BUTTON_WIDTH, BUTTON_HEIGHT, Collider2DBase::TAG_2D::EXIT_BUTTON);
	CollisionController::GetInstance().RegisterCollider2D(exitButtonCollider_.get());
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::EXIT_BUTTON, true);

	buttonTags =
	{
		Collider2DBase::TAG_2D::SOLO_PLAY_BUTTON,
		Collider2DBase::TAG_2D::MULTI_PLAY_BUTTON,
		Collider2DBase::TAG_2D::OPTION_BUTTON,
		Collider2DBase::TAG_2D::EXIT_BUTTON
	};

#ifdef _DEBUG
	const Vector2F peachPos(100.0f, Application::SCREEN_HALF_Y + 100);
	const Vector2F wavePos(800.0f, Application::SCREEN_HALF_Y + 250);
	const Vector2F oniSimaPos(Application::SCREEN_SIZE_X - 100.0f, Application::SCREEN_HALF_Y - 50);

	peachCollider_ = std::make_unique<Collider2DBox>(peachPos, 300.0f, 300.0f, Collider2DBase::TAG_2D::PEACH);
	waveCollider_ = std::make_unique<Collider2DBox>(wavePos, static_cast<float>(Application::SCREEN_SIZE_X), static_cast<float>(Application::SCREEN_HALF_Y), Collider2DBase::TAG_2D::WAVE);
	oniSimaCollider_ = std::make_unique<Collider2DBox>(oniSimaPos, 200.0f, 150.0f, Collider2DBase::TAG_2D::ONI_GASHIMA);

	CollisionController::GetInstance().RegisterCollider2D(peachCollider_.get());
	CollisionController::GetInstance().RegisterCollider2D(waveCollider_.get());
	CollisionController::GetInstance().RegisterCollider2D(oniSimaCollider_.get());

	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::PEACH, true);
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::WAVE, true);
	CollisionController::GetInstance().SetCollisionGroup2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, Collider2DBase::TAG_2D::ONI_GASHIMA, true);
#endif // _DEBUG

	isSelectMenu_ = true;

	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_SEA);
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE_THUNDER);

	SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT,
		ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::SE_SELECT));
}

void SceneTitle::Update(void)
{
	if (Loading::GetInstance()->IsLoading())
	{
		return;
	}

	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	const Vector2 mousePos = keyConfInputManager.GetMousePosition();
	const Vector2F mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

	if (mousePosF.x != prevMousePos_.x || mousePosF.y != prevMousePos_.y ||
		(GetMouseInput() & (MOUSE_INPUT_LEFT | MOUSE_INPUT_RIGHT)) != 0)
	{
		isMouseMode_ = true;
		SetMouseDispFlag(true);
	}

	if (keyConfInputManager.IsPressed("UP") || keyConfInputManager.IsPressed("DOWN") ||
		keyConfInputManager.IsPressed("LEFT") || keyConfInputManager.IsPressed("RIGHT") ||
		keyConfInputManager.IsTriggerDown("OK") || keyConfInputManager.IsTriggerDown("CANCEL"))
	{
		isMouseMode_ = false;
		SetMouseDispFlag(false);
	}

	prevMousePos_ = mousePosF;

	if (isMouseMode_)
	{
		cursorCollider_->SetCenterPos(mousePosF);
	}
	else
	{
		cursorCollider_->SetCenterPos(CURSOR_OUTOF_SCREEN);
	}

#ifdef _DEBUG
	if (KeyConfInputManager::GetInstance().IsTriggerDown("APPLY_DEBUG"))
	{
		isDebugMode_ = true;
	}

	if (isDebugMode_ == true)
	{
		if (KeyConfInputManager::GetInstance().IsTriggerDown("UNAPPLY_DEBUG"))
		{
			isDebugMode_ = false;
		}

		UpdateGui();
		return;
	}
#endif

	if (isSelectMenu_)
	{
		if (inputIntervalCounter_ > 0)
		{
			inputIntervalCounter_--;
		}

		if (inputIntervalCounter_ <= 0)
		{
			if (keyConfInputManager.IsPressed("DOWN"))
			{
				selectedIdx_ = (selectedIdx_ + 1) % MENU_BUTTON_NUM;
				inputIntervalCounter_ = STICK_INPUT_INTERVAL;
			}
			else if (keyConfInputManager.IsPressed("UP"))
			{
				selectedIdx_ = (selectedIdx_ - 1 + MENU_BUTTON_NUM) % MENU_BUTTON_NUM;
				inputIntervalCounter_ = STICK_INPUT_INTERVAL;
			}
		}

		if (!keyConfInputManager.IsPressed("DOWN") && !keyConfInputManager.IsPressed("UP"))
		{
			inputIntervalCounter_ = 0;
		}

		for (int i = 0; i < MENU_BUTTON_NUM; ++i)
		{
			if (CollisionController::GetInstance().IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, buttonTags[i]))
			{
				selectedIdx_ = i;
				break;
			}
		}

		for (int i = 0; i < MENU_BUTTON_NUM; ++i)
		{
			const bool isTarget = CollisionController::GetInstance().IsTagCollidingWithTag2D(Collider2DBase::TAG_2D::MOUSE_CURSOR, buttonTags[i]) || (selectedIdx_ == i);

			if (isTarget && keyConfInputManager.IsTriggerDown("OK"))
			{
				isSelectMenu_ = false;
				SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
			}
		}
	}
	else
	{
		if (selectedIdx_ == static_cast<int>(MENU_ITEM::OPTION))
		{
			SceneManager::GetInstance().PushScene(std::make_shared<SceneOperation>());
			isSelectMenu_ = true;
		}
		else if (!SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_SELECT))
		{
			ProcessMenuState();
		}
	}
}

void SceneTitle::Draw(void)
{
	time_ += TIME_INCREMENT;

	const float bobOffsetY = std::sinf(time_ * BOBBING_SPEED) * BOBBING_AMPLITUDE;
	auto& shaderCtrl = ShaderController::GetInstance();

	// 背景の描画
	NormalWaveShaderParams backgroundParams;
	backgroundParams.lightVectorX = LIGHT_VECTOR_DEFAULT;
	backgroundParams.lightVectorY = LIGHT_VECTOR_DEFAULT;
	backgroundParams.lightVectorZ = LIGHT_VECTOR_DEFAULT;
	backgroundParams.ambientRate = AMBIENT_RATE_DEFAULT;
	backgroundParams.timeValue = 0;
	backgroundParams.useNormalMapFlag = 1.0f;

	shaderCtrl.Draw2D(
		ResourceManager::SRC::PS_NORMAL_MAP,
		0.0f, 0.0f,
		1.0f,
		backgroundParams,
		backgroundHandle_,
		backNormalHandle_
	);

	// 鬼ヶ島の描画
	NormalWaveShaderParams oniSimaParams;
	oniSimaParams.lightVectorX = LIGHT_VECTOR_DEFAULT;
	oniSimaParams.lightVectorY = LIGHT_VECTOR_DEFAULT;
	oniSimaParams.lightVectorZ = LIGHT_VECTOR_DEFAULT;
	oniSimaParams.ambientRate = AMBIENT_RATE_DEFAULT;
	oniSimaParams.timeValue = 0;
	oniSimaParams.useNormalMapFlag = 1.0f;

	shaderCtrl.Draw2D(
		ResourceManager::SRC::PS_NORMAL_MAP,
		static_cast<int>(Application::SCREEN_HALF_X - ONISIMA_POS_X_OFFSET), ONISIMA_POS_Y,
		ONISIMA_DRAW_SCALE,
		oniSimaParams,
		oniSimaHandle_,
		oniSimaNormalHandle_
	);

	// 波の描画
	NormalWaveShaderParams waveParams;
	waveParams.lightVectorX = LIGHT_VECTOR_DEFAULT;
	waveParams.lightVectorY = LIGHT_VECTOR_DEFAULT;
	waveParams.lightVectorZ = LIGHT_VECTOR_DEFAULT;
	waveParams.ambientRate = AMBIENT_RATE_DEFAULT;
	waveParams.timeValue = time_;
	waveParams.waveSpeedValue = WAVE_SPEED_DEFAULT;
	waveParams.waveForceValue = WAVE_FORCE_DEFAULT;
	waveParams.useNormalMapFlag = 1.0f;

	shaderCtrl.Draw2D(
		ResourceManager::SRC::PS_NORMAL_MAP,
		0, WAVE_POS_Y,
		WAVE_DRAW_SCALE,
		waveParams,
		waveHandle_,
		waveNormalHandle_
	);

	// 桃の描画
	NormalWaveShaderParams peachParams;
	peachParams.lightVectorX = LIGHT_VECTOR_DEFAULT;
	peachParams.lightVectorY = LIGHT_VECTOR_DEFAULT;
	peachParams.lightVectorZ = LIGHT_VECTOR_DEFAULT;
	peachParams.ambientRate = AMBIENT_RATE_DEFAULT;
	peachParams.timeValue = time_;
	peachParams.useNormalMapFlag = 1.0f;

	shaderCtrl.Draw2D(
		ResourceManager::SRC::PS_NORMAL_MAP,
		PEACH_POS_X, static_cast<int>(Application::SCREEN_HALF_Y + bobOffsetY),
		PEACH_DRAW_SCALE,
		peachParams,
		peachHandle_,
		peachNormalHandle_
	);

	const int imageTitleX = (Application::SCREEN_HALF_X / 2) + 20;

	// タイトル画像の描画
	NormalWaveShaderParams titleParams;
	titleParams.lightVectorX = 0.0f;
	titleParams.lightVectorY = 0.0f;
	titleParams.lightVectorZ = 0.0f;
	titleParams.ambientRate = AMBIENT_RATE_DEFAULT;
	titleParams.timeValue = time_;
	titleParams.waveSpeedValue = 0.0f;
	titleParams.waveForceValue = 0.0f;
	titleParams.useNormalMapFlag = 1.0f;

	shaderCtrl.Draw2D(
		ResourceManager::SRC::PS_NORMAL_MAP,
		imageTitleX, TITLE_IMAGE_Y,
		PEACH_DRAW_SCALE,
		titleParams,
		imageTitle_,
		titleNormalHandle_
	);

	shaderCtrl.ExecuteDrawCommands();

	using TAG_2D = Collider2DBase::TAG_2D;

	std::array<Collider2DBox*, 4> colliders =
	{
		soloPlayButtonCollider_.get(),
		multiPlayButtonCollider_.get(),
		optionButtonCollider_.get(),
		exitButtonCollider_.get()
	};

	for (int i = 0; i < MENU_BUTTON_NUM; ++i)
	{
		const bool isSelected = CollisionController::GetInstance().IsTagCollidingWithTag2D(TAG_2D::MOUSE_CURSOR, buttonTags[i]) || (selectedIdx_ == i);
		const int imgIdx = isSelected ? (i * 2 + 1) : (i * 2);

		const Vector2F pos = colliders[i]->GetCenterPos();
		DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), MENU_BUTTON_SCALE, UtilityMath::DEG2RAD, imageMenu_[imgIdx], true);
	}

#ifdef _DEBUG
	DrawDebug();
#endif
}

void SceneTitle::Release(void)
{
}

void SceneTitle::DrawDebug(void)
{
	CollisionController::GetInstance().DrawDebug2D();
}

void SceneTitle::ProcessMenuState(void)
{
	switch (static_cast<MENU_ITEM>(selectedIdx_))
	{
	case MENU_ITEM::SOLO:
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneLobby>(false));
	}
	break;

	case MENU_ITEM::MULTI:
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneLobby>(true));
	}
	break;

	case MENU_ITEM::OPTION:
	{
		SceneManager::GetInstance().PushScene(std::make_shared<SceneOperation>());
	}
	break;

	case MENU_ITEM::EXIT:
	{
		Application::GetInstance().GameEnd();
	}
	break;

	default:
		break;
	}
}

void SceneTitle::UpdateGui(void)
{
	if (KeyConfInputManager::GetInstance().IsTriggerDown("OK"))
	{
		auto& colCtrl = CollisionController::GetInstance();
		using TAG = Collider2DBase::TAG_2D;

		if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::PEACH))
		{
			GuiController::GetInstance().SetActiveGui(peachGui_);
		}
		else if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::WAVE))
		{
			GuiController::GetInstance().SetActiveGui(waveGui_);
		}
		else if (colCtrl.IsTagCollidingWithTag2D(TAG::MOUSE_CURSOR, TAG::ONI_GASHIMA))
		{
			GuiController::GetInstance().SetActiveGui(oniSimaGui_);
		}
	}
}