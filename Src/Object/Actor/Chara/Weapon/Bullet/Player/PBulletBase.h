#pragma once
#include "../../../../ActorBase.h"
#include <DxLib.h>
#include <functional>
#include "../../../../../../Common/Quaternion.h"

class PBulletBase : public ActorBase
{
public:

	enum class BULLET_STATE
	{
		INACTIVE,
		SHOT,
		BLAST,
	};


	PBulletBase(void);

	virtual ~PBulletBase(void)override = default;

	virtual void Load(void)override = 0;

	void Update(void)override final;

	void Draw(void)override;


	/// @brief ê∂ê¨èàóù
	/// @param _pos 
	/// @param _throwDir 
	/// @param _shotCnt 
	/// @param isFinish ç≈èIíeÇ©î€Ç©
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt, bool isFinish);

	void Shot(const VECTOR& _shotDir);

	bool IsAlive(void)const;

	bool GetIsVisible(void)const { return isVisible_; };

	float GetRadius(void)const { return radius_; }

	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	virtual void PreActiveProcess(void){};

	int GetPower(void) { return activePower_; }

	virtual void BlastAction(void);


protected:

	BULLET_STATE bulletState_;

	float radius_;

	float curGravityPow_;

	float shotSpeedXZ_;
	float shotSpeedY_;

	// î≠éÀï˚å¸
	VECTOR shotPow_;

	float aliveTime_;

	int shotCnt_;

	bool isVisible_;

	int power_;
	int activePower_;

	bool isFinish_;


	std::function<void(void)> updateProc_;


	virtual void InitTransform(void)override = 0;

	void InitCollider(void)override final;

	void InitAnimation(void)final {};

	void InitPost(void)override;

	virtual void SetParam(void) = 0;

	virtual void UpdatePost(void) = 0;

	void ReleasePost(void)override;

	virtual void ChangeBulletState(BULLET_STATE _state) = 0;
};

