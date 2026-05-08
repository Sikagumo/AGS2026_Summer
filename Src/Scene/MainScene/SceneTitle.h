#pragma once

#include "../SceneBase.h"

class SceneTitle : public SceneBase
{
public:

    /// @brief コンストラクタ
    SceneTitle(void);

    /// @brief デストラクタ
    ~SceneTitle(void) override = default;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief ゲーム終了が要求されているか取得
   /// @return 終了するならtrue
    bool IsExitRequested(void) const;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;


private:

    // アプリケーション終了フラグ
    bool exitRequested_;                

    void DrawDebug(void);

};
