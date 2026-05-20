#pragma once
#include "PBulletBase.h"

class PBulletBig : public PBulletBase
{
public:

	PBulletBig(void);

	~PBulletBig(void)override = default;

	void Load(void)override;

protected:

	void InitTransform(void)override;

	void InitPost(void)override;

	void UpdatePost(void)override;

	void ChangeBulletState(BULLET_STATE _state)override;

	void SetParam(void)override;

private:

	float radiusMax_;
	float scaleMax_;

	float scaleUpTime_;
};

