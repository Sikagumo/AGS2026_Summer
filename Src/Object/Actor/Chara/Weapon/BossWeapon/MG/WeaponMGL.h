#pragma once
#include <memory>
#include <vector>
#include "MGBase.h"

class BBulletBase;

class WeaponMGL :
	public MGBase
{
public:
	

	WeaponMGL(void);

	~WeaponMGL(void)override = default;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;
	

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

	

private:

	static constexpr VECTOR LINE_START_POS = { -50.0f,0.0f,50.0f };
	static constexpr VECTOR LINE_END_POS = { -50.0f,-10.0f,50.0f };
	static constexpr VECTOR CAPSULE_START_POS = { -50.0f,0.0f,140.0f };
	static constexpr VECTOR CAPSULE_END_POS = { -50.0f,0.0f,-40.0f };
	static constexpr int MAX_BULLET_COUNT = 200;

	//発射位置
	static constexpr int MUZZLE_MAX_COUNT = 6;

	const VECTOR MUZZLE_POS[MUZZLE_MAX_COUNT] = {
		{ -52.0f,4.0f,150.0f },
		{ -47.0f,1.0f,150.0f },
		{ -47.0f,-5.0f,150.0f },
		{ -52.0f,-8.0f,150.0f },
		{ -57.0f,1.0f,150.0f },
		{-57.0f,-5.0f,150.0f },
	};


	


	
	
};

