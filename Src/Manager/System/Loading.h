#pragma once

#include <thread>
#include <atomic>
#include <functional>

/// <summary>
/// ローディング画面を制御するクラス（非同期ロード対応）
/// </summary>
class Loading
{
public:

    // インスタンスを生成する
    static void CreateInstance(void);

    // インスタンスを取得する
    static Loading* GetInstance(void);

    // インスタンスを破棄する
    static void  DestroyInstanceInstance(void);

    // 初期化する
    void Initialize(void);

    // 更新する
    void Update(void);

    // 描画する
    void Draw(void);

    // 非同期ロードを開始する
    void StartAsyncLoad(std::function<void()> loadFunc);

    // ロード完了処理を行う
    void EndAsyncLoad(void);

    // ロード中か確認する
    bool IsLoading(void) const;

    // 進捗率を取得する
    int GetProgress(void) const;

    // 進捗率を設定する
    void SetProgress(float progress);

private:


    // シングルトンインスタンス
    static Loading* instance_;

    // 非同期ロード処理スレッド
    std::thread loadingThread_;

    // ロード中フラグ
    std::atomic<bool> isLoading_{ false };

    // 進捗率
    std::atomic<float> progress_{ 0 };

    // コンストラクタ
    Loading(void) = default;

    // デストラクタ
    ~Loading(void);

    // コピーやムーブを禁止する
    Loading(const Loading&) = delete;
    Loading& operator=(const Loading&) = delete;
    Loading(Loading&&) = delete;
    Loading& operator=(Loading&&) = delete;

    // 非同期ロード用のスレッド関数を実行する
    void ThreadFunc(std::function<void()> loadFunc);

};
