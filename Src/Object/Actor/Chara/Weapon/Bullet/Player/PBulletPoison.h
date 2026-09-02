#pragma once
#include "PBulletBase.h"

class PBulletPoison : public PBulletBase
{
public:

	static constexpr float SHOT_SPEED_POISON_XZ = 10.0f;
	static constexpr float SHOT_SPEED_POISON_Y = 8.5f;

	PBulletPoison(int _shotType);

	~PBulletPoison(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override;

	void BlastAction(void)override;


protected:

	void InitTransform(void)override;

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;

private:

	float activeTime_;
};

