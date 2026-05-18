
#pragma once

#include <memory>
#include "../../ActorBase.h"

class WeaponBase :
    public ActorBase
{
public:
	struct Bone {
		int id = 0;
		Transform transform;

	};


	WeaponBase(void);
	virtual ~WeaponBase(void)override = default;


	void Update(void)override final;

	

	void Release(void)override;

	virtual void SetBone(int _id, Transform _trans)=0;
	virtual int GetDamage(void) const = 0;

private:


protected:

	

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	float hp_;
	float attackDamage_;	
	

	Bone bone_;

	virtual void DrawPre(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//受けたダメージをボスに伝えるための関数
	virtual float Damage(void)=0;


	virtual void CollisionReserve(void) {};
};

