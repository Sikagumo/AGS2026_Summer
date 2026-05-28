#pragma once
class DamageController
{

public:

	enum class BOSS_WEPO_DAMEGE
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

	int GetBossDamege(void) { return bossDamege_;}
	int GetPlayerDamege(void) { return playerDamege_; }

	int GetWeaponMGRDamege(void) { return weaponMGRDamege_; }
	int GetWeaponMGLDamege(void) { return weaponMGLDamege_; }

	int GetWeaponMPRDamege(void) { return weaponMPRDamege_; }
	int GetWeaponMPLDamege(void) { return weaponMPLDamege_; }

	int GetWeaponRGDamege(void) { return weaponRGDamege_; }

	int GetWeaponCannonLDamege(void) { return weaponCannonLDamege_; }
	int GetWeaponCannonRDamege(void) { return weaponCannonRDamege_; }


	void SetPlayerAttack(int _attack) { playerAttack_ = _attack; }
	void SetBossAttack(int _attack) { bossAttack_ = _attack; }
	void SetPlayerMaxHp(int _maxHp) { playerHp_ = _maxHp; }
	

private:
	static constexpr float BOSS_CAT_DAMEGE = 0.8;


	int playerDamege_;
	int bossDamege_;
	int weaponMGRDamege_;
	int weaponMGLDamege_;
	int weaponMPRDamege_;
	int weaponMPLDamege_;
	int weaponRGDamege_;
	int weaponCannonLDamege_;
	int weaponCannonRDamege_;
	int playerAttack_;
	int bossAttack_;
	int playerHp_;
};

