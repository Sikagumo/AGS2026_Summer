#pragma once
#include "BBulletBase.h"
class BBulletLaser :
	public BBulletBase
{
public:

	BBulletLaser(void);

	virtual ~BBulletLaser(void);

	void SetIsAttack(bool _isAttack)override { isAttack_ = _isAttack; }
	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius)override { transform_.pos = _pos; dir_ = _dir; radius_ = _radius; }

	void SetPlayerPos(VECTOR _pos)override {}

	void SetUpMaxPos_(float _pos)override {}

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

};

