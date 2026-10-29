#pragma once
#include "PBulletBase.h"

class PBulletBomb : public PBulletBase
{
public:

	// 弾の発射速度
	static constexpr float SHOT_SPEED_BOMB_XZ = 10.0f;
	static constexpr float SHOT_SPEED_BOMB_Y = 7.5f;

	/// @brief コンストラクタ
	/// @param _shotType 弾の種類
	PBulletBomb(int _shotType);

	/// @brief デストラクタ
	~PBulletBomb(void)override = default;


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
	static constexpr float RADIUS_BULLET = 9.0f;
	static constexpr float RADIUS_BLAST = 65.0f;

	// スケール
	static constexpr float SCALE_BOMB = 1.0f;

	// 生存時間
	static constexpr float TIME_ALIVE_BOMB = 10.0f;

	// 攻撃力
	static constexpr int POWER_BOMB = 65;

	// エフェクトのスケール
	static constexpr float BLAST_EFFECT_SCALE = 35.0f;

	// エフェクトの再生速度
	static constexpr float BLAST_EFFECT_SPEED = 1.0f;
};

