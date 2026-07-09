#pragma once
#include <vector>

class DamageController
{

public:

	///武器ダメージの登録用
	enum class BOSS_WEPO_TYPE
	{
		NONE,
		MG,
		PRESSWAVE,
		CANNON,
		MP,
		RG,
		RODE,
		
	};

	struct BOSS_DATA
	{
		BOSS_WEPO_TYPE type;
		float attack;
	};



	DamageController(void);
	~DamageController(void);

	void Update(void);


	/// @brief ボスが受けるダメージ
	int GetBossDamage(void) { return bossDamage_;}

	/// @brief プレイヤーが受けるダメージ
	int GetPlayerDamage(void) { return playerDamage_; }

	/// @brief 右マシンガンが受けるダメージ
	int GetWeaponMGRDamage(void) { return weaponMGRDamage_; }

	/// @brief 左マシンガンが受けるダメージ
	int GetWeaponMGLDamage(void) { return weaponMGLDamage_; }

	/// @brief 右ミサイルポッドが受けるダメージ
	int GetWeaponMPRDamage(void) { return weaponMPRDamage_; }

	/// @brief 左ミサイルポッドが受けるダメージ
	int GetWeaponMPLDamage(void) { return weaponMPLDamage_; }

	/// @brief レールガンが受けるダメージ
	int GetWeaponRGDamage(void) { return weaponRGDamage_; }

	/// @brief 右キャノンが受けるダメージ
	int GetWeaponCannonLDamage(void) { return weaponCannonLDamage_; }

	/// @brief 左キャノンが受けるダメージ
	int GetWeaponCannonRDamage(void) { return weaponCannonRDamage_; }

	/// @brief 無敵付与
	bool GetInvincible(void) { return isInvincible_;}

	/// @brief プレイヤーの攻撃力受け取り用
	void SetPlayerAttack(int _attacks) { playerAttack_ = _attacks; }

	/// @brief プレイヤーの最大HP受け取り用
	void SetPlayerMaxHp(int _maxHp) { playerHpMax_ = _maxHp; }
	

	void DebugDraw(void);

private:

	// ダメージの軽減数値
	static constexpr float BOSS_CUT_DAMAGE = 0.8f;


	// プレイヤーが受けるダメージの保管
	int playerDamage_;

	// ボスが受けるダメージの保管
	int bossDamage_;

	// 右マシンガンが受けるダメージの保管
	int weaponMGRDamage_;

	// 左マシンガンが受けるダメージの保管
	int weaponMGLDamage_;

	// 右ミサイルポッドが受けるダメージの保管
	int weaponMPRDamage_;

	// 左ミサイルポッドが受けるダメージの保管
	int weaponMPLDamage_;

	// レールガンが受けるダメージの保管
	int weaponRGDamage_;

	// 右キャノンが受けるダメージの保管
	int weaponCannonLDamage_;

	//左キャノンが受けるダメージの保管
	int weaponCannonRDamage_;


	// プレイヤーが与えるダメージの保管
	int playerAttack_;

	// プレイヤーの最大HPの保管
	int playerHpMax_;


	bool isInvincible_;

	int DamageData_;
	
	BOSS_DATA cannon_;
	BOSS_DATA mg_;
	BOSS_DATA mp_;
	BOSS_DATA rg_;
	BOSS_DATA pressWave_;
	BOSS_DATA rode_;

	
};

