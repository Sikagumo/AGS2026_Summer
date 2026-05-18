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


	/// @brief コンストラクタ
	/// @param _shotCnt 発射回数
	PBulletBase(void);

	virtual ~PBulletBase(void)override = default;


	void Update(void)override final;

	void Draw(void)override;

	void Release(void)override;

	void CreateShot(const VECTOR& _pos, const VECTOR& _shotDir, const Quaternion& _rot, int _shotCnt);

	bool IsAlive(void)const;

	float GetRadius(void)const { return radius_; }



protected:

	BULLET_STATE bulletState_;

	float radius_;

	float curGravityPow_;

	float shotSpeed_;

	// 発射方向
	VECTOR shotPow_;

	float aliveTime_;

	int shotCnt_;


	std::function<void(void)> updateProc_;


	virtual void InitLoad(void)override = 0;

	virtual void InitTransform(void)override = 0;

	void InitCollider(void)override final;

	void InitAnimation(void)final {};

	void InitPost(void)override;

	virtual void SetParam(void) = 0;

	virtual void UpdatePost(void) = 0;

	virtual void ChangeBulletState(BULLET_STATE _state) = 0;
};

