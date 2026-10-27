#pragma once
#include "PBulletBase.h"

class PBulletRecovery : public PBulletBase
{
public:

	// 弾の発射速度
	static constexpr float SHOT_SPEED_RECOVERY_XZ = 5.0f;
	static constexpr float SHOT_SPEED_RECOVERY_Y = 5.0f;

	// 回復割合(0.0～1.0)
	static constexpr float RECOVERY_RATE = 0.175f;


	PBulletRecovery(int _shotType);

	~PBulletRecovery(void)override = default;


protected:

	/// @brief 衝突判定の初期化(回復範囲の判定を追加)
	void InitCollider(void)override;

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void)override;

	/// @brief 各弾の更新処理
	void UpdatePost(void)override;

	/// @brief 各弾の更新処理
	void SetParam(void)override;

	/// @brief 爆破時の処理
	void BlastAction(void)override;


private:

	// 半径
	static constexpr float RADIUS_BULLET = 10.0f;
	static constexpr float RADIUS_RECOVERY = 250.0f;

	// スケール
	static constexpr float SCALE_RECOVERY = 1.5f;

	// 生存時間
	static constexpr float TIME_ALIVE_RECOVERY = 15.0f;

	// 回復有効時間
	static constexpr float ACTIVE_TIME = 1.5f;

	// 回復エフェクトのスケール・再生速度
	static constexpr float RECOVERY_EFFECT_SCALE = 35.0f;
	static constexpr float RECOVERY_EFFECT_SPEED = 1.0f;

	// 回復の継続時間
	float activeTimeRecovery_;
};

