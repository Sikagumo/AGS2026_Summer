#pragma once
#include <DxLib.h>
#include <memory>
#include "../CharaBase.h"
#include "../../../Common/Transform.h";

class WeponBase;


class Boss : public CharaBase
{
public:
	Boss(void);
	~Boss(void) override;

	void Release(void) override;



private:
	//bossの大きさ
	static constexpr VECTOR BOSS_SIZE = { 3.0f, 3.0f, 3.0f };
	//bossの初期座標
	static constexpr VECTOR BOSS_INIT_POS= { 0.0f, 0.0f, 0.0f };
	//回転
	static constexpr float INIT_ROT = 180.0f;
	static constexpr int JOINT_NO = 12;


	Transform transformFeet_;
	Transform transformBody_;


	float hp_;
	float attackDelay_;





protected:
	// リソースロード
	void InitLoad(void) override;

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

	void CollisionReserve(void) override  {};
};

