#pragma once
#include "../WeaponBase.h"
class WeaponMGL :
	public WeaponBase
{
public:

	WeaponMGL(void);

	~WeaponMGL(void)override = default;

	// リソースロード
	void Load(void) override;

	void Release(void)override;

	void SetBone(int _id,Transform _trans, ColliderBase::TAG _tag) override;
	int GetDamage(void) const override;
	VECTOR GetPos(void) const override;

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

	float Damage(void) override;

	// 前描画
	void DrawPre(void) override;

	void CollisionReserve(void) override {};
};

