#pragma once
#include "../Actor/CharaBase.h"

class Player : public CharaBase
{
public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		MAX,
	};

	Player(void);

	~Player(void)override = default;

protected:


	void InitLoadPost(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	// 衝突判定の初期化
	void InitCollider(void)override;

	// アニメーションの初期化
	void InitAnimationPost(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	// 更新処理
	void UpdateProcess(void)override;

	// 
	void UpdateProcessPost(void)override;

	void CollisionReserve(void)override;


private:

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };

	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };


	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS = { 0.0f, 160.0f, 0.0f };

	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS = { 0.0f, 80.0f, 0.0f };


	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;
	

	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 5000.0f;

	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 575.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_INPUT = 0.6f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 5.0f;

	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 10.0f;

	bool isDash_;


	// 操作
	void ProcessMove(void);

	// ジャンプ
	void ProcessJump(void);

	void PlayAnim(ANIM_TYPE type, bool _isLoop = true);
};