#pragma once
#include "BBulletBase.h"
class BBulletMG :
    public BBulletBase
{
public:
	BBulletMG(Transform& _transform);
	~BBulletMG(void)override;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;



	void SetIsAttac(bool _isAttac)override { isAttac_ = _isAttac; }

	void SetPos(VECTOR _pos) {};

	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radiuse)override { transform_.pos = _pos; dir_ = _dir; radiuse_ = _radiuse; }

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

	

	static constexpr float INIT_SPEED = 10.0f;
	static constexpr float MAX_ALIVE_TIME = 120.0f;

	int aliveTime_ = 0;

};

