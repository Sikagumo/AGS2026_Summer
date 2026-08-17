#pragma once
#include "./PlayerBase.h"
#include "./PActionController.h"
#include <DxLib.h>
#include <array>

class Player : public PlayerBase
{
private:

	// 吹っ飛ばし量
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
		DEFEAT,
		MAX,
	};

	enum class ACTION_TYPE
	{
		NONE = -1,
		JUMP,
		DODGE,
		DEFEAT,
		ATTACK_SPECIAL,
		ATTACK,
	};

	/// @brief コンストラクタ
	/// @param _playerNo プレイヤー番号
	/// @param _jobType 攻撃の種類
	/// @param _skinType 見た目の種類
	/// @param _startPos 開始位置
	Player(int _playerNo, JOB_TYPE _jobType, SKIN_TYPE _skinType, const VECTOR& _startPos);

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
	
	void SetSoundData(VECTOR _pos, float _radius, bool _isLanging, bool _isMGFire, bool _isRoad)override;

	// 自分が操作するキャラクターかどうかを設定
	void SetHostControl(bool _isLocal);

	//自分が操作するキャラクターかどうかを取得する
	bool GetHostControl(void) const { return isHostControl_; }

	// ネットワークから受け取った情報を強制的にセットする（ラジコン用）
	void SetNetworkAction(const VECTOR& _pos, const Quaternion& _rot, int _animId, bool _isAttack, int _currentHp);

	/// @brief リスポーンしたか否か
	bool GetIsRespawn(void)const;


	// キャラクターの通信キーを設定
	void SetNetKey(int _key);

	// キャラクターの通信キーを取得
	int GetNetKey(void) const { return netKey_; }

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
		= { SHOT_TYPE::BOMB, SHOT_TYPE::BIG, SHOT_TYPE::RAPID_FIRE, SHOT_TYPE::POISON };

	static constexpr std::array<SHOT_TYPE, static_cast<int>(JOB_TYPE::MAX)>
		SHOT_TYPE_SPECIAL
		= { SHOT_TYPE::BOMB_FINISH, SHOT_TYPE::BIG, SHOT_TYPE::CLUSTER, SHOT_TYPE::RECOVERY };

	// 連射パラメータ
	static constexpr float SCALE_RAPID = 0.85f;
	static constexpr float RADIUS_RAPID = 10.0f;
	static constexpr int POWER_RAPID = 18;
	static constexpr float SHOT_SPEED_XZ_RAPID = 17.5f;
	static constexpr float SHOT_SPEED_Y_RAPID = 2.5f;
	static constexpr float ALIVE_TIME_RAPID = 2.5f;


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

	// 撃破時間
	float curTimeDefeat_;

	std::unique_ptr<PActionController> actionController_;

	float shotTerm_;

	// 自分が操作するかどうか
	bool isHostControl_;

	// 誰のキャラクターかを識別key
	int netKey_;

	// 攻撃したかどうか
	bool isNetAttack_;

	bool isAttackSend_;


	// 操作
	void ProcessMove(void);

	/// @brief ジャンプ処理
	void ProcessJump(void);
	void Jump(void);

	/// @brief 回避処理
	void ProcessDodge(void);
	void Dodge(void);

	/// @brief 回避処理
	void ProcessDefeat(void);

	void ProcessKnock(void);

	void ProcessAttack(void);

	void SetRespawn(void);


	/// @brief アニメーション再生
	/// @param _type アニメーションの種類
	/// @param _isLoop ループ再生するか否か
	/// @param _animSpeed 再生速度指定(任意)
	void PlayAnimation(ANIM_TYPE _type, bool _isLoop = true, bool _isAnimBlend = true, float _animSpeed = -1.0f);

	void CreateBullet(void);
	void ShotBullet(void);
	void UpdateBullets(void);

	/* 拡散弾処理 */
	void CreateCluster(void);
	void ShotCluster(void);
	std::unique_ptr<PBulletNormal> _CreateClusterBullet(const VECTOR& _throwDir);

	void DelayRotate(void)override;

	void DrawShotOrbit(void);

	VECTOR CalcShotDir(void);

	void ProcShotNormal(void);
	void ProcShotSpecial(void);

	// 自分の最新状態をパケット用に変換して NetManager に送る
	void SendMyActionToNetManager(void);
};