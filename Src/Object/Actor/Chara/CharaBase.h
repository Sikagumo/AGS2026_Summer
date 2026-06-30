#pragma once
#include "../ActorBase.h"
#include "../../Common/AnimationController.h"
#include "../../../Utility/UtilityMath.h"
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

	/// @brief コンストラクタ
	CharaBase(void);

	/// @brief デストラクタ
	virtual ~CharaBase(void) override = default;

	void Update(void)override final;

	virtual void DrawDebug(void);

protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -300.0f;

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION = 20;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 1.0f;

	//影の描画する高さ式（前が高さ)(後ろが補正値)
	static constexpr float SHADOW_POS_Y = -22.0f+0.5;
	//影の濃さの最大値
	static constexpr int MAX_SHADOW_COL = 255;
	//影を完全に消す高さ
	static constexpr float SHADOW_FADE_HEIGHT = 3000.0f;


	std::unique_ptr<AnimationController> animation_;

	float moveSpeed_;

	VECTOR moveDir_;

	VECTOR movePow_;

	float jumpPow_;

	// 移動前の座標
	VECTOR prevPos_;

	// ジャンプ判定
	bool isJump_;

	int shadowHandle_;
	
	// 頂点番号の定数
	static constexpr int LEFT_BACK = 0;
	static constexpr int LEFT_FORWARD = 1;
	static constexpr int RIGHT_BACK = 2;
	static constexpr int RIGHT_FORWARD = 3;

	// 頂点配列
	VERTEX3D imageVertex_[4];


	virtual void InitAnimation(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override;


	// 移動方向に応じた遅延回転
	virtual  void DelayRotate(void);

	void CalcGravityPow(void);

	/// @brief 位置に加算する値を計算
	virtual VECTOR CalcAddPosition(void) { return UtilityMath::VECTOR_ZERO; };

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
	virtual void CollisionReserve(void) {};

	void DrawShadowRound(float shadowScl);

};