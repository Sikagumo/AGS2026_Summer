#pragma once
#include "../../ActorBase.h"

class WeaponBase :
    public ActorBase
{
public:
	WeaponBase(void);
	~WeaponBase(void);


	// 更新
	virtual void Update(void) ;

	// 描画
	virtual void Draw(void);

	// 解放
	virtual void Release(void);

private:


protected:

	float hp_;
	float attackDamage_;

	


	// リソースロード
	virtual void InitLoad(void) override = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) override = 0;

	// 衝突判定の初期化
	virtual void InitCollider(void) override = 0;

	// アニメーションの初期化
	virtual void InitAnimation(void) override = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) override=0;

	// 前描画
	virtual void DrawPre(void) override  {};



	//受けたダメージをボスに伝えるための関数
	virtual float Damage(void);

};

