#pragma once
#include "../SceneBase.h"

/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame : public SceneBase
{
public:

    /// @brief コンストラクタ
    SceneGame(void);

    /// @brief デストラクタ
    ~SceneGame(void) = default;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

private:

    /// @brief デバッグ情報の描画
    void DrawDebug(void);
};