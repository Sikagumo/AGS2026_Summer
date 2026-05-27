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

SceneManager::SceneManager(void)
{
    isGameEnd_ = false;
    isSceneChanging_ = false;

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
    // ”wŒiF
    constexpr COLOR_F BACK_COLOR = { 125,125,125 };

    // ŠÂ‹«Œõ‚Ì‹­‚³
    const float AmbientVal = 0.8f;

    // ‹¾–ÊŒõEŠÂ‹«Œõ‚Ìƒx[ƒX’l
    const float LightMidVal = 0.5f;

    // ƒtƒHƒO‚ÌF
    const int FogCol = 5;

    // ƒtƒHƒOŠJn‹——£
    const float FogStart = 10000.0f;

    // ƒtƒHƒOI—¹‹——£
    const float FogEnd = 20000.0f;

    // ”wŒiF‚ğİ’è‚·‚é
    SetBackgroundColor(BACK_COLOR.r, BACK_COLOR.g,BACK_COLOR.b);

    // Zƒoƒbƒtƒ@‚ğ—LŒø‚É‚·‚é
    SetUseZBuffer3D(true);

    // Zƒoƒbƒtƒ@‚Ö‚Ì‘‚«‚İ‚ğ—LŒø‚É‚·‚é
    SetWriteZBuffer3D(true);

    // ƒoƒbƒNƒJƒŠƒ“ƒO‚ğ—LŒø‚É‚·‚é
    SetUseBackCulling(true);

    // ƒ‰ƒCƒeƒBƒ“ƒO‚ğ—LŒø‚É‚·‚é
    SetUseLighting(true);
    SetLightEnable(true);

    SetGlobalAmbientLight(GetColorF(AmbientVal, AmbientVal, AmbientVal, 1.0f));

    ChangeLightTypeDir(VGet(0.0f, -1.0f, 1.0f));                               // ƒ‰ƒCƒg‚Ì•ûŒü
    SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));                       // ŠgUŒõ
    SetLightSpcColor(GetColorF(LightMidVal, LightMidVal, LightMidVal, 1.0f));  // ‹¾–ÊŒõ
    SetLightAmbColor(GetColorF(LightMidVal, LightMidVal, LightMidVal, 1.0f));  // ŠÂ‹«Œõ

    // ƒtƒHƒO‚ğİ’è‚·‚é
    SetFogEnable(true);
    SetFogColor(FogCol, FogCol, FogCol);
    SetFogStartEnd(FogStart, FogEnd);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
    // ŒÃ‚¢ƒV[ƒ“‚ğ‰ğ•ú
    for (auto& s : scenes_)
    {
        s->Release();
    }
    scenes_.clear();
   

    // CollisionController‚ğƒNƒŠƒA
    //CollisionController::GetInstance().Clear();

    // BGM‚ğ’â~‚·‚é
    SoundManager::GetInstance().StopAllBGM();

    // V‚µ‚¢ƒV[ƒ“‚ğİ’è
    scenes_.push_back(scene);
    isSceneChanging_ = true;

    // ”ñ“¯Šúƒ[ƒhŠJniƒ[ƒh‰æ–Ê•t‚«j
    Loading::GetInstance()->StartAsyncLoad([scene]()
        {
        scene->Load();
        });
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
    scenes_.push_back(scene);

    // ‘¦ƒ[ƒhE‰Šú‰»
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

    // CollisionController‚ğƒNƒŠƒA
    CollisionManager::GetInstance().Clear();

    // BGM‚ğ’â~‚·‚é
    SoundManager::GetInstance().StopAllBGM();

    isSceneChanging_ = true;
    scenes_.push_back(scene);

    // ”ñ“¯Šúƒ[ƒh‚ğŠJn‚·‚é
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
            // 3D•`‰æİ’è‚ğ‰Šú‰»‚·‚é
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

    // ƒ[ƒh’†‚Ìˆ—‚ğŠ®‘S‚É•ª—£‚·‚é
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

    // ’Êí‚Ì•`‰æ
    if (camera_) camera_->SetBeforeDraw();

    for (auto& scene : scenes_)
    {
        if (scene) scene->Draw();
    }

    if (camera_) camera_->DrawDebug();
}

void SceneManager::Release(void)
{
<<<<<<< HEAD
    if (Loading::GetInstance())
=======
    // ƒ[ƒhŠ®—¹‚ğ‘Ò‹@‚·‚é
    if (Loading::GetInstance()->IsLoading())
>>>>>>> main
    {
        // éåŒæœŸãƒ­ãƒ¼ãƒ‰ãã®ã‚‚ã®ã‚’ã‚­ãƒ£ãƒ³ã‚»ãƒ«ã•ã›ã‚‹
        SetUseASyncLoadFlag(false);
        // Loadingã®çŠ¶æ…‹ã‚’å¼·åˆ¶çš„ã«çµ‚äº†ã•ã›ã‚‹
        Loading::GetInstance()->EndAsyncLoad();
    }

    // ŠeƒV[ƒ“‚ğ‰ğ•ú‚·‚é
    for (auto& scene : scenes_)
    {
        scene->Release();
    }
    scenes_.clear();

    // ƒJƒƒ‰‚ğ‰ğ•ú‚·‚é
    camera_.reset();

    // Šeƒ}ƒl[ƒWƒƒ[‚ğ”jŠü‚·‚é
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