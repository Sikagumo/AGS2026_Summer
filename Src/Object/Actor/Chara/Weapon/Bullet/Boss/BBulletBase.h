#pragma once
#include "../../../../ActorBase.h"

/// @brief エネミー・ボス弾の基底クラス
class BBulletBase : public ActorBase
{
public:

	/// @brief コンストラクタ
	BBulletBase(void);

	/// @brief デストラクタ
	virtual ~BBulletBase(void);

	/// @brief フレーム毎の更新処理
	void Update(void) override final;

	/// @brief 描画処理
	void Draw(void) override final;

	/// @brief 攻撃状態を設定する
	/// @param _isAttack true: 攻撃中 / false: 非攻撃中
	virtual void SetIsAttack(bool _isAttack) = 0;

	/// @brief 弾を生成・初期化する
	/// @param _pos 生成座標
	/// @param _dir 飛翔方向
	/// @param _radius 当たり判定半径
	virtual void CreateBullets(VECTOR _pos, VECTOR _dir, float _radius) = 0;

	/// @brief 生存状態を取得する
	/// @return true: 生きている / false: 死亡
	bool GetIsAlive(void) { return isAlive_; }

	/// @brief プレイヤーの目標座標を設定する
	/// @param _pos プレイヤー座標
	virtual void SetPlayerPos(VECTOR _pos) = 0;

	/// @brief 弾の最高高度の上限値を設定する
	/// @param _pos 上限高度
	virtual void SetUpMaxPos(float _pos) = 0;

	/// @brief 発射元のトランスフォーム情報を設定する
	/// @param _trans 発射元のトランスフォーム
	virtual void SetTransform(const Transform& _trans) { weaponTrans_ = _trans; }

protected:

	static constexpr float INIT_ROT = 180.0f;		// 初期Y軸回転角度（度）

	bool isAttack_;									// 攻撃フラグ
	bool isAlive_;									// 生存フラグ
	float speed_;									// 飛翔速度
	float radius_;									// 当たり判定半径
	VECTOR dir_;									// 飛翔方向ベクトル

	Transform weaponTrans_;							// 発射元武器のトランスフォーム

	/// @brief 各弾ごとの固有更新処理
	virtual void UpdateProcess(void) = 0;

	/// @brief 更新処理後の個別処理
	virtual void UpdateProcessPost(void) = 0;

	/// @brief 描画前処理
	virtual void DrawPre(void) override = 0;
};