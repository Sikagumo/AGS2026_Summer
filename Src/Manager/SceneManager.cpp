#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/MatchScene.h"
#include "../Scene/ConnectScene.h"
#include "./InputManager.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{

	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	// フェード機能の初期化
	fader_ = new Fader();
	fader_->Init();

	// カメラ
	camera_ = new Camera();
	camera_->Init();

	// 画面遷移中判定
	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(sceneId_);

}

void SceneManager::Init3D(void)
{

	// 背景色設定
	SetBackgroundColor(
		BACKGROUND_COLOR_R, 
		BACKGROUND_COLOR_G, 
		BACKGROUND_COLOR_B);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);
	
	// ライトの設定
	const VECTOR LIGHT_DIR = VGet(0.3f, -0.7f, 0.8f);
	ChangeLightTypeDir(LIGHT_DIR);

	// フォグ設定
	SetFogEnable(true);


	SetFogColor(255,255, 255);
	const float FOG_START = 10000.0f;
	const float FOG_END  = 15000.0f;
	SetFogStartEnd(FOG_START, FOG_END);

	// シャドウマップが想定するライトの方向もセット
	const VECTOR SHADOW_DIR = VNorm(VGet(0.0f, -1.0f, 1.0f));
	SetShadowMapLightDirection(shadowMapHandle_, SHADOW_DIR);

	// シャドウマップに描画する範囲を設定
	const VECTOR SHADOW_AREA_MIN = VGet(-10000.0f, -1.0f, -10000.0f);
	const VECTOR SHADOW_AREA_MAX = VGet(10000.0f, 5000.0f, 10000.0f);
	SetShadowMapDrawArea(shadowMapHandle_, SHADOW_AREA_MIN, SHADOW_AREA_MAX);
}

void SceneManager::Update(void)
{

	if (scene_ == nullptr) { return; }

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// フェード機能の更新
	fader_->Update();
	if (isSceneChanging_)
	{
		// フェード状態の切替処理
		Fade();
	}
	else
	{
		// 各シーンの更新処理
		scene_->Update();
	}

	// カメラ更新
	camera_->Update();

#ifdef _DEBUG
	if (InputManager::GetInstance().KeyIsTrgDown(KEY_INPUT_DELETE))
	{
		//isDebugMode_ = !isDebugMode_;
	}
#endif
}

void SceneManager::Draw(void)
{
	
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	camera_->SetBeforeDraw();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowMapHandle_);

	// 各シーンの描画処理
	scene_->Draw();

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// カメラ描画
	camera_->DrawDebug();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	
	// 暗転・明転
	fader_->Draw();

}

void SceneManager::Destroy(void)
{

	// シーンの解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	// シャドウマップの削除
	DeleteShadowMap(shadowMapHandle_);

	// フェード機能の解放
	delete fader_;

	camera_->Release();
	delete camera_;


	// インスタンスのメモリ解放
	delete instance_;

}

void SceneManager::ChangeScene(SCENE_ID nextId)
{

	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;

}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	return 1.0f / 60.0f;
	//return deltaTime_;
}

float SceneManager::GetTotalGameTime(void)
{
	return totalGameTime_;
}

void SceneManager::SetTotalGameTime(float time)
{
	totalGameTime_ = time;
}

void SceneManager::ForwardGameTime(void)
{
	totalGameTime_ += GetDeltaTime();
}


Camera* SceneManager::GetCamera(void) const
{
	return camera_;
}

SceneManager::SceneManager(void):
	sceneId_(SCENE_ID::NONE),
	waitSceneId_(SCENE_ID::NONE),
	scene_(nullptr), fader_(nullptr),
	camera_(nullptr),
	deltaTime_(1.0f / 60.0f),
	isSceneChanging_(false),
	isDebugMode_(false)
{
	bool isDebug = false;

#ifdef _DEBUG
	//isDebug = true;
#endif

	isDebugMode_ = isDebug;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{

	// リソースの解放
	ResourceManager::GetInstance().Release();

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		delete scene_;
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = new TitleScene();
		break;
	case SCENE_ID::MATCH:
		scene_= new MatchScene();
		break;
	case SCENE_ID::CONNECT:
		scene_ = new ConnectScene();
		break;
	case SCENE_ID::GAME:
		scene_ = new GameScene();
		break;
	
	}

	// 各シーンの初期化
	scene_->Init();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		// 明転中
		if (fader_->IsEnd())
		{
			// 明転が終了したら、フェード処理終了
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd())
		{
			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);
			// 暗転から明転へ
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}

}


