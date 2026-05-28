#pragma once
#include "../WeaponBase.h"
class WeaponMP :
	public WeaponBase
{
public:

	WeaponMP();

	~WeaponMP(void)override = default;

	// リソースロード
	void Load(void) override;

	void ReleasePost(void)override;

	/// <summary>
	/// ボーン情報の受け取り用
	/// </summary>
	/// <param name="_id">接続ボーンの番号</param>
	/// <param name="_trans">接続ボーンを持つ対象のトランスフォーム</param>
	/// <param name="_tag">当たり判定登録よタグ</param>
	void SetBone(int _id, Transform _trans, ColliderBase::TAG _tag) override;

	///現在の座標
	VECTOR GetPos(void) const override;
	/// <summary>
	/// ウェポンのダメージ受け取り用
	/// </summary>
	/// <param name="_damage">実数ダメージ</param>
	void SetDamage(int _damage)override { hp_ -= _damage; }
	/// <summary>
	/// ウェポンの生存状態を渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>true=生きている</returns>
	bool GetIsAlive(void)override { return isAlive_; }
	/// <summary>
	/// ウェポンの現在HPを渡すよう
	/// </summary>
	/// <param name=""></param>
	/// <returns>現在HP</returns>
	int GetHp(void)override { return hp_; }
protected:

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

	

	// 前描画
	void DrawPre(void) override;

	void CollisionReserve(void) override {};
};

