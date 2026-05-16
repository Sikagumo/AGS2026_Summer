#pragma once
#include "../CharaBase.h"
#include <memory>
#include <vector>
#include "../Weapon/Bullet/Player/PBulletBase.h"

class PlayerBase : public CharaBase
{
public:

	enum class BULLET_TYPE
	{
		NONE = -1,
		INTERFERING, // 妨害
		BIG,
		RAPID_FIRE, // 連射
		RECOVERY,	// 回復

		MAX,
	};

	enum class PLAYER_TYPE
	{
		HYMAN,
		DOG,
		MONKEY,
		BIRD,
	};

	enum class PLAYER_STATE
	{
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
	};

	PlayerBase(int _playerNo, BULLET_TYPE _bulletType, PLAYER_TYPE _playerType = PLAYER_TYPE::HYMAN);

	virtual ~PlayerBase(void)override = default;

	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	PLAYER_TYPE GetPlayerType(void)const { return playerType_; };
	BULLET_TYPE GetBulletType(void)const { return bulletType_; };

	//std::vector<std::unique_ptr<PBulletBase>>& GetPBullet(void);


protected:

	// プレイヤー番号
	const int playerNo_;

	std::vector<std::unique_ptr<PBulletBase>> bullets_;
	

	BULLET_TYPE bulletType_;

	PLAYER_TYPE playerType_;
};
