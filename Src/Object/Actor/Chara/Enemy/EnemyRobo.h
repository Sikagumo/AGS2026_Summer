#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include <functional>
#include "../CharaBase.h"

class EnemyRobo :
	public CharaBase
{
public:

	/// @brief ステートの種類
	enum class STATE
	{
		IDLE,   // 待機状態
		ATTACK, // 攻撃状態
		MOVE,   // 移動状態
		END,    // 死亡/終了状態
	};

	/// @brief アニメーションの種類
	enum class ANIM_TYPE
	{
		ATTACKA, // 攻撃パターンA
		ATTACKB, // 攻撃パターンB
		DIR,     // 基本姿勢（向き）
		WARK,    // 歩行
		MAX,     // アニメーション種類の最大数
	};

	/// @brief コンストラクタ
	/// @param _pos 初期出現位置
	EnemyRobo(VECTOR _pos);

	/// @brief デストラクタ
	~EnemyRobo() override;

	/// @brief リソースロード処理
	void Load(void) override;

	/// @brief ターゲットとなるプレイヤーの座標を設定する
	/// @param _pos プレイヤー座標
	void SetPlayerPos(VECTOR _pos) { playerPos_ = _pos; }

	/// @brief 生存しているかどうかを取得する
	/// @return 生存フラグ（HP > 0）
	bool IsAlive(void) const { return hp_ > 0; };

protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	// 更新処理
	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

	// 描画処理
	void DrawPre(void) override;

	void CollisionReserve(void) override {};

	//ステート更新
	STATE state_;											//現在の状態
	int stateBase_;											//状態の基準値（数値管理用）
	std::map<int, std::function<void(void)>> stateChanges_;	//状態遷移時の初期処理マップ
	std::function<void(void)> stateUpdate_;					//現在の状態の更新処理関数

	/// @brief 状態を変更する
	/// @param _state 変更後の状態
	void ChangeState(STATE _state);

	/// @brief 状態を変更する（数値指定）
	/// @param state 状態番号
	void ChangeState(int state);

	/// @brief 待機状態への遷移処理
	void ChangeStateIdle(void);

	/// @brief 攻撃状態への遷移処理
	void ChangeStateAttack(void);

	/// @brief 移動状態への遷移処理
	void ChangeStateMove(void);

	/// @brief 死亡/終了状態への遷移処理
	void ChangeStateEnd(void);

	

	/// @brief 待機状態の更新処理
	void UpdateIdle(void);

	/// @brief 攻撃状態の更新処理
	void UpdateAttack(void);

	/// @brief 移動状態の更新処理
	void UpdateStateMove(void);

	/// @brief 死亡/終了状態の更新処理
	void UpdateEnd(void);

private:

	// トランスフォーム関連
	static constexpr VECTOR INIT_SCALE = { 2.0f, 2.0f, 2.0f };         // 初期モデルスケール
	static constexpr float INIT_ROT_Y = 180.0f;                        // 初期Y軸回転角度（度）

	// 当たり判定（コライダー）関連
	static constexpr VECTOR COL_LINE_START_POS = { 0.0f, 60.0f, 0.0f };   // ライン判定の始点位置
	static constexpr VECTOR COL_LINE_END_POS = { 0.0f, -1.0f, 0.0f };     // ライン判定の終点位置
	static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f, 90.0f, 0.0f };// カプセル判定の始点位置
	static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f, 40.0f, 0.0f };  // カプセル判定の終点位置
	static constexpr float COL_CAPSULE_END_RADIUS = 30.0f;               // カプセル判定の半径
	static constexpr VECTOR COL_SPHERE_POS = { 0.0f, 40.0f, 40.0f };      // 索敵/本体球状判定の相対位置
	static constexpr float COL_SPHERE_RADIUS = 20.0f;                    // 索敵/本体球状判定の半径
	static constexpr float COL_ATTACK_SPHERE_RADIUS = 20.0f;             // 攻撃用球状判定の半径
	static constexpr int BONE_HAND_INDEX = 52;                            // 手のボーン番号（攻撃判定の基準位置）

	// アニメーション関連
	static constexpr VECTOR ANIM_OFFSET = { 0.0f, 0.0f, -0.25f };        // アニメーション再生時のオフセット位置
	static constexpr float ANIM_SPEED = 20.0f;                            // アニメーション再生速度
	static constexpr int ATTACK_ANIM_DIVISOR = 2;                         // 攻撃アニメーション分岐用の時間除数

	// ステータス・移動関連
	static constexpr int MAX_HP = 200;                                    // 最大HP（初期HP）
	static constexpr float MOVE_SPEED_INIT = 5.0f;                        // 通常移動速度
	static constexpr int NORMAL_DAMAGE = 200;                             // 通常攻撃を受けた際の被ダメージ量
	static constexpr int POISON_DAMAGE = 1;                               // 毒状態時の持続被ダメージ量

	// 演出・タイマー関連
	static constexpr float COUNT_MAX = 10.0f;                             // 死亡演出タイマーの上限値
	static constexpr VECTOR END_EFFECT_ROT = { 0.0f, 0.0f, 0.0f };       // 死亡時爆発エフェクトの回転角度
	static constexpr VECTOR END_EFFECT_SCALE = { 50.0f, 50.0f, 50.0f };  // 死亡時爆発エフェクトのスケール
	static constexpr float END_EFFECT_SPEED = 1.0f;                       // 死亡時爆発エフェクトの再生速度

	int hp_;             // 現在のHP
	VECTOR playerPos_;   // ターゲットプレイヤーの現在座標
	VECTOR moveDir_;     // 移動方向ベクトル
	int count_;          // 死亡演出等の汎用カウントタイマー
	bool poizun_;        // 毒状態（持続ダメージ）フラグ

	/// @brief ターゲット（プレイヤー）の方を向く処理
	void LockPlayer(void);

	/// @brief 被ダメージ・状態異常処理
	void Damage(void);
};