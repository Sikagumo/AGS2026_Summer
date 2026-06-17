#include "../../../../Manager/Decoration/SoundManager.h"
#include "PlayerBase.h"

constexpr int HP = 250;

PlayerBase::PlayerBase(int _playerNo, BULLET_TYPE _bulletType, const VECTOR& _startPos, PLAYER_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, bulletType_(_bulletType)
	, playerType_(_playerType)
	, hp_(HP), MAX_HP(HP)
	, START_POS(_startPos)
	, bodyPos_(UtilityMath::VECTOR_ZERO)
	, timeInv_(0.0f)
{
}

void PlayerBase::SetDamage(int _damage, bool _isInvincible, float _timeInvincible)
{
	// –³“G‰Â”\Žž‚É–³“G’†AHP‚ª‚OˆÈ‰ºAƒ_ƒ[ƒW—Ê‚ª0ˆÈ‰ºŽžAˆ—I—¹
	if (_isInvincible && timeInv_ > 0.0f
		|| hp_ <= 0 || _damage <= 0) {
		return;
	}

	if (_isInvincible)
	{
		// –³“G‰Â”\ŽžA–³“GŽžŠÔŠ„‚è“–‚Ä
		timeInv_ = _timeInvincible;
	}

	hp_ -= _damage;
}

void PlayerBase::SetSoundDate(VECTOR _pos, float _radius,bool _isLanging, bool _isMGFire,bool _isRoad)
{
	if (_isLanging)
	{
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_BOSS_LANDING, _pos, transform_.pos, _radius);
	}

	if (_isMGFire)
	{
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_MG_FIRE, _pos, transform_.pos, _radius);
	}

	if (_isRoad)
	{
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_ROAD, _pos, transform_.pos, _radius);
	}

}

void PlayerBase::UpdaetaSound(void)
{
	SoundManager::GetInstance().Update3D(transform_.pos);
}
