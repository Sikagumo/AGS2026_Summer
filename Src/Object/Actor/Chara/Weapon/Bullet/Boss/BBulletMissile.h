#pragma once
#include "BBulletBase.h"
class BBulletMissile :
    public BBulletBase
{
public:
	BBulletMissile();
	~BBulletMissile(void)override;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;



	void SetIsAttack(bool _isAttack)override { isAttack_ = _isAttack; }

	void SetPos(VECTOR _pos) {};

	void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius)override { transform_.pos = _pos; dir_ = _dir; radius_ = _radius; }

	void SetTransform(Transform trans)override { weaponTrans_ = trans; }

	void SetPlayerPos(VECTOR _pos)override { playerPos_ = _pos; }

	void SetUpMaxPos_(float _pos)override { maxPos_ = _pos; }

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

	//影の描画する高さ式（前が高さ)(後ろが補正値)
	static constexpr float SHADOW_POS_Y = -7.0f + 0.5;
	//影の濃さの最大値
	static constexpr int MAX_SHADOW_COL = 255;
	//影を完全に消す高さ
	static constexpr float SHADOW_FADE_HEIGHT = 3000.0f;


	static constexpr int MISSILE_COUNT = 6;

	// 頂点番号の定数
	static constexpr int LEFT_BACK = 0;
	static constexpr int LEFT_FORWARD = 1;
	static constexpr int RIGHT_BACK = 2;
	static constexpr int RIGHT_FORWARD = 3;

	// 頂点配列
	VERTEX3D imageVertex_[4];

	int fallingHandle_;
	bool isUp_;
	bool isAttack_;
	VECTOR playerPos_;
	int attackCount_;
	
	float maxPos_;

	void MoveUp(void);
	void MoveDown(void);
	void Attack(void);

	void DrawAreaAlert(void);

};

