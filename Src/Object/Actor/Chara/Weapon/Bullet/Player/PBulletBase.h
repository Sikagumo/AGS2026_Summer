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
		INACTIVE,
		SHOT,
		BLAST,
	};


	PBulletBase(int _shotType, bool _isGravity = true);

	virtual ~PBulletBase(void)override = default;

	virtual void Load(void)override = 0;

	void Update(void)override final;

	void Draw(void)override;


	/// @brief 生成処理
	/// @param _pos 発射位置
	/// @param _throwDir 投げる位置の調整角度 
	/// @param _shotCnt 発射数
	/// @param isFinish 最終弾か否か
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt = 0, bool isFinish = false);

	/// @brief 発射処理
	/// @param _shotDir 発射方向(未割当時、投げる方向を利用)
	void Shot(const VECTOR& _shotDir = UtilityMath::VECTOR_ZERO);

	bool IsAlive(void)const;

	bool GetIsVisible(void)const { return isVisible_; };

	/// @brief 当たり判定半径取得 
	float GetRadiusBullet(void)const { return radiusBullet_; }
	float GetRadiusBlast(void)const { return radiusBlast_; }

	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	virtual void PreActiveProcess(void){};

	int GetPowerBullet(void)const { return activePowerBullet_; }
	int GetPowerBlast(void)const { return activePowerBlast_; }

	int GetShotType(void)const { return shotType_; }
	virtual void BlastAction(void) = 0;

	const VECTOR& GetThrowDir(void)const { return throwDir_; };
	const VECTOR& GetThrowPow(void)const { return throwPow_; };

	float GetAliveTime(void)const { return aliveTime_; };
	

protected:

	enum class COLLISION_TYPE
	{
		BULLET = 0, // 弾
		BLAST,		// 爆発
		SUPPORT,	// 回復
	};

	BULLET_STATE bulletState_;

	const bool IS_GRAVITY;

	int shotType_;

	float radiusBullet_;
	float radiusBlast_;

	float curGravityPow_;

	float shotSpeedXZ_;
	float shotSpeedY_;

	// 発射方向
	VECTOR throwPow_;
	VECTOR throwDir_;

	float aliveTime_;

	int shotCnt_;

	bool isVisible_;

	int power_;
	int activePowerBullet_;
	int activePowerBlast_;

	bool isFinish_;

	// 消滅させるか否か
	bool isActiveDestroy_;


	std::function<void(void)> updateProc_;


	void InitTransform(void)override = 0;

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

