#pragma once
#include <DxLib.h>
#include <memory>
#include <array>
#include <functional>
#include "../CharaBase.h"
#include "../../../Common/Transform.h";


class WeaponMGL;
class WeaponMGR;
class WeaponCannon;
class WeaponMP;
class WeaponRG;
class BBulletWave;

class Boss : public CharaBase
{
public:
	///ステートパターン
	enum class STATE
	{
		IDLE,
		ATTACK,
		JUMP,
		END,
	};

	///ウェポンの接続ボーンの名前
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

	///各ボーンの情報
	struct Bone {
		int id = 0;
		Transform transform;

	};

	Boss(void);
	~Boss(void) override;

	// リソースロード
	void Load(void) override;

	//ゲット・セット
	
	/// <summary>
	/// ボスの現在座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	VECTOR GetBossPos(void) const;

	//各武器のダメージ受け取り用関数
	//ガトリング
	void SetWeaponMGLDamage(int _damage);
	void SetWeaponMGRDamage(int _damage);
	//ミサイルポッド
	void SetWeaponMPLDamage(int _damage);
	void SetWeaponMPRDamage(int _damage);
	//キャノン
	void SetWeaponCannonLDamage(int _damage);
	void SetWeaponCannonRDamage(int _damage);
	//レールガン
	void SetWeaponRGDamage(int _damage);
	//ボス本体へのダメージ受け取り用
	void SetBossDamage(int _damage);


	void UpdateCollision(void);
	/// <summary>
	/// ボス現在HP
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	int GetHP(void) { return hp_; }


private:
	//bossの大きさ
	static constexpr VECTOR BOSS_SIZE = { 3.0f, 3.0f, 3.0f };
	//bossの初期座標
	static constexpr VECTOR BOSS_INIT_POS= { 0.0f, 0.0f, 500.0f };
	//回転
	static constexpr float INIT_ROT = 180.0f;
	//ジャンプ力
	static constexpr float POW_JUMP_INIT = 3000.0f;

	//当たり判定の座標
	//ライン
	static constexpr VECTOR COL_LINE_START_POS = { 0.0f,60.0f,0.0f };
	static constexpr VECTOR COL_LINE_END_POS = { 0.0f,-1.0f,0.0f };
	//カプセル
	static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f,130.0f,0.0f };
	static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f,80.0f,0.0f };
	static constexpr float COL_CAPSULE_END_RADIUS = 80.0f;

	//ボーンの番号
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

	//ボス本体の各トランスフォーム
	Transform transformFeet_;
	Transform transformBody_;
	Transform transformFeetCar_;
	Transform transformWheelBack_;
	Transform transformWheelFront_;

	//ステータス
	int hp_;						//HP
	std::array<Bone,7> boneId_;		//各ボーン
	BONE_NAME boneName_;			//ボーンの名前

	//攻撃関連
	int jumpCount_;
	int attackCount_;



	//武器のポインター宣言
	std::unique_ptr<WeaponMGL> weaponMGL_;
	std::unique_ptr<WeaponMGR> weaponMGR_;
	std::unique_ptr<WeaponCannon> weaponCannonL_;
	std::unique_ptr<WeaponCannon> weaponCannonR_;
	std::unique_ptr<WeaponMP> weaponMPL_;
	std::unique_ptr<WeaponMP> weaponMPR_;
	std::unique_ptr<WeaponRG> weaponRG_;
	std::unique_ptr< BBulletWave> wave_;
	

	///ボーン初期化
	void BoneParam(void);
	///ボーンアプデ
	void BossTransformUpdate(void);

	/// 状態
	STATE state_;
	/// 状態管理
	int stateBase_;

	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;
	// 状態遷移
	void ChangeState(STATE _state);
	// 状態遷移
	void ChangeState(int state);
	void ChangeStateIdle(void);
	void ChangeStateAttack(void);
	void ChangeStateJump(void);
	void ChangeStateEnd(void);
	// 更新系
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;
	void UpdateIdle(void);
	void UpdateAttack(void);
	void UpdateJump(void);
	void UpdateEnd(void);
	 


protected:

	/// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	/// 衝突判定の初期化
	void InitCollider(void) override;

	/// アニメーションの初期化
	void InitAnimation(void) override;

	

	///初期化後の個別処理
	void InitPost(void) override;

	///アップデート関数
	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

	

	// 前描画
	void DrawPre(void) override;

	void ReleasePost(void) override;

	void CollisionReserve(void) override  {};

	///ウェポンセット呼び出し関数まとめ用
	void WeaponSet(void);

	///ウェポンロード呼び出し関数まとめ用
	void WeaponLoad(void);

	///ウェポンイニット呼び出し関数まとめ用
	void WeaponInit(void);

	///ウェポンアップデート呼び出し関数まとめ用
	void WeaponUpdate(void);

	///ウェポンドロー呼び出し関数まとめ用
	void WeaponDraw(void);
	
	
};

