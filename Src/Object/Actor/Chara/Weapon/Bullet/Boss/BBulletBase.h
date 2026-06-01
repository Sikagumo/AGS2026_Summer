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

	virtual void CreateBullets(VECTOR _pos, VECTOR _dir,float _radiuse)=0;

protected:
	bool isAttac_;
	float speed_;
	float radiuse_;
	VECTOR dir_;


	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override=0;
};

