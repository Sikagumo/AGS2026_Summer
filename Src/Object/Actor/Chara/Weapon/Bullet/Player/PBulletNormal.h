#pragma once
#include "PBulletBase.h"

class PBulletNormal : public PBulletBase
{
public:

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

	/// @brief デストラクタ
	~PBulletNormal(void)override = default;


protected:

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	/// @brief 各弾の更新処理
	void UpdatePost(void)override;

	/// @brief 各弾の更新処理
	void SetParam(void)override;

	/// @brief 爆破時の処理
	void BlastAction(void)override;


private:

	// 生存時間
	const float ALIVE_TIME;

	// スケール
	const float SCALE;
};

