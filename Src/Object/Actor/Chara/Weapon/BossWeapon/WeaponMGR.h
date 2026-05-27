#pragma once
#include "../WeaponBase.h"
class WeaponMGR :
    public WeaponBase
{
public:

	WeaponMGR();

	~WeaponMGR(void)override = default;

	// リソースロード
	void Load(void) override;

	void SetBone(int _id, Transform _trans, ColliderBase::TAG _tag) override;

	VECTOR GetPos(void) const override;
	void SetDamage(int _damage)override { hp_ -= _damage; }
	bool GetIsAlive(void)override { return isAlive_; }
	int GetHp(void)override { return hp_; }
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

	void ReleasePost(void)override;

	void CollisionReserve(void) override {};
};

