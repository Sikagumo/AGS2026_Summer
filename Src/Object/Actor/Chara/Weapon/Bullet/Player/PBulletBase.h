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

	virtual void Load(void)override = 0;

	void Update(void)override final;

	void Draw(void)override;


	/// @brief 生成処理
	/// @param _pos 
	/// @param _throwDir 
	/// @param _shotCnt 
	/// @param isFinish 最終弾か否か
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt, bool isFinish);

	void Shot(const VECTOR& _shotDir);

	bool IsAlive(void)const;

	float GetRadius(void)const { return radius_; }

	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	virtual void PreActiveProcess(void){};


protected:

	BULLET_STATE bulletState_;

	float radius_;

	float curGravityPow_;

	float shotSpeedXZ_;
	float shotSpeedY_;

	// 発射方向
	VECTOR shotPow_;

	float aliveTime_;

	int shotCnt_;

	bool isVisible_;

	int power_;

	bool isFinish_;


	std::function<void(void)> updateProc_;


	virtual void InitTransform(void)override = 0;

	void InitCollider(void)override final;

	void InitAnimation(void)final {};

	void InitPost(void)override;

	virtual void SetParam(void) = 0;

	virtual void UpdatePost(void) = 0;

	void ReleasePost(void)override;

	virtual void BlastAction(void);

	virtual void ChangeBulletState(BULLET_STATE _state) = 0;
};

