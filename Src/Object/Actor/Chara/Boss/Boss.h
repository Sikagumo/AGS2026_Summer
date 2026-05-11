#pragma once
#include <DxLib.h>
#include "../../CharaBase.h"
#include "../../../Common/Transform.h";

class Boss : public CharaBase
{
public:
	Boss();
	~Boss() override;

	void Release() override;

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

protected:
	// リソースロード
	void InitLoad(void);

	// 大きさ、回転、座標の初期化
	void InitTransform(void);

	// 衝突判定の初期化
	void InitCollider(void);

	// アニメーションの初期化
	void InitAnimationPost(void);

	// 初期化後の個別処理
	void InitPost(void);

	virtual void UpdateProcess(void);
	virtual void UpdateProcessPost(void);

	

	// 前描画
	void DrawPre(void);

	void CollisionReserve(void) {};
};

