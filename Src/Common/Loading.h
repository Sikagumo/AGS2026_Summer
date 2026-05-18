#pragma once

#include <thread>
#include <atomic>
#include <functional>

/// @brief ローディング画面を制御するクラス（非同期ロード対応）
class Loading
{
public:

    /// @brief インスタンスを生成する
    /// @param void 
    /// @return void
    static void CreateInstance(void);

    /// @brief インスタンスを取得する
    /// @param void 
    /// @return Loading* インスタンスのポインタ
    static Loading* GetInstance(void);

    /// @brief インスタンスを破棄する
    /// @param void 
    /// @return void
    static void DestroyInstance(void);

    /// @brief 初期化する
    /// @param void 
    /// @return void
    void Initialize(void);

    /// @brief 更新する
    /// @param void 
    /// @return void
    void Update(void);

    /// @brief 描画する
    /// @param imageHandle 移動させる画像のグラフィックハンドル
    /// @return void
    void Draw(void);

    /// @brief 非同期ロードを開始する
    /// @param loadFunc ロード中に実行する関数オブジェクト
    /// @return void
    void StartAsyncLoad(std::function<void()> loadFunc);

    /// @brief ロード完了処理を行う
    /// @param void 
    /// @return void
    void EndAsyncLoad(void);

    /// @brief ロード中か確認する
    /// @param void 
    /// @return bool ロード中ならtrue
    bool IsLoading(void) const;

    /// @brief 進捗率を取得する
    /// @param void 
    /// @return int 進捗率（0〜100）
    int GetProgress(void) const;

    /// @brief 進捗率を設定する
    /// @param progress 設定する進捗率（0.0f〜100.0f）
    /// @return void
    void SetProgress(float progress);

private:

    // シングルトンインスタンス
    static Loading* instance_; 

    // 非同期ロード処理スレッド
    std::thread loadingThread_; 

    // ロード中フラグ
    std::atomic<bool> isLoading_; 

    // 進捗率
    std::atomic<float> progress_; 

    // 画像ハンドル
    int imageHandle_;

    /// @brief コンストラクタ
    Loading(void);

    /// @brief デストラクタ
    ~Loading(void);

    Loading(const Loading&) = delete;
    Loading& operator=(const Loading&) = delete;
    Loading(Loading&&) = delete;
    Loading& operator=(Loading&&) = delete;

    /// @brief 非同期ロード用のスレッド関数を実行する
    /// @param loadFunc ロード中に実行する関数オブジェクト
    /// @return void
    void ThreadFunc(std::function<void()> loadFunc);

};