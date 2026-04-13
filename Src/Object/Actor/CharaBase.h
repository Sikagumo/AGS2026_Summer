#pragma once
#include "./ActorBase.h"
class AnimationController;

class CharaBase : public ActorBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		VIEW_RANGE,
		MAX,
	};

	// コンストラクタ
	CharaBase(void);

	// デストラクタ
	virtual ~CharaBase(void) override = default;

	void Update(void)override final;

	void Release(void)override;

protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -30.0f;

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION = 20;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 1.0f;


	AnimationController* animation_;

	int shadowHandle_;

	float moveSpeed_;

	VECTOR moveDir_;

	VECTOR movePow_;

	VECTOR jumpPow_;

	// 移動前の座標
	VECTOR prevPos_;

	// ジャンプの入力受付時間
	float stepJump_;

	// ジャンプ判定
	bool isJump_;

	
	virtual void InitLoad(void)override final;
	virtual void InitLoadPost(void) = 0;
	virtual void InitAnimation(void)override final;
	virtual void InitAnimationPost(void) = 0;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override;

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	void CalcGravityPow(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
	virtual void CollisionReserve(void) {};

	
	void DrawLate(void)override;

	// 影描画
	void DrawShadowRound(void);
};