#pragma once
#include "PBulletBase.h"
class PBulletRapidFire : public PBulletBase
{
public:

	PBulletRapidFire(void);

	~PBulletRapidFire(void)override = default;


protected:

	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitPost(void)override;

	void SetParam(void)override;

};

