#pragma once
#include "PBulletBase.h"

class PBulletBomb : public PBulletBase
{
public:

	static constexpr float SHOT_SPEED_BOMB_XZ = 10.0f;
	static constexpr float SHOT_SPEED_BOMB_Y = 7.5f;

	PBulletBomb(int _shotType);

	~PBulletBomb(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override;

	void BlastAction(void)override;


protected:

	void InitTransform(void)override;

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;
};

