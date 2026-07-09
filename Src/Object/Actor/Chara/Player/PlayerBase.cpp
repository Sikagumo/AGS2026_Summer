#include "../../../../Manager/Decoration/SoundManager.h"
#include "PlayerBase.h"

constexpr int HP_MAX_PLAYER = 250;

PlayerBase::PlayerBase(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos, PLAYER_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, jobType_(_jobType)
	, playerType_(_playerType)
	, hp_(HP_MAX_PLAYER), HP_MAX(HP_MAX_PLAYER)
	, START_POS(_startPos)
	, bodyPos_(UtilityMath::VECTOR_ZERO)
	, timeInv_(0.0f), timeInvDodge_(0.0f)
{
}

void PlayerBase::SetDamage(int _damage, bool _isInvincible
						  , float _timeInvincible, bool _isIgnoreInvincible)
{
	if (!_isIgnoreInvincible)
	{
		// –³“G‚ð–³Ž‹‚µ‚ÄUŒ‚‚µ‚È‚¢‚Æ‚«‚ÉA
		// –³“G’†AHP‚ª‚OˆÈ‰ºAƒ_ƒ[ƒW—Ê‚ª0ˆÈ‰ºŽžAˆ—I—¹
		if (timeInv_ > 0.0f || timeInvDodge_ > 0.0f
			|| hp_ <= 0 || _damage <= 0) {
			return;
		}
	}

	if (_isInvincible)
	{
		// –³“G‰Â”\ŽžA–³“GŽžŠÔŠ„‚è“–‚Ä
		timeInv_ = _timeInvincible;
	}

	hp_ -= _damage;
}

void PlayerBase::SetSoundData(VECTOR _pos, float _radius,bool _isLanging, bool _isMGFire,bool _isRoad)
{
	SoundManager& sound = SoundManager::GetInstance();
	if (_isLanging)
	{
		sound.Play3D(SoundManager::SOUND::SE_BOSS_LANDING
					 , _pos, transform_.pos, _radius);
	}

	if (_isMGFire)
	{
		sound.Play3D(SoundManager::SOUND::SE_MG_FIRE
					 , _pos, transform_.pos, _radius);
	}

	if (_isRoad)
	{
		sound.Play3D(SoundManager::SOUND::SE_ROAD
					 , _pos, transform_.pos, _radius);
	}

}

void PlayerBase::UpdateSound(void)
{
	SoundManager::GetInstance().Update3D(transform_.pos);
}
