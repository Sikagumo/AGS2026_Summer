#pragma once
#include "../../../../ActorBase.h"
#include <DxLib.h>
#include <functional>
#include "../../../../../../Common/Quaternion.h"
#include "../../../../../../Utility/UtilityMath.h"

class PBulletBase : public ActorBase
{
public:

	enum class BULLET_STATE
	{
		INACTIVE, // –³Œøó‘Ô
		SHOT,     // ”­Ëó‘Ô
		BLAST,    // ’…’eó‘Ô
	};

	/// @brief ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	/// @param _shotType ’e‚Ìí—Ş
	/// @param _isGravity d—Í‚ğ‚Â‚¯‚é‚©”Û(default:true)
	PBulletBase(int _shotType, bool _isGravity = true);

	virtual ~PBulletBase(void)override = default;

	virtual void Load(void)override = 0;

	void Update(void)override final;

	void Draw(void)override;


	/// @brief ¶¬ˆ—
	/// @param _pos ”­ËˆÊ’u
	/// @param _throwDir “Š‚°‚éˆÊ’u‚Ì’²®Šp“x 
	/// @param _shotCnt ”­Ë”
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt = 0);

	/// @brief ”­Ëˆ—
	/// @param _shotDir ”­Ë•ûŒü(–¢Š„“–A“Š‚°‚é•ûŒü‚ğ—˜—p)
	void Shot(const VECTOR& _shotDir = UtilityMath::VECTOR_ZERO);

	/// @brief ’e‚ª¶‘¶’†‚©”Û‚©
	bool IsAlive(void)const;

	/// @brief ’e‚ª•`‰æ‚µ‚Ä‚¢‚é‚©”Û‚©
	bool GetIsVisible(void)const { return isVisible_; };

	/// @brief ’e‚Ì“–‚½‚è”»’è”¼Œaæ“¾
	float GetRadiusBullet(void)const { return radiusBullet_; }

	/// @brief ’…’e‚Ì”š”­‚Ì“–‚½‚è”»’è”¼Œaæ“¾ 
	float GetRadiusBlast(void)const { return radiusBlast_; }


	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	virtual void PreActiveProcess(void){};

	/// @brief ’e©‘Ì‚ÌUŒ‚—Í
	int GetPowerBullet(void)const { return activePowerBullet_; }

	/// @brief ’…’e‚Ì”š”­‚ÌUŒ‚—Í
	int GetPowerBlast(void)const { return activePowerBlast_; }

	/// @brief ’e‚Ìí—Ş‚ğæ“¾
	int GetShotType(void)const { return shotType_; }

	const VECTOR& GetThrowDir(void)const { return throwDir_; };
	const VECTOR& GetThrowPow(void)const { return throwPow_; };

	float GetAliveTime(void)const { return aliveTime_; };
	

protected:

	enum class COLLISION_TYPE
	{
		BULLET = 0, // ’e
		BLAST,		// ”š”­
		SUPPORT,	// ‰ñ•œ
	};

	BULLET_STATE bulletState_;

	const bool IS_GRAVITY;

	int shotType_;

	float radiusBullet_;
	float radiusBlast_;

	float curGravityPow_;

	float shotSpeedXZ_;
	float shotSpeedY_;

	// ”­Ë•ûŒü
	VECTOR throwPow_;
	VECTOR throwDir_;

	float aliveTime_;

	int shotCnt_;

	bool isVisible_;

	int power_;
	int activePowerBullet_;
	int activePowerBlast_;

	// Á–Å‚³‚¹‚é‚©”Û‚©
	bool isActiveDestroy_;


	std::function<void(void)> updateProc_;


	void InitTransform(void)override = 0;

	void InitCollider(void)override final;

	void InitAnimation(void)final {};

	void InitPost(void)override;

	virtual void SetParam(void) = 0;

	virtual void UpdatePost(void) = 0;

	virtual void BlastAction(void) = 0;

	void ReleasePost(void)override;

	virtual void ChangeBulletStateProc(void) = 0;


private:

	void ChangeBulletState(BULLET_STATE _state);
};

