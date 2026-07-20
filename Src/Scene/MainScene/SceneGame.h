#pragma once
#include "../SceneBase.h"
#include <memory>
#include <DxLib.h>
#include <vector>
#include <array>
#include "../../Object/Actor/Chara/Player/Player.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"
#include "../../Object/Actor/Chara/Enemy/EnemyRobo.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../../Object/Common/DamageController.h"
#include "../../Common/GameTimer.h"


/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame : public SceneBase
{
public:
    
    struct PlayerSelectType
    {
        PlayerBase::JOB_TYPE job;
        PlayerBase::SKIN_TYPE skin;
    };

    /// @brief コンストラクタ
    SceneGame(std::vector<PlayerSelectType> _playerSelectType);

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
    static constexpr int ENEMYS_POP = 7;


    const std::vector<VECTOR> ENEMY_POS = {
        {0,0,0},{100,0,100},{200,0,200},{300,0,300},
        {-100,0,-100},{-200,0,-200},{-300,0,-300},

        {100,0,0},{200,0,100},{300,0,200},{400,0,300},
        {-200,0,-100},{-300,0,-200},{-400,0,-300},

        {2,0,0},{300,0,100},{400,0,200},{500,0,300},
        {-300,0,-100},{-400,0,-200},{-500,0,-300},

        {3,0,0},{400,0,100},{500,0,200},{600,0,300},
        {-400,0,-100},{-500,0,-200},{-600,0,-300},
    };



    enum class GAME_STATE
    {
        NONE = -1,
        MOVIE,
        MOVIE_BOSS,
        GAME,
    };
    
    const std::vector<VECTOR> PLAYER_INIT_POS =
    { { 0.0f,0,-3000.0f }
     ,{ 250.0f,0,-3250.0f }
     ,{ -250.0f,0,-3250.0f }
     ,{ 0.0f,0,-3500.0f } };

    std::vector<std::unique_ptr<Player>> players_;

    std::unique_ptr<Boss> boss_;

    std::vector<std::unique_ptr<EnemyRobo>>enemyRobos_;

    std::unique_ptr<Stage> stage_;

    std::unique_ptr<DamageController> damageController_;

    std::unique_ptr<GameTimer> gameTimer_;

    int targetHpImage_;
    int targetHpBerImage_;
    int gameTexts_;

    enum class UI_GAME
    {
        TIMER_LABEL,
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

    /// @brief Gui用の更新処理
    void UpdateGui(void) override;
};