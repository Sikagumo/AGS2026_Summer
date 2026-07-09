#include "Application.h"

#include <DxLib.h>
#include <EffekseerForDXLib.h>

#include "Manager/Generic/InputManager.h"
#include "Manager/Generic/ResourceManager.h"
#include "Manager/Generic/SceneManager.h"
#include "Manager/InputTextManager.h"
#include "Manager/Generic/KeyConfInputManager.h"
#include "Manager/Decoration/SoundManager.h"
#include "Manager/System/NetManager.h"
#include "Manager/Decoration/EffectManager.h"
#include "FPS/FpsController.h"



Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_ANIM = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SOUND = "Data/Sound/";
const std::string Application::PATH_CSV = "Data/Csv/";
const std::string Application::PATH_SHADER = "Data/Shader/";


void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("MOMOTARO伝説");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	
#ifdef _DEBUG
	ChangeWindowMode(TRUE);
#else
	ChangeWindowMode(FALSE);
#endif

	// FPS制御初期化
	fpsController_ = new FpsController(FRAME_RATE);

	SetUseASyncLoadFlag(true);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	SetMultiThreadFlag(true);
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	// Effekseerの初期化
	InitEffekseer();

	// 乱数のシード値を設定する
	DATEDATA date;

	// 現在時刻を取得する
	GetDateTime(&date);

	// 乱数の初期値を設定する
	// 設定する数値によって、ランダムの出方が変わる
	SRand(date.Year + date.Mon + date.Day + date.Hour + date.Min + date.Sec);

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	InputTextManager::CreateInstance();
	KeyConfInputManager::CreateInstance();

	// ネットワーク管理初期化
	NetManager::CreateInstance();

	ResourceManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();

	EffectManager::CreateInstance();
	EffectManager::GetInstance().Initialize();

	


}

void Application::Run(void)
{

	InputManager& inputManager = InputManager::GetInstance();
	SceneManager& sceneManager = SceneManager::GetInstance();
	InputTextManager& inputTextManager = InputTextManager::GetInstance();
	NetManager& netManager = NetManager::GetInstance();

	
	// ゲームループ
	while (ProcessMessage() == 0)
	{
		if (sceneManager.IsSceneChanging() && CheckHitKey(KEY_INPUT_ESCAPE) != 0)
		{

		}
		else if (KeyConfInputManager::GetInstance().isTrigerDown("CANCEL") != 0 || isGameEnd_)
		{
			break;
		}

		bool isChanging = SceneManager::GetInstance().IsSceneChanging();

		sceneManager.Update();

		if (!isChanging)
		{
			ClearDrawScreen();
		}

		netManager.Update();
		inputTextManager.Update();
		inputManager.Update();
		KeyConfInputManager::GetInstance().Update();


		sceneManager.Draw();
		// 平均FPS描画
		//fpsController_->Draw();

		// ネットワーク管理更新(フレームの最後)
		//netManager.UpdateEndOfFrame();

		ScreenFlip();
		// 理想FPS経過待ち
		fpsController_->Wait();
	}
	netManager.DestroyInstance();
}

void Application::DestroyInstance(void)
{
	InputManager::GetInstance().DestroyInstance();
	KeyConfInputManager::GetInstance().DestroyInstance();
	ResourceManager::GetInstance().DestroyInstance(); 
	// ネットワーク管理破棄
	NetManager::GetInstance().DestroyInstance();

	// シーン管理解放
	SceneManager::GetInstance().DestroyInstance();

	SetUseASyncLoadFlag(false);

	// Effekseerを終了する。
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	// インスタンスのメモリ解放
	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

void Application::GameEnd(void)
{
	isGameEnd_ = true;
}

bool Application::GetGameEnd(void)
{
	return isGameEnd_;
}

Application::Application(void)
	:
	isInitFail_(false),
	isReleaseFail_(false),
	isGameEnd_(false)
{
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
