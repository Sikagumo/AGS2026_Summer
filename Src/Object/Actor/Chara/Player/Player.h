#pragma once
#include "./PlayerBase.h"
#include "../Player/PlayerBase.h"
#include "./PActionController.h"
class InputManager;
class PBulletBig;

class Player : public PlayerBase
{
public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		SHOT,
		JUMP,
		MAX,
	};

	Player(int _playerNo, BULLET_TYPE _playerType);

	~Player(void)override = default;

	void Release(void)override;


protected:

	void InitLoad(void)override;
	void InitAnimation(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitPost(void)override;


	void UpdateProcess(void);
	void UpdateProcessPost(void);


	void DrawPre(void)override;
	void DrawLate(void)override;
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


	InputManager& inputManager_;

	// 攻撃回数
	int attackNumMax_;
	int curAttackNum_;

	int shadowHandle_;

	ANIM_TYPE animType_;

	std::unique_ptr<PActionController> actionController_;


	// 操作
	void ProcessMove(void);

	// ジャンプ
	void ProcessJump(void);

	void ProcessAttack(void);


	void DrawShadowRound(void);
	void PlayAnim(ANIM_TYPE _type, bool _isLoop = true);

	void CreateBullet(void);
};