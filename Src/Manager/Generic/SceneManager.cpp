#include "SceneManager.h"
#include "../../Scene/SceneBase.h"
#include "../../Scene/MainScene/SceneTitle.h"
#include "../../Scene/MainScene/SceneGame.h"
#include "../../Scene/MainScene/SceneGamePlayer.h"
#include "../../Scene/MainScene/SceneGameBoss.h"
#include "../Decoration/SoundManager.h"
#include "../../Object/Manager/CollisionManager.h"
#include "../System/TimeManager.h"
#include "../../Camera/Camera.h"
#include "../../Common/Loading.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new SceneManager();
    }
    instance_->Initialize();
}

SceneManager& SceneManager::GetInstance(void)
{
    return *instance_;
}

void SceneManager::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

SceneManager::SceneManager(void):
    isGameEnd_(false),
    isSceneChanging_(false),
    isFirstFrame_(true)
{

    camera_ = std::make_unique<Camera>();
}

SceneManager::~SceneManager(void)
{
    Release();
}

void SceneManager::Initialize(void)
{
    SoundManager::CreateInstance();
    SoundManager::GetInstance().Initialize();
    TimeManager::CreateInstance();
    Loading::CreateInstance();
    Loading::GetInstance()->Initialize();
    CollisionManager::CreateInstance();
    CollisionManager::GetInstance().Initialize();

    isFirstFrame_ = true;
}

void SceneManager::Init3D(void)
{
    // 背景色
    constexpr COLOR_F BACK_COLOR = { 125,125,125 };

    // 環境光の強さ
    const float AmbientVal = 0.8f;

    // 鏡面光・環境光のベース値
    const float LightMidVal = 0.5f;

    // フォグの色
    const int FogCol = 5;

    // フォグ開始距離
    const float FogStart = 10000.0f;

    // フォグ終了距離
    const float FogEnd = 20000.0f;

    // 背景色を設定する
    SetBackgroundColor(BACK_COLOR.r, BACK_COLOR.g,BACK_COLOR.b);

    // Zバッファを有効にする
    SetUseZBuffer3D(true);

    // Zバッファへの書き込みを有効にする
    SetWriteZBuffer3D(true);

    // バックカリングを有効にする
    SetUseBackCulling(true);

    // ライティングを有効にする
    SetUseLighting(true);
    SetLightEnable(true);

    SetGlobalAmbientLight(GetColorF(AmbientVal, AmbientVal, AmbientVal, 1.0f));

    ChangeLightTypeDir(VGet(0.0f, -1.0f, 1.0f));                               // ライトの方向
    SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));                       // 拡散光
    SetLightSpcColor(GetColorF(LightMidVal, LightMidVal, LightMidVal, 1.0f));  // 鏡面光
    SetLightAmbColor(GetColorF(LightMidVal, LightMidVal, LightMidVal, 1.0f));  // 環境光

    // フォグを設定する
    SetFogEnable(true);
    SetFogColor(FogCol, FogCol, FogCol);
    SetFogStartEnd(FogStart, FogEnd);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
    // 古いシーンを解放
    for (auto& s : scenes_)
    {
        s->Release();
    }
    scenes_.clear();
   

    // CollisionControllerをクリア
    //CollisionController::GetInstance().Clear();

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    // 新しいシーンを設定
    scenes_.push_back(scene);
    isSceneChanging_ = true;

    // 非同期ロード開始（ロード画面付き）
    Loading::GetInstance()->StartAsyncLoad([scene]()
        {
        scene->Load();
        });
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
    scenes_.push_back(scene);

    // 即時ロード・初期化
    scene->Load();
    scene->EndLoad();
    scene->Initialize();
}

void SceneManager::PopScene(void)
{
    if (scenes_.size() > 1)
    {
        scenes_.back()->Release();
        scenes_.pop_back();

        //if (!scenes_.empty())
        //{
        //    scenes_.back()->OnResume();
        //}
    }
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
    scenes_.clear();

    // CollisionControllerをクリア
    CollisionManager::GetInstance().Clear();

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    isSceneChanging_ = true;
    scenes_.push_back(scene);

    // 非同期ロードを開始する
    Loading::GetInstance()->StartAsyncLoad([scene]()
        {
        scene->Load();
        });
}

void SceneManager::Update(void)
{
    if (isFirstFrame_)
    {
        isFirstFrame_ = false; 

        if (camera_)
        {
            // 3D描画設定を初期化する
            Init3D();

            camera_->Init();
        }

        ChangeScene(std::make_shared<SceneTitle>());

        return;
    }

    if (scenes_.empty()) { return; }

    TimeManager::GetInstance().Update();

    if (isGameEnd_) { return; }

    const float LoadCompleteThreshold = 100.0f;

    // ロード中の処理を完全に分離する
    if (isSceneChanging_)
    {
        auto loader = Loading::GetInstance();
        loader->Update();

        if (loader->GetProgress() >= LoadCompleteThreshold && !loader->IsLoading())
        {
            auto current = scenes_.back();
            current->EndLoad();
            current->Initialize();
            isSceneChanging_ = false;
        }
        return;
    }

    auto current = scenes_.back();
    if (current)
    {
        current->Update();
    }

    CollisionManager::GetInstance().Update();

    if (current)
    {
        current->UpdateCollision();
    }

    
    if (camera_) camera_->Update();

}

void SceneManager::Draw(void)
{
    if (scenes_.empty()) { return; }

    auto loader = Loading::GetInstance();

    if (isSceneChanging_ || (loader && loader->IsLoading()))
    {
        if (loader)
        {
            loader->Draw(); 
        }
        return;
    }   

    if (camera_ == nullptr)
    {
        return;
    }

    // 通常時の描画
    if (camera_) camera_->SetBeforeDraw();

    for (auto& scene : scenes_)
    {
        if (scene) scene->Draw();
    }

    if (camera_) camera_->DrawDebug();
}

void SceneManager::Release(void)
{
    if (Loading::GetInstance())

    // ロード完了を待機する
    if (Loading::GetInstance()->IsLoading())
    {
        SetUseASyncLoadFlag(false);

        Loading::GetInstance()->EndAsyncLoad();
    }

    // 各シーンを解放する
    for (auto& scene : scenes_)
    {
        scene->Release();
    }
    scenes_.clear();

    // カメラを解放する
    camera_.reset();

    // 各マネージャーを破棄する
    SoundManager::GetInstance().DestroyInstance();
    TimeManager::GetInstance().DestroyInstance();
    Loading::GetInstance()->DestroyInstance();
    CollisionManager::DestroyInstance();
}

void SceneManager::GameEnd(void)
{
    isGameEnd_ = true;
}

bool SceneManager::GetGameEnd(void) const
{
    return isGameEnd_;
}

const std::unique_ptr<Camera>& SceneManager::GetCamera(void) const
{
    return camera_;
}