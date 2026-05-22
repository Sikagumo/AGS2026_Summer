#pragma once
class DamageController
{

public:
	DamageController();
	~DamageController();

	void Update();

	int GetBossDamege() { return damege_;}
	int GetPlayerDamege() { return damege_; }
	int GetWeaponDamege() { return damege_; }
	

private:
	int damege_;

};

