#pragma once
#include "./PlayerBase.h"
#include "../Player/PlayerBase.h"
#include "./PActionController.h"
#include <DxLib.h>

class Player : public PlayerBase
{
private:

	static constexpr float KNOCK_POW_Y = 100.0f;
public:

	enum class ANIM_TYPE
	{
		NONE = -1,
		IDLE,
		RUN,
		THROW_LEFT,
		THROW_RIGHT,
		THROW_RUN,
		JUMP,
		MAX,
	};

	Player(int _playerNo, BULLET_TYPE _playerType);

	~Player(void)override = default;

	void Load(void)override;

	void Draw(void)override;

	void DrawDebug(void)override;

	void ReleasePost(void)override;

	/// @brief 現在攻撃力取得
	int GetPower(void);

	/// @brief 吹っ飛ばし処理
	/// @param _knockDirXZ 横吹っ飛ばし方向
	/// @param _knockPowXZ 横吹っ飛ばし力
	/// @param _isStan スタンさせるか否か 
	/// @param _knockPowY 縦吹っ飛ばし力
	void SetKnock(const VECTOR& _knockDirXZ, float _knockPowXZ
					, bool _isStan, float _knockPowY = KNOCK_POW_Y);


protected:

	void InitAnimation(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitPost(void)override;


	void UpdateProcess(void);
	void UpdateProcessPost(void);


	void DrawPre(void)override;
	void DrawLate(void)override;
private:


	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 100.0f;

	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 250.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_INPUT = 1.85f;

	// 攻撃回数
	int attackNumMax_;
	int curAttackNum_;

	int shotIndex_;
	int shadowHandle_;

	ANIM_TYPE animType_;

	VECTOR throwPos_;
	VECTOR throwDir_;

	// カメラに応じた回転をするか否か
	bool isCameraRotActive_;

	std::unique_ptr<PActionController> actionController_;


	// 操作
	void ProcessMove(void);

	// ジャンプ
	void ProcessJump(void);

	void ProcessAttack(void);


	void DrawShadowRound(void);
	void PlayAnim(ANIM_TYPE _type, bool _isLoop = true);

	void CreateBullet(void);
	void ShotBullet(void);
	void UpdateBullets(void);

	void DelayRotate(void)override;
};