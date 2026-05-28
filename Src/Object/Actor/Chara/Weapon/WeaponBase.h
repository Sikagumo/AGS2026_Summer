
#pragma once

#include <memory>
#include "../../ActorBase.h"
#include "../../../Collider/ColliderBase.h"

class WeaponBase :
    public ActorBase
{
public:
	struct Bone {
		int id = 0;
		Transform transform;

	};


	WeaponBase(void);
	virtual ~WeaponBase(void)override = default;


	void Update(void)override final;

	

	void ReleasePost(void)override;
	/// <summary>
	/// ボーン情報とタグの取得
	/// </summary>
	/// <param name="_id">ボーンのナンバー</param>
	/// <param name="_trans">ボーンを持つ相手のトランスフォーム</param>
	/// <param name="_tag">当たり判定用のタグ</param>
	virtual void SetBone(int _id, Transform _trans,ColliderBase::TAG _tag)=0;
	
	/// <summary>
	/// ロックオン用の座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual VECTOR GetPos(void) const = 0;

	
	virtual void SetDamage(int _damage) = 0;
	virtual bool GetIsAlive(void)=0;
	virtual int GetHp(void) = 0;
private:


protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -30.0f;

	static constexpr VECTOR WEAPON_SIZE = { 3.0f,3.0f,3.0f };
	static constexpr float WEAPON_ROT = 180.0f;



	int hp_=500;
	int attackDamage_;	
	bool isAlive_;
	VECTOR movePow_;
	VECTOR jumpPow_;
	// ジャンプ判定
	bool isJump_;
	VECTOR localBackPos_;	//当たり判定用の後方の座標
	VECTOR localFrontPos_;	//当たり判定用の前方の座標
	VECTOR localPos_;		//カメラのロックオン用の中央座標
	Bone bone_;
	ColliderBase::TAG tag_;

	virtual void DrawPre(void)override;

	virtual void UpdateProcess(void) = 0;
	virtual void UpdateProcessPost(void) = 0;

	void CalcGravityPow(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	virtual void CollisionReserve(void) {};
};

