#pragma once
#include "PBulletBase.h"

class PBulletRecovery : public PBulletBase
{
public:

	static constexpr float SHOT_SPEED_RECOVERY_XZ = 5.0f;
	static constexpr float SHOT_SPEED_RECOVERY_Y = 5.0f;

	// âÒïúäÑçá(0.0Å`1.0)
	static constexpr float RECOVERY_RATE = 0.125f;


	PBulletRecovery(int _shotType);

	~PBulletRecovery(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override;

	void BlastAction(void)override;


protected:

	void InitPost(void)override;

	void InitTransform(void) override;

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;

private:

	float activeTime_;
};

