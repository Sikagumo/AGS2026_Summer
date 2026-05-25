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


	DamageController();
	~DamageController();

	void Update();

	int GetBossDamege() { return bossDamege_;}
	int GetPlayerDamege() { return playerDamege_; }
	int GetWeaponDamege() { return weaponDamege_; }

	void SetPlayerAttack(int _attack) { playerAttack_ = _attack; }
	void SetBossAttack(int _attack) { bossAttack_ = _attack; }
	void SetPlayerMaxHp(int _maxHp) { playerHp_ = _maxHp; }
	

private:
	static constexpr float BOSS_CAT_DAMEGE = 0.8;


	int playerDamege_;
	int bossDamege_;
	int weaponDamege_;
	int playerAttack_;
	int bossAttack_;
	int playerHp_;
};

