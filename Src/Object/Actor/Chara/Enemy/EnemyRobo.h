#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include <functional>
#include "EnemyBase.h"
class EnemyRobo :
    public EnemyBase
{
public:
    EnemyRobo();
    ~EnemyRobo()override;


    // リソースロード
    void Load(void) override;

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

	// 状態遷移
	void ChangeState(STATE _state) override;
	// 状態遷移
	void ChangeState(int state) override;
	void ChangeStateIdle(void) override;
	void ChangeStateAttack(void) override;
	void ChangeStateMOVE(void) override;
	void ChangeStateEnd(void) override;
	// 更新系
	void UpdateIdle(void) override;
	void UpdateAttack(void) override;
	void UpdateStateMOVE(void) override;
	void UpdateEnd(void) override;

private:

	//当たり判定の座標
	//ライン
	static constexpr VECTOR COL_LINE_START_POS = { 0.0f,60.0f,0.0f };
	static constexpr VECTOR COL_LINE_END_POS = { 0.0f,-1.0f,0.0f };
	//カプセル
	static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f,130.0f,0.0f };
	static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f,80.0f,0.0f };
	static constexpr float COL_CAPSULE_END_RADIUS = 80.0f;

};

