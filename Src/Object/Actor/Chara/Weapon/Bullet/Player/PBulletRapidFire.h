#pragma once
#include "PBulletBase.h"
class PBulletRapidFire : public PBulletBase
{
public:

	PBulletRapidFire(void);

	~PBulletRapidFire(void)override = default;

	void Load(void)override;

	void BlastAction(void)override;


protected:

	void InitTransform(void)override;

	void InitPost(void)override;

	void SetParam(void)override;

	void ChangeBulletStateProc(void)override;
};

