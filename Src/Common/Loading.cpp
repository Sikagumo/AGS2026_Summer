#include "Loading.h"

#include <DxLib.h>
#include <iostream>

#include "../Application.h"
#include "../Manager/Generic/ResourceManager.h"

Loading* Loading::instance_ = nullptr;

void Loading::CreateInstance(void)
{
    if (!instance_)
    {
        instance_ = new Loading();
    }
}

Loading* Loading::GetInstance(void)
{
    return instance_;
}

void Loading::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

Loading::Loading(void)
    : isLoading_(false)
    , progress_(0.0f)
    , imageHandle_(-1)
{
}

Loading::~Loading(void)
{
    if (loadingThread_.joinable())
    {
        loadingThread_.join();
    }
}

void Loading::Initialize(void)
{
    isLoading_ = false;
    progress_ = 0.0f;
    imageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_PEACH);
}

void Loading::StartAsyncLoad(std::function<void()> loadFunc)
{
    if (isLoading_)
    {
        return;
    }

    if (loadingThread_.joinable())
    {
        loadingThread_.join();
    }

    Initialize();
    isLoading_ = true;

    loadingThread_ = std::thread(&Loading::ThreadFunc, this, loadFunc);
}

void Loading::ThreadFunc(std::function<void()> loadFunc)
{
    try
    {
        progress_ = 0.0f;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (loadFunc)
        {
            SetUseASyncLoadFlag(TRUE);

            loadFunc();

            SetUseASyncLoadFlag(FALSE);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        progress_ = 100.0f;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ロード中に例外が発生しました: " << exception.what() << std::endl;
        progress_ = 100.0f;
    }
    catch (...)
    {
        std::cerr << "ロード中に不明な例外が発生しました。" << std::endl;
        progress_ = 100.0f;
    }

    EndAsyncLoad();
}

void Loading::Update(void)
{
    if (!isLoading_ && progress_ >= 100.0f && loadingThread_.joinable())
    {
        loadingThread_.join();
    }
}

void Loading::Draw(void)
{
    const int screenWidth = Application::SCREEN_SIZE_X; // 画面の横幅
    const int screenHeight = Application::SCREEN_SIZE_Y; // 画面の縦幅

    ClearDrawScreen();
    // 背景を黒で塗りつぶす
    DrawBox(0, 0, screenWidth, screenHeight, GetColor(0, 0, 0), TRUE);

    // 画面内における画像移動の基準となる横幅（旧バーの幅相当）
    const int movementRangeWidth = 800; 
    const int centerX = screenWidth / 2; 
    const int centerY = screenHeight / 2; 

    // 現在の進捗率を取得（0.0f 〜 100.0f）
    const float currentProgress = progress_.load(std::memory_order_acquire);

    // 画像のサイズを取得
    int imageWidth = 0; // 画像の横幅
    int imageHeight = 0; // 画像の縦幅
    GetGraphSize(imageHandle_, &imageWidth, &imageHeight);

    // 画像が画面中央を基準に左右へ綺麗に移動するための開始地点と終了地点を計算
    const int startX = centerX - movementRangeWidth / 2 - imageWidth / 2; 
    const int endX = centerX + movementRangeWidth / 2 - imageWidth / 2; 

    // 現在の進捗率を基に、開始地点から終了地点までのX座標を線形補間
    const int imageX = static_cast<int>(startX + (endX - startX) * (currentProgress / 100.0f)); 
    const int imageY = centerY - imageHeight / 2; 

    DrawRotaGraph(imageX, imageY, 0.5f, 0.0f,imageHandle_, true);
}

void Loading::EndAsyncLoad(void)
{
    isLoading_.store(false, std::memory_order_release);
    progress_.store(100.0f, std::memory_order_release);
}

bool Loading::IsLoading(void) const
{
    return isLoading_.load(std::memory_order_acquire);
}

int Loading::GetProgress(void) const
{
    return static_cast<int>(progress_.load(std::memory_order_acquire));
}

void Loading::SetProgress(float progress)
{
    if (progress < 0.0f)
    {
        progress = 0.0f;
    }
    if (progress > 100.0f)
    {
        progress = 100.0f;
    }
    progress_.store(progress, std::memory_order_release);
}