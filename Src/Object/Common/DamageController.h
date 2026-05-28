#pragma once
class DamageController
{

public:

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

	int GetBossDamage(void) { return bossDamage_;}
	int GetPlayerDamage(void) { return playerDamage_; }

	int GetWeaponMGRDamage(void) { return weaponMGRDamage_; }
	int GetWeaponMGLDamage(void) { return weaponMGLDamage_; }

	int GetWeaponMPRDamage(void) { return weaponMPRDamage_; }
	int GetWeaponMPLDamage(void) { return weaponMPLDamage_; }

	int GetWeaponRGDamage(void) { return weaponRGDamage_; }

	int GetWeaponCannonLDamage(void) { return weaponCannonLDamage_; }
	int GetWeaponCannonRDamage(void) { return weaponCannonRDamage_; }


	void SetPlayerAttack(int _attack) { playerAttack_ = _attack; }
	void SetBossAttack(int _attack) { bossAttack_ = _attack; }
	void SetPlayerMaxHp(int _maxHp) { playerHp_ = _maxHp; }
	

private:
	static constexpr float BOSS_CUT_DAMAGE = 0.8f;


	int playerDamage_;
	int bossDamage_;
	int weaponMGRDamage_;
	int weaponMGLDamage_;
	int weaponMPRDamage_;
	int weaponMPLDamage_;
	int weaponRGDamage_;
	int weaponCannonLDamage_;
	int weaponCannonRDamage_;
	int playerAttack_;
	int bossAttack_;
	int playerHp_;
};

