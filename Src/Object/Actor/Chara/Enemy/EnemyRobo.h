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

	///ステートパターン
	enum class STATE
	{
		IDLE,
		ATTACK,
		MOVE,
		END,
	};


	enum class ANIM_TYPE
	{
		ATTACKA,
		ATTACKB,
		DIR,
		WARK,
		MAX,
	};



    EnemyRobo(VECTOR _pos);
    ~EnemyRobo()override;


    // リソースロード
    void Load(void) override;

	void SetPlayerPos(VECTOR _pos) { playerPos_ = _pos; }

protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

	// 前描画
	void DrawPre(void) override;


	void CollisionReserve(void) override {};


	/// 状態
	STATE state_;
	/// 状態管理
	int stateBase_;

	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;
	// 状態遷移
	void ChangeState(STATE _state);
	// 状態遷移
	void ChangeState(int state);
	void ChangeStateIdle(void);
	void ChangeStateAttack(void);
	void ChangeStateMove(void);
	void ChangeStateEnd(void);
	// 更新系
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;
	void UpdateIdle(void);
	void UpdateAttack(void);
	void UpdateStateMove(void);
	void UpdateEnd(void);

	
private:

	//当たり判定の座標
	//ライン
	static constexpr VECTOR COL_LINE_START_POS = { 0.0f,60.0f,0.0f };
	static constexpr VECTOR COL_LINE_END_POS = { 0.0f,-1.0f,0.0f };
	//カプセル
	static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f,90.0f,0.0f };
	static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f,40.0f,0.0f };
	static constexpr float COL_CAPSULE_END_RADIUS = 30.0f;

	static constexpr float COUNT_MAX = 10.0f;

	static constexpr float MOVE_SPEED_INIT = 5.0f;

	int hp_;
	VECTOR playerPos_;
	VECTOR moveDir_;
	int count_;
	bool poizun_;


	void LockPlayer(void);

	void Damez(void);
};

