#pragma once
#include "PBulletBase.h"

class PBulletPoison : public PBulletBase
{
public:

	// 弾の発射速度
	static constexpr float SHOT_SPEED_POISON_XZ = 10.0f;
	static constexpr float SHOT_SPEED_POISON_Y = 6.75f;

	/// @brief コンストラクタ
	/// @param _shotType 弾の種類
	PBulletPoison(int _shotType);

	/// @brief デストラクタ
	~PBulletPoison(void)override;

	/// @brief リソース読み込み処理
	void Load(void)override;


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

	// 半径
	static constexpr float RADIUS_BULLET = 5.0f;
	static constexpr float RADIUS_POISON = 100.0f;

	// スケール
	static constexpr float SCALE_POISON = 1.0f;

	// 生存時間
	static constexpr float TIME_ALIVE_POISON = 7.5f;

	// 攻撃力
	static constexpr int POWER_POISON = 1;

	// 有効時間
	static constexpr float ACTIVE_TIME = 2.5f;

	// 毒エフェクトのスケール・再生速度
	static constexpr float POISON_EFFECT_SCALE = 35.0f;
	static constexpr float POISON_EFFECT_SPEED = 1.0f;

	// 毒の継続時間
	float activeTimePoison_;
};

