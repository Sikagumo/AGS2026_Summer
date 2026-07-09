#pragma once
#include "./PlayerBase.h"
#include "./PActionController.h"
#include <DxLib.h>
#include <array>

class Player : public PlayerBase
{
private:

	static constexpr float KNOCK_POW_Y = 5.0f;
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
		DODGE,
		MAX,
	};

	/// @brief コンストラクタ
	/// @param _playerNo プレイヤー番号
	/// @param _jobType 攻撃の種類
	/// @param _startPos 開始位置
	Player(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos);

	~Player(void)override = default;

	void Load(void)override;

	void Draw(void)override;

	void DrawDebug(void)override;

	void ReleasePost(void)override;

	/// @brief 吹っ飛ばし処理
	/// @param _knockDirXZ 横吹っ飛ばし方向
	/// @param _knockPowXZ 横吹っ飛ばし力
	/// @param _isStan スタンさせるか否か 
	/// @param _knockPowY 縦吹っ飛ばし力
	void SetKnock(const VECTOR& _knockDirXZ, float _knockPowXZ
					, bool _isStan, float _knockPowY = KNOCK_POW_Y);

	void SetRespawn(void);
	
	void SetPlayerType(PLAYER_TYPE _type = PLAYER_TYPE::MAX);
	
	void SetSoundData(VECTOR _pos, float _radius, bool _isLanging, bool _isMGFire, bool _isRoad)override;


protected:

	void InitAnimation(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitPost(void)override;


	void UpdateProcess(void);
	void UpdateProcessPost(void);


	void DrawPre(void)override;
	void DrawLate(void)override;

	VECTOR CalcAddPosition(void) override;


private:

	static constexpr std::array<SHOT_TYPE, static_cast<int>(JOB_TYPE::MAX)>
		SHOT_TYPE_NORMAL
		= { SHOT_TYPE::BOMB, SHOT_TYPE::BIG, SHOT_TYPE::RAPID_FIRE, SHOT_TYPE::RECOVERY };

	static constexpr std::array<SHOT_TYPE, static_cast<int>(JOB_TYPE::MAX)>
		SHOT_TYPE_SPECIAL
		= { SHOT_TYPE::BOMB_FINISH, SHOT_TYPE::NONE, SHOT_TYPE::CLUSTER, SHOT_TYPE::POISON };


	// 連射パラメータ
	static constexpr float SCALE_RAPID = 1.0f;
	static constexpr float RADIUS_RAPID = 10.0f;
	static constexpr int POWER_RAPID = 1;
	static constexpr float SHOT_SPEED_XZ_RAPID = 17.5f;
	static constexpr float SHOT_SPEED_Y_RAPID = 2.5f;
	static constexpr float ALIVE_TIME_RAPID = 3.0f;


	// 攻撃回数
	int attackNumMax_;
	int curAttackNum_;

	int shotIndex_;
	int shadowHandle_;

	ANIM_TYPE animType_;

	// 投げる位置
	VECTOR throwPos_;
	VECTOR throwDir_;

	// 吹っ飛ばし量
	Vector2F knockPowXZ_;

	Vector2F dodgePowXZ_;

	// カメラに応じた回転をするか否か
	bool isCameraRotActive_;

	// 回避クールタイム
	float curTimeWaitDodge_;

	std::unique_ptr<PActionController> actionController_;

	static constexpr float SHOT_RAPID_TERM = 0.5f;
	float shotTerm_;

	//static constexpr float THROW_SPEED_RAPID_START = 30.0f;
	//static constexpr float THROW_SPEED_RAPID_MAX = 75.0f;
	//static constexpr float THROW_SPEED_RAPID_INC = (THROW_SPEED_RAPID_MAX - THROW_SPEED_RAPID_START) / 30;

	//float animSpeedRapid_;


	// 操作
	void ProcessMove(void);

	/// @brief ジャンプ処理
	void ProcessJump(void);
	void Jump(void);

	/// @brief 回避処理
	void ProcessDodge(void);
	void Dodge(void);

	void ProcessKnock(void);

	void ProcessAttack(void);


	void DrawShadowRound(void);

	/// @brief アニメーション再生
	/// @param _type アニメーションの種類
	/// @param _isLoop ループ再生するか否か
	/// @param _animSpeed 再生速度指定(任意)
	void PlayAnimation(ANIM_TYPE _type, bool _isLoop = true, float _animSpeed = -1.0f);

	void CreateBullet(void);
	void ShotBullet(void);
	void UpdateBullets(void);

	void DelayRotate(void)override;

	void MoveLimit(void);

	void DrawShotOrbit(void);

	VECTOR CalcShotDir(void);

	void ProcShotNormal(void);
	void ProcShotSpecial(void);
};