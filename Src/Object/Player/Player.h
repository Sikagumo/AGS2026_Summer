#pragma once
#include "../Actor/CharaBase.h"
#include <memory>
#include <vector>
#include "../Bullet/Player/PBulletBase.h"

class Player : public CharaBase
{
public:

	Player(int _no);

	~Player(void)override = default;

	void Release(void)override;

	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	std::vector<std::unique_ptr<PBulletBase>>& GetPBullet(void);

protected:

	void InitLoad(void)override;
	void InitAnimationPost(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitPost(void)override;


	void UpdateProcess(void);

private:

	// プレイヤー番号
	const int playerNo_;

	std::vector<std::unique_ptr<PBulletBase>> bullet_;
};