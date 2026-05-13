#pragma once
#include "../../ActorBase.h"

class WeaponBase :
    public ActorBase
{
public:
	WeaponBase(void);
	virtual ~WeaponBase(void)override = default;


	void Update(void)override final;

	void Release(void)override;

private:


protected:

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	float hp_;
	float attackDamage_;	
	int modelId_;
	int jointNo_;

	virtual void DrawPre(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//受けたダメージをボスに伝えるための関数
	virtual float Damage(void)=0;


	virtual void CollisionReserve(void) {};
};

