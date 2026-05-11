#pragma once
#include "../SceneBase.h"
#include <memory>
#include "../../Object/Player/Player.h"
class Player;
class SceneManager;


/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame_Player : public SceneBase
{
public:

    /// @brief コンストラクタ
    SceneGame_Player(void);

    /// @brief デストラクタ
    ~SceneGame_Player(void) = default;

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

    SceneManager& sceneManager_;
    //std::unique_ptr<Player> player_;
    Player* player_;

    /// @brief デバッグ情報の描画
    void DrawDebug(void);
};