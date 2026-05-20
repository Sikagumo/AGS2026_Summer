#pragma once
#include "../SceneBase.h"
#include <memory>
#include "../../Object/Actor/Chara/Player/Player.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"
#include "../../Object/Actor/Stage/Stage.h"


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

    /// @brief 衝突の影響だけを更新する処理 
    void UpdateCollision(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

private:

    SceneManager& sceneManager_;
    std::unique_ptr<Player> player_;

    std::unique_ptr<Boss> boss_;

    std::unique_ptr<Stage> stage_;

    VECTOR tempBossWeaponPos_;

    /// @brief デバッグ情報の描画
    void DrawDebug(void);
};