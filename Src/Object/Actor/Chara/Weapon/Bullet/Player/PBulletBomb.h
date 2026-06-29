#pragma once
#include "PBulletBase.h"

class PBulletBomb : public PBulletBase
{
public:

	PBulletBomb(void);

	~PBulletBomb(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override;

	void BlastAction(void)override;


protected:

	void InitTransform(void)override;

	void InitPost(void)override;

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;
};

