
#pragma once

#include <memory>
#include "../../ActorBase.h"
#include "../../../Collider/ColliderBase.h"

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

	virtual void SetBone(int _id, Transform _trans,ColliderBase::TAG _tag)=0;
	virtual int GetDamage(void) const = 0;
	virtual VECTOR GetPos(void) const = 0;

private:


protected:

	

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	float hp_;
	float attackDamage_;	
	VECTOR localBackPos_;	//当たり判定用の後方の座標
	VECTOR localFrontPos_;	//当たり判定用の前方の座標
	VECTOR localPos_;		//カメラのロックオン用の中央座標
	Bone bone_;
	ColliderBase::TAG tag_;

	virtual void DrawPre(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	//受けたダメージをボスに伝えるための関数
	virtual float Damage(void)=0;


	virtual void CollisionReserve(void) {};
};

