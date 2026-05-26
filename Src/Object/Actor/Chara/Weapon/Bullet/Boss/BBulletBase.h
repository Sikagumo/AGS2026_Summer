#pragma once
#include "../../../../ActorBase.h"
class BBulletBase :
    public ActorBase
{
public:

	BBulletBase();

	virtual ~BBulletBase();

	void Update(void)override final;
	void Draw(void)override final;
	virtual void SetIsAttac(bool _isAttac) = 0;
protected:
	bool isAttac_ = false;

	
	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override=0;
};

