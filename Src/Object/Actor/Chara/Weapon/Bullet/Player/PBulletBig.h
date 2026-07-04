#pragma once
#include "PBulletBase.h"

class PBulletBig : public PBulletBase
{
public:

	static constexpr float SHOT_SPEED_BIG_XZ = 5.0f;
	static constexpr float SHOT_SPEED_BIG_Y = 5.0f;


	PBulletBig(void);

	~PBulletBig(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override { isScaleUp_ = true; };

	void BlastAction(void)override;


protected:

	void InitTransform(void)override;

	void InitPost(void)override;

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;

private:

	float radiusMax_;
	float scaleMax_;

	bool isScaleUp_;

	float scaleUpTime_;
};

