#pragma once
#include "../Actor/CharaBase.h"
#include <memory>
#include <vector>
#include "../Bullet/Player/PBulletBase.h"

class PlayerBase : public CharaBase
{
public:

	PlayerBase(int _playerNo);

	virtual ~PlayerBase(void)override = default;

	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	//std::vector<std::unique_ptr<PBulletBase>>& GetPBullet(void);


protected:

	// プレイヤー番号
	const int playerNo_;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 5.0f;

	std::vector<std::unique_ptr<PBulletBase>> bullet_;
};

