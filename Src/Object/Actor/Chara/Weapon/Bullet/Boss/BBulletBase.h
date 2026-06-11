#pragma once
#include "../../../../ActorBase.h"
class BBulletBase :
    public ActorBase
{
public:

	BBulletBase(void);

	virtual ~BBulletBase(void);

	void Update(void)override final;
	void Draw(void)override final;
	virtual void SetIsAttack(bool _isAttack) = 0;

	virtual void CreateBullets(VECTOR _pos, VECTOR _dir,float _radius)=0;
	bool GetIsAlive(void) { return isAlive_; }

	

protected:

	static constexpr float INIT_ROT = 180.0f;

	bool isAttack_;
	bool isAlive_;
	float speed_;
	float radius_;
	VECTOR dir_;


	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	virtual void DrawPre(void)override=0;
};

