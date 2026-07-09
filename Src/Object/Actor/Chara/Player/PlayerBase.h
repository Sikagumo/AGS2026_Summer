#pragma once
#include "../CharaBase.h"
#include <memory>
#include <vector>
#include "../Weapon/Bullet/Player/PBulletBase.h"

class PlayerBase : public CharaBase
{
public:

	enum class JOB_TYPE
	{
		NONE = -1
		, BOMB		  // 爆破
		, CANNON	  // 巨大
		, RAPID_FIRE  // 連射
		, SUPPORT	  // 回復
		, MAX
	};

	enum class SHOT_TYPE
	{
		NONE = -1

		, BOMB		// 爆破
		, BOMB_FINISH

		, BIG // 巨大

		, RAPID_FIRE // 連射
		, CLUSTER	 // 拡散


		, RECOVERY	// 回復
		, POISON	// 毒
		, MAX
	};

	enum class PLAYER_TYPE
	{
		HYMAN,
		DOG,
		MONKEY,
		BIRD,

		MAX
	};

	enum class PLAYER_STATE
	{
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
	};

	static constexpr float TIME_INVINCIBLE = 1.0f;

	/// @brief コンストラクタ
	/// @param _playerNo プレイヤー番号
	/// @param _jobType 弾の種類
	/// @param _startPos 初期位置
	/// @param _playerType プレイヤーの見た目の種類
	PlayerBase(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos, PLAYER_TYPE _playerType = PLAYER_TYPE::HYMAN);

	virtual ~PlayerBase(void)override = default;


	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	PLAYER_TYPE GetPlayerType(void)const { return playerType_; };
	JOB_TYPE GetJobType(void)const { return jobType_; };
	SHOT_TYPE GetShotType(void)const { return shotType_; };

	/// @brief プレイヤーにダメージ処理
	/// @param _damage ダメージ量
	/// @param _isInvincible 無敵化させるか否か
	/// @param _timeInvincible 無敵時間
	/// @param _isIgnoreInvincible 無敵を無視するか否か
	void SetDamage(int _damage, bool _isInvincible = true, float _timeInvincible = TIME_INVINCIBLE, bool _isIgnoreInvincible = false);

	int GetCurHp(void)const { return hp_; };
	int GetMaxHp(void)const { return HP_MAX; };

	const VECTOR& GetPos(void)const { return transform_.pos; };
	VECTOR GetBodyPos(void)const { return bodyPos_; };

	virtual void SetSoundData(VECTOR _pos, float _radius, bool _isLanging,bool _isMGFire, bool _isRoad);

	/// @brief 弾を取得
	const std::vector<std::unique_ptr<PBulletBase>>& GetBullets(void)const { return bullets_; };


protected:

	// プレイヤー番号
	const int playerNo_;

	const int HP_MAX;

	// 初期位置
	const VECTOR START_POS;

	// 無敵時間
	float timeInv_;
	float timeInvDodge_;

	VECTOR bodyPos_;

	std::vector<std::unique_ptr<PBulletBase>> bullets_;
	
	int hp_;

	JOB_TYPE jobType_;

	SHOT_TYPE shotType_;

	PLAYER_TYPE playerType_;

	
	void UpdateSound(void);
};
