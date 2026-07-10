#pragma once
#include "../SceneBase.h"
#include <memory>
#include <DxLib.h>
#include <vector>
#include <array>
#include "../../Object/Actor/Chara/Player/Player.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../../Object/Common/DamageController.h"
#include "../../Common/GameTimer.h"


/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame : public SceneBase
{
public:

    /// @brief コンストラクタ
    SceneGame(std::vector<PlayerBase::JOB_TYPE> _playerJob);

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

    const std::vector<VECTOR> PLAYER_INIT_POS =
    { { 0,0,-1000.0f },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };

    std::vector<std::unique_ptr<Player>> players_;

    std::unique_ptr<Boss> boss_;

    std::unique_ptr<Stage> stage_;

    std::unique_ptr<DamageController> damageController_;

    std::unique_ptr<GameTimer> gameTimer_;

    int targetHpImage_;
    int targetHpBerImage_;


    enum class UI_GAME
    {

        MAX
    };
    std::array<int, static_cast<int>(UI_GAME::MAX)> uiGame_;

    int playerHpImage_;
    int playerHpImageBack_;


    float CalcHpBarScale(const VECTOR& _targetPos);


    /// @brief デバッグ情報の描画
    void DrawDebug(void);

    void DrawHpBerPlayer(void);
    void DrawHpBerBoss(void);

    void DamageProcess(void);

    void UpdateGameTime(void);
};