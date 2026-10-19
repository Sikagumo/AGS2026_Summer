#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <vector>

class BBulletBase;


class MGBase : public WeaponBase
{
public:

	MGBase(void);
	~MGBase(void) override = default;

	/// @brief 現在攻撃中かどうかを取得する
	/// @return true: 攻撃中 / false: 非攻撃中
	bool IsAttack(void) { return isAttack_; }

protected:

	// 回転制限の定数（度数法）
	static constexpr float LIMIT_MIN_ANGLE = -30.0f;	// 縦回転の最小角度制限（下向き）
	static constexpr float LIMIT_MAX_ANGLE = 20.0f;		// 縦回転の最大角度制限（上向き）
	static constexpr int MAX_BULLET_COUNT = 200;		// 1回の攻撃で発射する総弾数
	static constexpr int MUZZLE_MAX_COUNT = 6;			// 銃口の最大数

	static constexpr float CAPSULE_RADIUS = 20.0f;		// 当たり判定カプセルの半径
	static constexpr float BULLET_HIT_SIZE = 6.0f;		// 発射する弾のヒットサイズ

	// エフェクト・発射ブレ調整用定数
	static constexpr float RAND_DIR_MIN = -5.0f;					// 発射方向のランダムブレ最小幅
	static constexpr float RAND_DIR_MAX = 5.0f;						// 発射方向のランダムブレ最大幅
	static constexpr float EFFECT_ROT_X_OFFSET = -90.0f;			// エフェクト回転のX軸補正角度
	static constexpr VECTOR EFFECT_SCALE = { 10.0f, 10.0f, 10.0f }; // エフェクトの拡大率

	/// @brief 子クラス(MGL/MGR)のUpdateProcessから呼ばれる共通更新関数
	void UpdateCommon(void);

	/// @brief プレイヤーの方向を狙う（回転制限・ブレ付き）
	void LookPlayer(void);

	/// @brief 弾を生成・発射する共通処理
	virtual void CreateBullets(void);

	/// @brief 使用可能な非アクティブ弾を取得（無ければ新規生成）
	/// @return 弾のスマートポインタ
	std::shared_ptr<BBulletBase> GetValidBullet(void);

	// 左右共通で使うメンバ変数
	VECTOR bulletDir_;									// 弾の発射方向ベクトル
	int bulletCount_;									// 残発射数のカウンタ
	VECTOR effectPos_;									// 発射エフェクトの表示座標
	VECTOR muzzlePos_[MUZZLE_MAX_COUNT];				// 各銃口のローカルオフセット座標
	int muzzleCount_;									// 現在使用中の銃口インデックス
	std::vector<std::shared_ptr<BBulletBase>> bullets_;	// 弾オブジェクトの管理プール
	int look_;											// 左右の向き補正用係数（-1または1）
	bool isAttack_;										// 攻撃実行中フラグタ
	

	/// @brief 待機ステート開始処理
	void ChangeStateIdle(void) override;

	/// @brief 攻撃予備動作ステート開始処理
	void ChangeStateAttackWindup(void) override;

	/// @brief 攻撃ステート開始処理
	void ChangeStateAttack(void) override;

	/// @brief 死亡・破壊ステート開始処理
	void ChangeStateEnd(void) override;

	/// @brief 待機ステートの毎フレーム更新
	void UpdateIdle(void) override;

	/// @brief 攻撃予備動作ステートの毎フレーム更新
	void UpdateAttackWindup(void) override;

	/// @brief 攻撃ステートの毎フレーム更新
	void UpdateAttack(void) override;

	/// @brief 攻撃エフェクトの位置・回転の追従と再生制御
	void UpdateAttackEffect(void);

	/// @brief 攻撃サウンドの3D音響設定
	void UpdateAttackSound(void);

};