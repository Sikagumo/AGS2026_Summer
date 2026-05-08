#include "Loading.h"

#include <DxLib.h>
#include <iostream>

#include "../../Application.h"

// インスタンスを初期化する
Loading* Loading::instance_ = nullptr;

// インスタンスを生成する
void Loading::CreateInstance(void)
{
    if (!instance_)
    {
        instance_ = new Loading();
        instance_->Initialize();
    }
}

// インスタンスを取得する
Loading* Loading::GetInstance(void)
{
    return instance_;
}

// インスタンスを破棄する
void Loading::DestroyInstanceInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

// デストラクタ
Loading::~Loading(void)
{
    // スレッドが実行中なら待機する
    if (loadingThread_.joinable())
    {
        loadingThread_.join();
    }
}

// 初期化する
void Loading::Initialize(void)
{
    isLoading_ = false;
    progress_ = 0.0f;
}

// 非同期ロードを開始する
void Loading::StartAsyncLoad(std::function<void()> loadFunc)
{
    // 既にロード中なら無視する
    if (isLoading_) return;

    // 前のスレッドが残っていれば待機する
    if (loadingThread_.joinable())
    {
        loadingThread_.join();
    }

    // 初期化してロード開始フラグを立てる
    Initialize();
    isLoading_ = true;

    // スレッドを開始する（detachしない）
    loadingThread_ = std::thread(&Loading::ThreadFunc, this, loadFunc);
}

// 非同期ロード処理を行う
void Loading::ThreadFunc(std::function<void()> loadFunc)
{
    try
    {
        progress_ = 0.0f;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 実際のロード処理を行う
        if (loadFunc)
        {
            loadFunc();
        }

        // 最後の描画を待つ
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // ロード完了
        progress_ = 100.0f;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ロード中に例外が発生しました: " << e.what() << std::endl;
        progress_ = 100.0f; // エラーでも終了扱いにする
    }
    catch (...)
    {
        std::cerr << "ロード中に不明な例外が発生しました。" << std::endl;
        progress_ = 100.0f;
    }

    // ロード完了処理を行う
    EndAsyncLoad();
}

// 更新する
void Loading::Update(void)
{
    // ロード完了チェック
    if (!isLoading_ && progress_ >= 100.0f && loadingThread_.joinable())
    {
        loadingThread_.join();
    }
}

// 描画する
void Loading::Draw(void)
{
    const int screenW = Application::SCREEN_SIZE_X;
    const int screenH = Application::SCREEN_SIZE_Y;

    ClearDrawScreen();
    DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);

    const int barW = 800;
    const int barH = 60;
    const int centerX = screenW / 2;
    const int centerY = screenH / 2;

    int prevFontSize = GetFontSize(); // 現在のサイズを保存
    SetFontSize(40);                 // 文字を大きく設定

    // 進捗率を取得
    float currentProgress = progress_.load(std::memory_order_acquire);
    int progressWidth = static_cast<int>(barW * currentProgress / 100.0f);

    if (progressWidth > 0)
    {
        DrawBox(centerX - barW / 2, centerY - barH / 2,
            centerX - barW / 2 + progressWidth, centerY + barH / 2,
            GetColor(0, 255, 0), TRUE);
    }

    DrawFormatString(centerX - 100, centerY - 15,
        GetColor(255, 255, 255),
        "Loading... %d%%", static_cast<int>(currentProgress));

    SetFontSize(prevFontSize);

    DrawBox(centerX - barW / 2, centerY - barH / 2,
        centerX + barW / 2, centerY + barH / 2,
        GetColor(255, 255, 255), FALSE);


}

// ロード完了処理を行う
void Loading::EndAsyncLoad(void)
{
    isLoading_.store(false, std::memory_order_release);
    progress_.store(100.0f, std::memory_order_release);
}

// ロード中か確認する
bool Loading::IsLoading(void) const
{
    return isLoading_.load(std::memory_order_acquire);
}

// 進捗率を取得する
int Loading::GetProgress(void) const
{
    return static_cast<int>(progress_.load(std::memory_order_acquire));
}

// 進捗率を設定する
void Loading::SetProgress(float progress)
{
    if (progress < 0.0f) { progress = 0.0f; }
    if (progress > 100.0f) { progress = 100.0f; }
    progress_.store(progress, std::memory_order_release);
}