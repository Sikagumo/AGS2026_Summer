#include "../../../../Manager/Decoration/SoundManager.h"
#include "PlayerBase.h"

PlayerBase::PlayerBase(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos, SKIN_TYPE _playerType)
	: CharaBase::CharaBase()
	, playerNo_(_playerNo)
	, jobType_(_jobType)
	, skinType_(_playerType)
	, hp_(HP_MAX)
	, START_POS(_startPos)
	, bodyPos_(UtilityMath::VECTOR_ZERO)
	, timeInv_(0.0f)
	, timeInvDodge_(0.0f)
	, shotType_(SHOT_TYPE::NONE)
	, bullets_{}
	, clusterBullets_{}
{
	transform_.pos = START_POS;
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

	// HP‚É”½‰f
	SoundManager::GetInstance().Play(SoundManager::SOUND::SE_DAMAGE_PLAYER);
	hp_ -= _damage;
}

void PlayerBase::SetSoundData(VECTOR _pos, float _radius, bool _isLanding, bool _isMGFire, bool _isRoad)
{
	if (_isLanding)
	{
		// ’…’nSEÄ¶
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_BOSS_LANDING
					 , _pos, transform_.pos, _radius);
	}

	if (_isMGFire)
	{
		// ƒ}ƒVƒ“ƒKƒ“SEÄ¶
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_BOSS_MG_FIRE
					 , _pos, transform_.pos, _radius);
	}

	if (_isRoad)
	{
		// “ËiSEÄ¶
		SoundManager::GetInstance().Play3D(SoundManager::SOUND::SE_BOSS_ROAD
					 , _pos, transform_.pos, _radius);
	}
}

void PlayerBase::UpdateSound(void)
{
	SoundManager::GetInstance().Update3D(transform_.pos);
}
