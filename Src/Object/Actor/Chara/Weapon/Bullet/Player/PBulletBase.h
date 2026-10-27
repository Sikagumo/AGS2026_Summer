#pragma once
#include "../../../../ActorBase.h"
#include <DxLib.h>
#include <functional>
#include "../../../../../../Common/Quaternion.h"
#include "../../../../../Collider/ColliderBase.h"
#include "../../../../../../Utility/UtilityMath.h"

class PBulletBase : public ActorBase
{
public:

	enum class BULLET_STATE
	{
		INACTIVE, // 無効状態
		SHOT,     // 発射状態
		BLAST,    // 着弾状態
	};

	/// @brief コンストラクタ
	/// @param _shotType 弾の種類
	/// @param _isGravity 重力をつけるか否(default:true)
	PBulletBase(int _shotType, bool _isGravity = true);

	/// @brief デストラクタ
	virtual ~PBulletBase(void)override;

	/// @brief リソース読み込み処理
	virtual void Load(void)override {};

	/// @brief 更新処理
	void Update(void)override final;

	/// @brief 描画処理
	void Draw(void)override;

	/// @brief 生成処理
	/// @param _pos 発射位置
	/// @param _throwDir 投げる位置の調整角度 
	/// @param _shotCnt 発射数
	void Create(const VECTOR& _pos, const VECTOR& _throwDir, int _shotCnt = 0);

	/// @brief 発射処理
	/// @param _shotDir 発射方向(未割当時、投げる方向を利用)
	void Shot(const VECTOR& _shotDir = UtilityMath::VECTOR_ZERO);

	/// @brief 弾の位置追従処理
	/// @param _pos 追従位置
	/// @param _offsetDir 弾の方向
	void SetFollow(const VECTOR& _pos, const VECTOR& _offsetDir);

	/// @brief 生成した瞬間の処理
	virtual void PreActiveProcess(void) {};

	/// @brief 弾が生存中か否か
	bool IsAlive(void)const;

	/// @brief 弾が描画しているか否か
	bool GetIsVisible(void)const { return isVisible_; };

	/// @brief 弾の当たり判定半径取得
	float GetRadiusBullet(void)const { return radiusBullet_; };

	/// @brief 着弾時の爆発の当たり判定半径取得 
	float GetRadiusBlast(void)const { return radiusBlast_; };

	/// @brief 弾自体の攻撃力
	int GetPowerBullet(void)const { return activePowerBullet_; };

	/// @brief 着弾時の爆発の攻撃力
	int GetPowerBlast(void)const { return activePowerBlast_; };

	/// @brief 弾の種類を取得
	int GetShotType(void)const { return shotType_; };

	/// @brief 弾状態を取得
	BULLET_STATE GetBulletState(void)const { return bulletState_; };

	/// @brief 投げる方向 取得
	const VECTOR& GetThrowDir(void)const { return throwDir_; };

	/// @brief 生存時間 取得
	float GetAliveTime(void)const { return aliveTime_; };
	

protected:

	/// @brief 弾の当たり判定の種類
	enum class COLLISION_TYPE
	{
		BULLET = 0, // 弾
		BLAST,		// 爆発範囲
		SUPPORT,	// 回復範囲
	};

	// 弾の球の分割数
	static constexpr int SPHERE_DIV = 16;

	// 弾の種類
	int shotType_;

	// 弾に重力を持たせるか否か
	const bool IS_GRAVITY;

	// 弾の状態
	BULLET_STATE bulletState_;

	// 弾の当たり判定の半径
	float radiusBullet_;
	float radiusBlast_;

	// 重力量
	float curGravityPow_;

	// 発射速度
	float shotSpeedXZ_;
	float shotSpeedY_;

	// 発射方向
	VECTOR throwPow_;
	VECTOR throwDir_;

	// 生存時間
	float aliveTime_;

	// 発射数
	int shotCnt_;

	// 弾を表示するか否か
	bool isVisible_;

	// 弾の攻撃力
	int power_;
	int activePowerBullet_;
	int activePowerBlast_;

	// 消滅させるか否か
	bool isActiveDestroy_;

	/// @brief 大きさ、回転、座標の初期化
	void InitTransform(void)override = 0;

	/// @brief 衝突判定の初期化
	void InitCollider(void)override;

	/// @brief アニメーションの初期化
	void InitAnimation(void)final {};

	/// @brief 初期化後の個別処理
	void InitPost(void)override final;

	/// @brief 弾のパラメータ割り当て処理
	virtual void SetParam(void) = 0;

	/// @brief 各弾の更新処理
	virtual void UpdatePost(void) = 0;

	/// @brief 爆破時の処理
	virtual void BlastAction(void) = 0;

	/// @brief 指定タグの当たり判定の有効/無効を切り替え
	/// @param _tag タグ
	/// @param _isActive 有効にするか否か
	void SetColliderActive(ColliderBase::TAG _tag, bool _isActive);

	/// @brief 指定種類の当たり判定(先頭)の半径を設定
	/// @param _type 指定の当たり判定
	/// @param _radius 半径
	void SetColliderRadius(COLLISION_TYPE _type, float _radius);

	/// @brief 着弾状態へ移行する
	void BeginBlast(void);

	/// @brief 爆発の当たり判定を指定半径で有効化する
	void ActivateBlastCollider(float _radius);


private:

	/// @brief 発射中の移動・生存時間の更新処理
	void UpdateShot(void);

	/// @brief 着弾対象に衝突しているか否か
	bool IsHitTarget(void);

	/// @brief ステージ衝突・下向き発射による着弾判定
	bool IsHitTerrain(void);
};

