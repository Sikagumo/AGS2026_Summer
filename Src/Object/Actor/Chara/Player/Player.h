#pragma once
#include "./PlayerBase.h"
#include "../Player/PlayerBase.h"
#include "./PActionController.h"
#include <DxLib.h>

class Player : public PlayerBase
{
private:

	static constexpr float KNOCK_POW_Y = 100.0f;
public:

	enum class ANIM_TYPE
	{
		NONE = -1,
		IDLE,
		RUN,
		THROW_LEFT,
		THROW_RIGHT,
		THROW_RUN,
		JUMP,
		MAX,
	};

	Player(int _playerNo, BULLET_TYPE _playerType);

	~Player(void)override = default;

	void Load(void)override;

	void Draw(void)override;

	void DrawDebug(void)override;

	void ReleasePost(void)override;

	/// @brief Œ»İUŒ‚—Íæ“¾
	int GetPower(void);

	/// @brief ‚Á”ò‚Î‚µˆ—
	/// @param _knockDirXZ ‰¡‚Á”ò‚Î‚µ•ûŒü
	/// @param _knockPowXZ ‰¡‚Á”ò‚Î‚µ—Í
	/// @param _isStan ƒXƒ^ƒ“‚³‚¹‚é‚©”Û‚© 
	/// @param _knockPowY c‚Á”ò‚Î‚µ—Í
	void SetKnock(const VECTOR& _knockDirXZ, float _knockPowXZ
					, bool _isStan, float _knockPowY = KNOCK_POW_Y);


protected:

	void InitAnimation(void)override;
	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitPost(void)override;


	void UpdateProcess(void);
	void UpdateProcessPost(void);


	void DrawPre(void)override;
	void DrawLate(void)override;


private:

	// UŒ‚‰ñ”
	int attackNumMax_;
	int curAttackNum_;

	int shotIndex_;
	int shadowHandle_;

	ANIM_TYPE animType_;

	VECTOR throwPos_;
	VECTOR throwDir_;

	// ƒJƒƒ‰‚É‰‚¶‚½‰ñ“]‚ğ‚·‚é‚©”Û‚©
	bool isCameraRotActive_;

	std::unique_ptr<PActionController> actionController_;


	// ‘€ì
	void ProcessMove(void);

	// ƒWƒƒƒ“ƒv
	void ProcessJump(void);
	void Jump(void);

	void ProcessAttack(void);


	void DrawShadowRound(void);
	void PlayAnimation(ANIM_TYPE _type, bool _isLoop = true);

	void CreateBullet(void);
	void ShotBullet(void);
	void UpdateBullets(void);

	void DelayRotate(void)override;
};