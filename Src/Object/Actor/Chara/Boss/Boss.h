#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include "../CharaBase.h"
#include "../../../Common/Transform.h";


class WeaponMGL;
class WeaponMGR;
class WeaponCannon;
class WeaponMP;
class WeaponRG;


class Boss : public CharaBase
{
public:
	enum class BONE_NAME
	{
		WEAPON_JOINT_MGL_L=0,
		WEAPON_JOINT_MGL_R,
		WEAPON_JOINT_CANNON_L,
		WEAPON_JOINT_CANNON_R,
		WEAPON_JOINT_MP_L,
		WEAPON_JOINT_MP_R,
		WEAPON_JOINT_RG,
		MAX,
	};
	struct Bone {
		int id = 0;
		Transform transform;

	};

	Boss(void);
	~Boss(void) override;

	// リソースロード
	void Load(void) override;

	void Release(void) override;

	VECTOR GetBossPos(void) const;

private:
	//bossの大きさ
	static constexpr VECTOR BOSS_SIZE = { 3.0f, 3.0f, 3.0f };
	//bossの初期座標
	static constexpr VECTOR BOSS_INIT_POS= { 0.0f, 0.0f, 0.0f };
	//回転
	static constexpr float INIT_ROT = 180.0f;
	static constexpr int JOINT_FEET_BODY = 12;
	static constexpr int JOINT_CAR_BODY = 4;
	static constexpr int JOINT_CAR_WHEEL_FRONT_L = 6;
	static constexpr int JOINT_CAR_WHEEL_FRONT_R = 16;
	static constexpr int JOINT_CAR_WHEEL_BACK_FRONT_L = 8;
	static constexpr int JOINT_CAR_WHEEL_BACK_FRONT_R = JOINT_FEET_BODY;
	static constexpr int JOINT_CAP_WHEEL_BACK_L = 10;
	static constexpr int JOINT_CAP_WHEEL_BACK_R = 14;
	static constexpr int JOINT_WAEAPON_MG_L = JOINT_CAR_BODY;
	static constexpr int JOINT_WAEAPON_MG_R = 10;
	static constexpr int JOINT_WAEAPON_CANNON_L = 6;
	static constexpr int JOINT_WAEAPON_CANNON_R = JOINT_FEET_BODY;
	static constexpr int JOINT_WAEAPON_MP_L = JOINT_CAR_WHEEL_BACK_FRONT_L;
	static constexpr int JOINT_WAEAPON_MP_R = JOINT_CAP_WHEEL_BACK_R;
	static constexpr int JOINT_WAEAPON_RG = JOINT_CAR_WHEEL_FRONT_R;

	Transform transformFeet_;
	Transform transformBody_;
	Transform transformFeetCar_;
	Transform transformWheelBack_;
	Transform transformWheelFront_;


	float hp_;
	float attackDelay_;
	std::array<Bone,7> boneId_;
	BONE_NAME boneName_;

	std::unique_ptr<WeaponMGL> weaponMGL_;
	std::unique_ptr<WeaponMGR> weaponMGR_;
	std::unique_ptr<WeaponCannon> weaponCannonL_;
	std::unique_ptr<WeaponCannon> weaponCannonR_;
	std::unique_ptr<WeaponMP> weaponMPL_;
	std::unique_ptr<WeaponMP> weaponMPR_;
	std::unique_ptr<WeaponRG> weaponRG_;

	void BoneParam(void);
	void BossTransformUpdate(void);

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

	void CollisionReserve(void) override  {};


};

