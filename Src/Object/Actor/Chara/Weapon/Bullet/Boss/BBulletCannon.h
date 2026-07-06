#pragma once
#include "BBulletBase.h"
class BBulletCannon :
    public BBulletBase
{
public:
	BBulletCannon();
	~BBulletCannon(void)override;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;



	void SetIsAttack(bool _isAttack)override { isAttack_ = _isAttack; }

	void SetPos(VECTOR _pos) {};

	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius)override { transform_.pos = _pos; dir_ = _dir; radius_ = _radius; }

	void SetPlayerPos(VECTOR _pos)override {}

	void SetUpMaxPos_(float _pos)override {}

	void SetTransform(Transform trans)override { weaponTrans_ = trans; }

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





private:



	static constexpr float INIT_SPEED = 80.0f;
	static constexpr float MAX_ALIVE_TIME = 300.0f;

	int aliveTime_ = 0;
};

