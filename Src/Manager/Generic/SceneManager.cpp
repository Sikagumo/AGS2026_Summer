#include "SceneManager.h"
#include "../../Scene/SceneBase.h"
#include "../../Scene/MainScene/SceneTitle.h"
#include "../../Scene/MainScene/SceneGame.h"
#include "../../Scene/MainScene/SceneGameBoss.h"
#include "../Decoration/SoundManager.h"
#include "../../Object/Collision/CollisionController.h"
#include "../../Shader/ShaderController.h"
#include "../System/TimeManager.h"
#include "../System/NetManager.h"
#include "../../Camera/Camera.h"
#include "../../Common/Loading.h"
#include "../../Common/Fader.h"
#include "../../Application.h"
#include "KeyConfInputManager.h"
#include "../../ImGUI/GuiController.h"
#include "../../Scene/MainScene/SceneLobby.h"

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

SceneManager::SceneManager(void)
    : isSceneChanging_(false)
    , isFirstFrame_(true)
    , sceneMutex_()
    , firstFadeImageHandle_(-1)
{
    camera_ = std::make_unique<Camera>();
    fader_ = std::make_unique<Fader>();
    scenes_ = std::list<std::shared_ptr<SceneBase>>();
    oldScene_ = nullptr;
    nextScene_ = nullptr;
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::Initialize(void)
{
    SetMouseDispFlag(true);

    NetManager::CreateInstance();
    SoundManager::CreateInstance();
    SoundManager::GetInstance().Initialize();
    TimeManager::CreateInstance();
    ShaderController::CreateInstance();
    ShaderController::GetInstance().Initialize();
    Loading::CreateInstance();
    Loading::GetInstance()->Initialize();
    CollisionController::CreateInstance();
    CollisionController::GetInstance().Initialize();
    GuiController::CreateInstance();
}

void SceneManager::Init3D(void)
{
    constexpr COLOR_F BACK_COLOR = { 0.0f, 0.0f, 0.0f }; // 背景色
    const float AMBIENT_VAL = 0.8f;                      // 環境光の強さ
    const float LIGHT_MID_VAL = 0.5f;                    // 鏡面光・環境光のベース値
    const int FOG_COL = 5;                               // フォグの色
    const float FOG_START = 10000.0f;                    // フォグ開始距離
    const float FOG_END = 20000.0f;                      // フォグ終了距離

    // 背景色を設定する
    SetBackgroundColor(static_cast<int>(BACK_COLOR.r)
        , static_cast<int>(BACK_COLOR.g)
        , static_cast<int>(BACK_COLOR.b));

    // Zバッファを有効にする
    SetUseZBuffer3D(true);

    // Zバッファへの書き込みを有効にする
    SetWriteZBuffer3D(true);

    // バックカリングを有効にする
    SetUseBackCulling(true);

    // ライティングを有効にする
    SetUseLighting(true);
    SetLightEnable(true);

    SetGlobalAmbientLight(GetColorF(AMBIENT_VAL, AMBIENT_VAL, AMBIENT_VAL, 1.0f));

    ChangeLightTypeDir(VGet(0.0f, -1.0f, 1.0f));                                     // ライトの方向
    SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));                             // 拡散光
    SetLightSpcColor(GetColorF(LIGHT_MID_VAL, LIGHT_MID_VAL, LIGHT_MID_VAL, 1.0f));  // 鏡面光
    SetLightAmbColor(GetColorF(LIGHT_MID_VAL, LIGHT_MID_VAL, LIGHT_MID_VAL, 1.0f));  // 環境光

    // フォグを設定する
    SetFogEnable(true);
    SetFogColor(FOG_COL, FOG_COL, FOG_COL);
    SetFogStartEnd(FOG_START, FOG_END);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> _scene)
{
    // CollisionControllerをクリア
    CollisionController::GetInstance().Clear();

    if (camera_)
    {
        CollisionController::GetInstance().RegisterActor(camera_.get());
    }

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    // 新しいシーンを設定
    nextScene_ = _scene;
    isSceneChanging_ = true;

    // フェードアウトを開始
    fader_->SetFade(Fader::STATE::FADE_OUT);

    // 非同期ロード開始
    Loading::GetInstance()->StartAsyncLoad([_scene]()
        {
            _scene->Load();
        });
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> _scene)
{
    scenes_.push_back(_scene);

    // 即時ロード・初期化
    _scene->Load();
    _scene->EndLoad();
    _scene->Initialize();
}

void SceneManager::PopScene(void)
{
    if (scenes_.size() > 1)
    {
        scenes_.back()->Release();
        scenes_.pop_back();
    }
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> _scene)
{
    scenes_.clear();

    // CollisionControllerをクリア
    CollisionController::GetInstance().Clear();

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    isSceneChanging_ = true;
    scenes_.push_back(_scene);

    // 非同期ロードを開始する
    Loading::GetInstance()->StartAsyncLoad([_scene]()
        {
            _scene->Load();
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

        fader_->LoadFadeImage();

		//auto jobs = { SceneGame::PlayerSelectType(PlayerBase::JOB_TYPE::CANNON, PlayerBase::SKIN_TYPE::DOG) };
        //ChangeScene(std::make_shared<SceneGame>(jobs));
        ChangeScene(std::make_shared<SceneTitle>());
    }

    TimeManager::GetInstance().Update();
    NetManager::GetInstance().Update();

    if (NetManager::GetInstance().GetMode() != NET_MODE::NONE
        && NetManager::GetInstance().GetIsConnectionLost())
    {
        NetManager::GetInstance().Stop();
        ChangeScene(std::make_shared<SceneTitle>());
        return;
    }

    if (Application::GetInstance().GetGameEnd())
    {
        return;
    }

    // ロード完了とみなす進捗のしきい値
    constexpr float LOAD_COMPLETE_THRESHOLD = 100.0f; 

    // ロード中の処理を完全に分離する
    if (isSceneChanging_)
    {
        fader_->Update();

        auto loader = Loading::GetInstance();
        loader->Update();

        // ロードが完了しており、かつフェードアウトが完了しているか
        const bool isLoadFinished = (loader->GetProgress() >= LOAD_COMPLETE_THRESHOLD && !loader->IsLoading());
        const bool isFadeOutFinished = (fader_->GetState() == Fader::STATE::FADE_OUT && fader_->IsEnd());

        if (isLoadFinished && isFadeOutFinished && nextScene_ != nullptr)
        {
            // 新しいシーンを確実に登録
            nextScene_->EndLoad();
            nextScene_->Initialize();
            scenes_.push_back(nextScene_);

            for (auto& scene : scenes_)
            {
                if (scene != nextScene_)
                {
                    oldScene_ = scene;
                }
            }
            scenes_.remove_if([this](const auto& s) { return s != nextScene_; });

            nextScene_ = nullptr;

            fader_->SetFade(Fader::STATE::FADE_IN);
        }

        // フェードインが完了したら、シーン切り替え状態を終了する
        if (nextScene_ == nullptr && fader_->GetState() == Fader::STATE::FADE_IN && fader_->IsEnd())
        {
            fader_->SetFade(Fader::STATE::NONE);
            isSceneChanging_ = false;
        }

        return;
    }

    // 次のフレームの先頭で、前のフレームの描画が終わった後に解放する
    if (oldScene_)
    {
        oldScene_->Release();
        oldScene_ = nullptr;
    }

    if (scenes_.empty())
    {
        return;
    }

    auto& currentScene = scenes_.back();
    if (currentScene)
    {
        currentScene->Update();
    }

    CollisionController::GetInstance().Update();

    if (camera_)
    {
        camera_->ResolveCollision();
    }
}

void SceneManager::Draw(void)
{
    ClearDrawScreen();

    if (!scenes_.empty())
    {
        if (camera_)
        {
            camera_->SetBeforeDraw();
        }
        for (auto& scene : scenes_)
        {
            if (scene)
            {
                scene->Draw();
            }
        }
    }

#ifdef _DEBUG
    for (auto& scene : scenes_)
    {
        if (scene)
        {
            if (scene->GetDebugMode() == true)
            {
                GuiController::GetInstance().DrawUI();
            }
        }
    }
#endif 

    // フェードを先に描画する
    fader_->Draw();

    // フェードアウトが終わってロード画面を上に重ねる
    auto loader = Loading::GetInstance();

    if (isSceneChanging_)
    {
        if (fader_->GetState() == Fader::STATE::FADE_OUT && fader_->IsEnd())
        {
            if (loader)
            {
                loader->Draw();
            }
        }
    }
}

void SceneManager::Release(void)
{
    // ロード完了を待機する
    if (Loading::GetInstance()->IsLoading())
    {
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
    ShaderController::GetInstance().DestroyInstance();
    TimeManager::GetInstance().DestroyInstance();
    Loading::GetInstance()->DestroyInstance();
    CollisionController::GetInstance().DestroyInstance();
    GuiController::DestroyInstance();
    NetManager::GetInstance().DestroyInstance();
}

const std::unique_ptr<Camera>& SceneManager::GetCamera(void) const
{
    return camera_;
}