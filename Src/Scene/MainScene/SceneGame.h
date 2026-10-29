#pragma once

#include <vector>
#include <array>
#include <functional>
#include <memory>
#include <map>
#include <DxLib.h>

#include "../SceneBase.h"
#include "../../Object/Actor/Chara/Player/Player.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Actor/Chara/Player/PlayerWeaponController.h"
#include "../../Object/Actor/Chara/Boss/Boss.h"
#include "../../Object/Actor/Chara/Enemy/EnemyRobo.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../../Object/Common/DamageController.h"
#include "../../Common/GameTimer.h"
#include "../../Shader/ShaderParameters.h"

/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame : public SceneBase
{
public:

	/// @brief プレイヤー選択情報の構造体
	struct PlayerSelectType
	{
		PlayerBase::JOB_TYPE job;   // 職業
		PlayerBase::SKIN_TYPE skin; // スキンタイプ
	};

	/// @brief コンストラクタ
	/// @param _playerSelectType プレイヤーの選択情報リスト
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

	// 定数関連
	static constexpr int ENEMYS_POP = 20;               // 敵のポップ数
	static constexpr int SLOW_COUNT_MAX = 3;            // スロー演出用のカウント最大値
	static constexpr float GAME_TIME = 500.0f;          // 制限時間
	static constexpr float GAME_TIME_DEFEAT_DEC = 75.0f;// 撃破時の制限時間減少量

	// 敵の初期配置座標リスト
	const std::vector<VECTOR> ENEMY_POS = 
	{
		{ -1254.0f, 1642.0f,  -890.0f },
		{   340.0f, 1715.0f,  1280.0f },
		{   890.0f, 1523.0f,  -450.0f },
		{ -1620.0f, 1789.0f,   310.0f },
		{   120.0f, 1610.0f,  -780.0f },
		{  -450.0f, 1554.0f,  1420.0f },
		{  1150.0f, 1678.0f,   980.0f },
		{  -210.0f, 1745.0f,  -320.0f },
		{   670.0f, 1580.0f,   410.0f },
		{ -1100.0f, 1795.0f,   850.0f },
		{   430.0f, 1665.0f, -1350.0f },
		{  -780.0f, 1512.0f,  -670.0f },
		{  1480.0f, 1730.0f,  -210.0f },
		{    50.0f, 1590.0f,   650.0f },
		{  -530.0f, 1705.0f,   590.0f },
		{  -910.0f, 1630.0f,   180.0f },
		{  1320.0f, 1770.0f,   610.0f },
		{   820.0f, 1540.0f, -1020.0f },
		{  -340.0f, 1680.0f, -1560.0f },
		{   210.0f, 1760.0f,  -190.0f },
		{  -720.0f, 1605.0f, -1210.0f },
		{   960.0f, 1725.0f,   120.0f },
		{ -1410.0f, 1515.0f,  -420.0f },
		{   580.0f, 1690.0f,  1620.0f },
		{  -180.0f, 1575.0f,  1050.0f },
		{  1650.0f, 1640.0f,   490.0f },
		{   -60.0f, 1735.0f, -1890.0f },
		{  1040.0f, 1560.0f, -1510.0f }
	};

	// ゲームの進行ステート
	enum class GAME_STATE
	{
		NONE = -1, // 未設定
		GAME,      // ゲームプレイ中
		GAME_END,  // ゲーム終了演出中
	};

	// プレイヤーの初期配置座標リスト
	const std::vector<VECTOR> PLAYER_INIT_POS =
	{
		{ 0.0f, 0, -3000.0f },
		{ 250.0f, 0, -3250.0f },
		{ -250.0f, 0, -3250.0f },
		{ 0.0f, 0, -3500.0f }
	};

	// キャラクター・オブジェクト関連
	std::vector<std::unique_ptr<Player>> players_;       // プレイヤーのリスト
	std::unique_ptr<Boss> boss_;                         // ボスオブジェクト
	std::vector<std::unique_ptr<EnemyRobo>> enemyRobots_; // 敵ロボのリスト
	std::unique_ptr<Stage> stage_;                       // ステージオブジェクト
	std::unique_ptr<DamageController> damageController_; // ダメージコントローラー
	std::unique_ptr<GameTimer> gameTimer_;               // ゲームタイマー

	// 画像ハンドル関連
	int targetHpImage_;     // ターゲットHP画像
	int targetHpBerImage_;  // ターゲットHPバー背景画像
	int infoImage_;         // 情報表示用画像
	int gameTexts_;         // ゲームテキスト用画像
	int playerHpImage_;     // プレイヤーHP画像
	int playerHpImageBack_; // プレイヤーHPバー背景画像

	// UIゲーム用列挙型
	enum class UI_GAME
	{
		TIMER_LABEL, // タイマーラベル
		MAX          // 最大数
	};
	std::array<int, static_cast<int>(UI_GAME::MAX)> uiGame_; // ゲーム用UIハンドル配列

    // 制御・パラメータ関連
	float tempTime_;                                     // 一時的な時間保持用
	int slowCount_;                                      // スロー演出用カウンター
	std::array<int, 4> imageResult_;                     // リザルト用画像ハンドル配列
	bool isGameOver_;                                    // ゲームオーバーかどうか
	RainShaderParams rainyParams_;                       // 雨シェーダーパラメータ
	float rainyTime_;                                    // 雨用経過時間

	// ステート管理関連
	GAME_STATE state_;                                   // 現在のゲームステート
	int stateBase_;                                      // ステートの基底値
	std::map<int, std::function<void(void)>> stateChanges_; // ステート変更時の初期処理マップ
	std::function<void(void)> stateUpdate_;              // 現在の更新処理関数
	std::function<void(void)> stateDraw_;                // 現在の描画処理関数

	/// @brief HPバーのスケール計算
	/// @param _targetPos 対象のワールド座標
	/// @return 計算されたスケール値
	float CalcHpBarScale(const VECTOR& _targetPos);

	/// @brief デバッグ情報の描画
	void DrawDebug(void);

	/// @brief プレイヤーのHPバー描画
	void DrawHpBerPlayer(void);

	/// @brief ボスのHPバー描画
	void DrawHpBerBoss(void);

	/// @brief ダメージ処理
	void DamageProcess(void);

	/// @brief ゲーム時間の更新
	void UpdateGameTime(void);

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;

	/// @brief 状態遷移（GAME_STATE版）
	/// @param _state 遷移先のステート
	void ChangeState(GAME_STATE _state);

	/// @brief 状態遷移（int版）
	/// @param _state 遷移先のステート値
	void ChangeState(int _state);

	/// @brief ゲームプレイ中への遷移初期処理
	void ChangeGame(void);

	/// @brief ゲーム終了への遷移初期処理
	void ChangeGameEnd(void);

	/// @brief ゲームプレイ中の更新処理
	void UpdateGame(void);

	/// @brief ゲーム終了演出中の更新処理
	void UpdateGameEnd(void);

	/// @brief ゲームプレイ中の描画処理
	void DrawGame(void);

	/// @brief ゲーム終了演出中の描画処理
	void DrawGameEnd(void);
};