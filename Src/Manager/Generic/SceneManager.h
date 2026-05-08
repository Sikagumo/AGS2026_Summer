#pragma once
#include <list>
#include <memory>
#include <mutex>
#include <chrono>

class SceneBase;
class Camera;

/// @brief シーンのスタック管理、遷移を管理するクラス（シングルトン）
class SceneManager
{
public:

    /// @brief インスタンスを明示的に生成する
    static void CreateInstance(void);

    /// @brief インスタンスを取得する
    /// @return SceneManagerの参照
    static SceneManager& GetInstance(void);

    /// @brief インスタンスを破棄する
    static void  DestroyInstance(void);

    /// @brief システムおよび3D描画設定を初期化する
    void Initialize(void);

    /// @brief 現在のシーンおよびデルタタイムの更新
    void Update(void);

    /// @brief シーンの描画処理を呼び出す
    void Draw(void);

    /// @brief 全シーンの解放処理
    void Release(void);

    /// @brief シーンを変更する（現在のスタックを全削除して新しいシーンを追加）
    /// @param scene 次に実行するシーンのポインタ
    void ChangeScene(std::shared_ptr<SceneBase> scene);

    /// @brief シーンを積む（現在のシーンを残したまま上に重ねる）
    /// @param scene 重ねるシーンのポインタ
    void PushScene(std::shared_ptr<SceneBase> scene);

    /// @brief シーンを外す（スタックの最上位シーンを削除する）
    void PopScene(void);

    /// @brief シーンをジャンプする（全削除→新規ロード、ChangeSceneと同様の動作）
    /// @param scene 遷移先シーンのポインタ
    void JumpScene(std::shared_ptr<SceneBase> scene);

    /// @brief ゲーム終了フラグの状態を取得する
    /// @return 終了する場合はtrue
    bool GetGameEnd(void) const;

    /// @brief 前フレームからの経過時間を取得する
    /// @return デルタタイム（秒）
    float GetDeltaTime(void) const;

    /// @brief 現在管理しているカメラを取得する
    /// @return カメラオブジェクトの共有ポインタ
    std::shared_ptr<Camera> GetCamera(void) const;

    /// @brief ゲーム終了フラグを立てる
    void GameEnd(void);

private:
    // 唯一のインスタンス
    static SceneManager* instance_;

    // シーン管理関連
    std::list<std::shared_ptr<SceneBase>> scenes_; // シーンを保持する（スタック構造）
    std::mutex sceneMutex_;                        // シーンアクセスを保護するミューテックス
    bool isGameEnd_;                               // ゲーム終了フラグ
    bool isSceneChanging_;                         // シーン切り替え中フラグ

    // 時間・計測関連
    std::chrono::system_clock::time_point preTime_; // 前フレームの時刻
    float deltaTime_;                               // デルタタイム

    // カメラ
    std::shared_ptr<Camera> camera_;

    /// @brief 3D描画に関する初期設定
    void Init3D(void);

    /// @brief デルタタイム計算用の時刻をリセットする
    void ResetDeltaTime(void);

    /// @brief コンストラクタ
    SceneManager(void);

    /// @brief デストラクタ
    ~SceneManager(void);

    /// @brief コピーコンストラクタを禁止する
    SceneManager(const SceneManager&) = delete;

    /// @brief 代入演算子を禁止する
    SceneManager& operator=(const SceneManager&) = delete;

    /// @brief ムーブコンストラクタを禁止する
    SceneManager(SceneManager&&) = delete;

    /// @brief ムーブ代入演算子を禁止する
    SceneManager& operator=(SceneManager&&) = delete;
};
