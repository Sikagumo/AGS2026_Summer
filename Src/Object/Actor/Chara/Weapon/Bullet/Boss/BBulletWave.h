#pragma once
#include "BBulletBase.h"

class BBulletWave:public BBulletBase
{
public:
	BBulletWave(Transform& _transform);
	~BBulletWave(void)override;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;

	

	void SetIsAttac(bool _isAttac)override { isAttac_ = _isAttac; }

	void SetPos(VECTOR _pos);

	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radiuse)override {}
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
	static constexpr float INIT_RADIUS = 4.0f;
	static constexpr float INCREASE_RADIUS = 4.0f;
	static constexpr float MAX_RADIUS = 1000.0f;


	Transform bossTransform_;
	float radius_;

};

