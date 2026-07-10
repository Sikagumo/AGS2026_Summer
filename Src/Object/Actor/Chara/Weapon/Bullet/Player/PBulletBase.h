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


	PBulletBase(bool _isGravity = true);

	virtual ~PBulletBase(void)override = default;

	virtual void Load(void)override = 0;

	void Update(void)override final;

	void Draw(void)override;


	/// @brief ê∂ê¨èàóù
	/// @param _pos î≠éÀà íu
	/// @param _throwDir ìäÇ∞ÇÈà íuÇÃí≤êÆäpìx 
	/// @param _shotCnt î≠éÀêî
	/// @param isFinish ç≈èIíeÇ©î€Ç©
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt = 0, bool isFinish = false);

	void Shot(const VECTOR& _shotDir);

	bool IsAlive(void)const;

	bool GetIsVisible(void)const { return isVisible_; };

	/// @brief ìñÇΩÇËîªíËîºåaéÊìæ 
	float GetRadiusBullet(void)const { return radiusBullet_; }
	float GetRadiusBlast(void)const { return radiusBlast_; }

	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	virtual void PreActiveProcess(void){};

	//int GetPowerBullet(void)const { return activePowerBullet_; }
	//int GetPowerBlast(void)const { return activePowerBlast_; }

	virtual void BlastAction(void) = 0;


protected:

	enum class COLLISION_TYPE
	{
		BULLET = 0, // íe
		BLAST,		// îöî≠
		SUPPORT,	// âÒïú
	};

	BULLET_STATE bulletState_;

	const bool IS_GRAVITY;

	float radiusBullet_;
	float radiusBlast_;

	float curGravityPow_;

	float shotSpeedXZ_;
	float shotSpeedY_;

	// î≠éÀï˚å¸
	VECTOR shotPow_;

	float aliveTime_;

	int shotCnt_;

	bool isVisible_;

	int power_;
	int activePowerBullet_;
	int activePowerBlast_;

	bool isFinish_;

	// è¡ñ≈Ç≥ÇπÇÈÇ©î€Ç©
	bool isActiveDestroy_;


	std::function<void(void)> updateProc_;


	virtual void InitTransform(void)override = 0;

	void InitCollider(void)override final;

	void InitAnimation(void)final {};

	void InitPost(void)override;

	virtual void SetParam(void) = 0;

	virtual void UpdatePost(void) = 0;

	void ReleasePost(void)override;

	virtual void ChangeBulletStateProc(void) = 0;


private:

	void ChangeBulletState(BULLET_STATE _state);
};

