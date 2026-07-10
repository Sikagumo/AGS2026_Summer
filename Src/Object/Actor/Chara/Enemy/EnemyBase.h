#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include <functional>
#include "../CharaBase.h"
class EnemyBase :
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

    EnemyBase();
    virtual ~EnemyBase();


    void DrawDebug(void)override;

	void SetPlayerPos(VECTOR _pos) { playerPos_ = _pos; }

protected:

	virtual void InitAnimation(void)override;

	virtual void UpdateProcess(void)override;
	virtual void UpdateProcessPost(void)override;

	virtual void DrawPre(void)override;


	// 移動方向に応じた遅延回転
	void DelayRotate(void)override;


	/// @brief 位置に加算する値を計算
	VECTOR CalcAddPosition(void)override { return UtilityMath::VECTOR_ZERO; };

	// 衝突判定
	
	void CollisionReserve(void)override {};


	// 状態遷移
	virtual void ChangeState(STATE _state);
	// 状態遷移
	virtual void ChangeState(int state);
	virtual void ChangeStateIdle(void);
	virtual void ChangeStateAttack(void);
	virtual void ChangeStateMOVE(void);
	virtual void ChangeStateEnd(void);
	// 更新系
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;
	virtual void UpdateIdle(void);
	virtual void UpdateAttack(void);
	virtual void UpdateStateMOVE(void);
	virtual void UpdateEnd(void);


	VECTOR playerPos_;




private:

};

