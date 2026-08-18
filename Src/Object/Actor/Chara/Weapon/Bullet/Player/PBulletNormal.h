#pragma once
#include "PBulletBase.h"

class PBulletNormal : public PBulletBase
{
public:

	static constexpr float SHOT_SPEED_BIG_XZ = 5.0f;
	static constexpr float SHOT_SPEED_BIG_Y = 5.0f;


	/// @brief コンストラクタ
	/// @param _scale モデルスケール
	/// @param _radius 弾の半径
	/// @param _power 攻撃力
	/// @param _shotSpeedXZ 平面の移動速度
	/// @param _shotSpeedY 上方向の移動速度
	/// @param _aliveTime 生存時間
	/// @param _isActiveGravity 重力を付与するか否か
	PBulletNormal(float _scale, float _radius, int _power
				, float _shotSpeedXZ, float _shotSpeedY, float _aliveTime
				, int _shotType, bool _isActiveGravity = true);

	~PBulletNormal(void)override = default;

	void Load(void)override;

	void PreActiveProcess(void)override;

	void BlastAction(void)override;


protected:

	void UpdatePost(void)override;

	void ChangeBulletStateProc(void)override;

	void SetParam(void)override;


private:

	const float ALIVE_TIME;
	const float SCALE;
};

