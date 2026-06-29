#pragma once
#include "PBulletBase.h"

class PBulletInterfere : public PBulletBase
{
public:

	PBulletInterfere(void);

	~PBulletInterfere(void)override = default;

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

	float timeEndActive_;

	bool isScaleUp_;

	float scaleUpTime_;
};

