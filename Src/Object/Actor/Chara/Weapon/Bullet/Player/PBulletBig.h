#pragma once
#include "PBulletBase.h"

class PBulletBig : public PBulletBase
{
public:

	// 弾の発射速度
	static constexpr float SHOT_SPEED_BIG_XZ = 5.0f;
	static constexpr float SHOT_SPEED_BIG_Y = 5.0f;

	/// @brief コンストラクタ
	/// @param _shotType 弾の種類
	PBulletBig(int _shotType);

	~PBulletBig(void)override = default;

	/// @brief 生成した瞬間の処理
	void PreActiveProcess(void)override { isScaleUp_ = true; };


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

	// 初期半径
	static constexpr float RADIUS_BIG = 9.0f;

	// 半径の拡大率
	static constexpr float RADIUS_INCREMENT = 27.5f;

	// 初期スケール
	static constexpr float SCALE_BIG = 1.5f;

	// スケールの拡大率
	static constexpr float SCALE_BIG_INCREMENT = 2.5f;

	// 生存時間
	static constexpr float TIME_ALIVE_BIG = 5.0f;

	// 攻撃力
	static constexpr int POWER_START = 75;

	// 攻撃力上昇値
	static constexpr int POWER_INCREMENT = 100;

	// 拡大完了までの時間
	static constexpr float RADIUS_DURATION = 1.0f;

	// 最大半径値
	float radiusMax_;

	// スケール拡大値
	float scaleMax_;

	// 拡大中か否か
	bool isScaleUp_;

	// 拡大割合
	float scaleUpTime_;
};

