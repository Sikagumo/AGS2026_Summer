#pragma once
#include "../../Actor/ActorBase.h"

class PBulletBase : public ActorBase
{
public:

	PBulletBase(void);

	virtual ~PBulletBase(void)override = default;


	void Update(void)override;

	void Draw(void)override;

	void Release(void)override;


protected:

	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitCollider(void)override;

	void InitAnimation(void)final {};

	void InitPost(void)override;

private:

	

};

