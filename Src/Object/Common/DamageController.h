#pragma once
class DamageController
{

public:

	///武器ダメージの登録用
	enum class BOSS_WEPO_DAMAGE
	{
		MG,
		PRESSWAVE,
		CANNON,
		MP,
		RG,
		
	};


	DamageController(void);
	~DamageController(void);

	void Update(void);
	///ボスが受けるダメージ
	int GetBossDamage(void) { return bossDamage_;}
	///プレイヤーが受けるダメージ
	int GetPlayerDamage(void) { return playerDamage_; }
	///右マシンガンが受けるダメージ
	int GetWeaponMGRDamage(void) { return weaponMGRDamage_; }
	///左マシンガンが受けるダメージ
	int GetWeaponMGLDamage(void) { return weaponMGLDamage_; }
	///右ミサイルポッドが受けるダメージ
	int GetWeaponMPRDamage(void) { return weaponMPRDamage_; }
	///左ミサイルポッドが受けるダメージ
	int GetWeaponMPLDamage(void) { return weaponMPLDamage_; }
	///レールガンが受けるダメージ
	int GetWeaponRGDamage(void) { return weaponRGDamage_; }
	///右キャノンが受けるダメージ
	int GetWeaponCannonLDamage(void) { return weaponCannonLDamage_; }
	///左キャノンが受けるダメージ
	int GetWeaponCannonRDamage(void) { return weaponCannonRDamage_; }
	///プレイヤーの攻撃力受け取り用
	void SetPlayerAttack(int _attack) { playerAttack_ = _attack; }
	///プレイヤーの最大HP受け取り用
	void SetPlayerMaxHp(int _maxHp) { playerHp_ = _maxHp; }
	

private:
	//ダメージの軽減数値
	static constexpr float BOSS_CUT_DAMAGE = 0.8f;


	int playerDamage_;			//プレイヤーが受けるダメージの保管
	int bossDamage_;			//ボスが受けるダメージの保管
	int weaponMGRDamage_;		//右マシンガンが受けるダメージの保管
	int weaponMGLDamage_;		//左マシンガンが受けるダメージの保管
	int weaponMPRDamage_;		//右ミサイルポッドが受けるダメージの保管
	int weaponMPLDamage_;		//左ミサイルポッドが受けるダメージの保管
	int weaponRGDamage_;		//レールガンが受けるダメージの保管
	int weaponCannonLDamage_;	//右キャノンが受けるダメージの保管
	int weaponCannonRDamage_;	//左キャノンが受けるダメージの保管
	int playerAttack_;			//プレイヤーが与えるダメージの保管
	int playerHp_;				//プレイヤーの最大HPの保管
};

