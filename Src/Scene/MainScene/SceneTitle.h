#pragma once

#include <array>

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

    /// @brief 衝突の影響だけを更新する処理 
    void UpdateCollision(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;


private:

    int imageTitle_;

    std::array<int, 3> imageMenu_;

    void DrawDebug(void);

};
