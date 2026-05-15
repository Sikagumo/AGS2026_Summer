#pragma once
#include "../CharaBase.h"
#include <memory>
#include <vector>
#include "../Weapon/Bullet/Player/PBulletBase.h"

class PlayerBase : public CharaBase
{
public:

	enum class PLAYER_TYPE
	{
		NONE = -1,
		INTERFERING, // 妨害
		BIG,
		RAPID_FIRE, // 連射
		RECOVERY,	// 回復

		MAX,
	};

	enum class PLAYER_STATE
	{
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
	};

	PlayerBase(int _playerNo, PLAYER_TYPE _playerType);

	virtual ~PlayerBase(void)override = default;

	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	//std::vector<std::unique_ptr<PBulletBase>>& GetPBullet(void);


protected:

	// プレイヤー番号
	const int playerNo_;

	std::vector<std::unique_ptr<PBulletBase>> bullets_;
	

	PLAYER_TYPE playerType_;
};
