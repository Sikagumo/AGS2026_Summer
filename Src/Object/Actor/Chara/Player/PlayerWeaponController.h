#pragma once
#include <DxLib.h>
#include <array>
#include <memory>
#include <vector>
#include "../Weapon/Bullet/Player/PBulletBase.h"
#include "../Weapon/Bullet/Player/PBulletNormal.h"
class Transform;

class PlayerWeaponController
{
public:

	/// @brief ”­Ëó‘Ô
	enum class SHOT_TYPE
	{
		NONE = -1,  // –³Œøó‘Ô

		BOMB,      // ”š”j

		BIG,       // ‹‘å

		RAPID_FIRE, // ˜AË
		CLUSTER,    // ŠgU

		RECOVERY, // ‰ñ•œ
		POISON,   // “Å

		MAX // —v‘f”
	};

	// ˜AË’e”­Ë‘¬“x
	static constexpr float RAPID_SHOT_SPEED_XZ = 17.5f;
	static constexpr float RAPID_SHOT_SPEED_Y = 2.5f;

	// ŠgU’e”­Ë‘¬“x
	static constexpr float CLUSTER_SHOT_SPEED = 20.0f;

	// ŠgU’e‚Ì•ªŠ„”
	static constexpr int CLUSTER_SPLIT = 16;

	// ŠgU’e‚Ìˆê“x‚É¶¬‚³‚ê‚é”
	static constexpr int CLUSTER_NUM_MAX = ((2 * CLUSTER_SPLIT) + 1);

	/// @brief ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	/// @param _jobType UŒ‚‚Ìí—Ş
	/// @param _owner ‚¿å‚ÌTransform
	PlayerWeaponController(int _jobType, const Transform& _owner);

	/// @brief ƒfƒXƒgƒ‰ƒNƒ^
	~PlayerWeaponController(void);

	/// @brief ƒŠƒ\[ƒX“Ç‚İ‚İ
	void Load(void);

	/// @brief ’eŠÛXV
	/// @param _skinType Œ©‚½–Ú‚Ìí—Ş
	/// @param _isAnimationStop ‚¿å‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ª’â~’†‚©”Û‚©
	void Update(int _skinType, bool _isAnimationStop);

	/// @brief •`‰æˆ—
	void Draw(void);

	/// @brief ’e‚Ì‹O“¹‚ğ•`‰æ
	void DrawShotOrbit(void);

	/// @brief ƒƒ‚ƒŠ‰ğ•úˆ—
	void Release(void);

	/// @brief ’e‚Ìí—Ş‚Ìİ’è
	void SetShotType(SHOT_TYPE _type) { shotType_ = _type; }
	SHOT_TYPE GetShotType(void) const { return shotType_; }

	/// @brief ’eŠÛ¶¬
	/// @param _isShotSpecial “Áê’e”­Ë‚©”Û‚©
	void CreateBullet(bool _isShotSpecial);

	/// @brief ’eŠÛ”­Ë
	void ShotBullet(void);

	/// @brief ŠgU’e”­Ë
	void ShotCluster(void);

	/// @brief UŒ‚‰ñ”
	int  GetAttackCount(void) const { return curAttackCount_; }

	/// @brief UŒ‚‰ñ”ƒŠƒZƒbƒg
	void ResetAttackCount(void) { curAttackCount_ = 0; }

	/// @brief ƒRƒ“ƒ{Å‘å”
	int GetAttackNumMax(void) const { return attackNumMax_; }

	/// @brief ”­Ë‚µ‚Ä‚¢‚é’e‚Ì¯•Ê”Ô† 
	int GetShotIndex(void)const { return shotIndex_; }

	/// @brief ”­Ë‘Ò‚¿‚Ì’e‚ª‚ ‚é‚©”Û‚©
	bool IsShooting(void) const { return shotIndex_ != -1; }

	/// @brief ŠgU’e‚ª¶‘¶’†‚©”Û‚©
	bool IsClusterAlive(void) const;

	/// @brief ’e‚ğæ“¾
	const std::vector<std::unique_ptr<PBulletBase>>&
		GetBullets(void)const { return bullets_; }

	/// @brief ŠgU’e‚ğæ“¾
	std::array<std::unique_ptr<PBulletNormal>, CLUSTER_NUM_MAX>&
		GetBulletsCluster(void) { return clusterBullets_; }


private:

	// ˜AËƒpƒ‰ƒ[ƒ^
	static constexpr float RAPID_SCALE      = 0.85f; // ƒXƒP[ƒ‹
	static constexpr float RAPID_RADIUS     = 10.0f; // “–‚½‚è”»’è”¼Œa
	static constexpr int   RAPID_POWER      = 30;    // UŒ‚—Í
	static constexpr float RAPID_ALIVE_TIME = 2.5f;  // ¶‘¶ŠÔ

	// ŠgU’eƒpƒ‰ƒ[ƒ^
	static constexpr float CLUSTER_SCALE      = 0.25f; // ƒXƒP[ƒ‹
	static constexpr float CLUSTER_RADIUS     = 5.0f;  // “–‚½‚è”»’è”¼Œa
	static constexpr int   CLUSTER_POWER      = 30;    // UŒ‚—Í
	static constexpr float CLUSTER_ALIVE_TIME = 0.25f; // ¶‘¶ŠÔ

	// ƒvƒŒƒCƒ„[‚ÌTransform
	const Transform& owner_;

	// ’e‚Ìí—Ş
	SHOT_TYPE shotType_;

	// ˜AË’e‚©”Û‚©
	const bool IS_RAPID_FIRE;

	// UŒ‚‚Ìí—Ş
	const SHOT_TYPE SHOT_NORMAL;
	const SHOT_TYPE SHOT_SPECIAL;

	// ’e
	std::vector<std::unique_ptr<PBulletBase>> bullets_;

	// ŠgU’e
	std::array<std::unique_ptr<PBulletNormal>, CLUSTER_NUM_MAX> clusterBullets_;

	// ’…’e’n“_‰æ‘œ
	int imageBulletHitPoint_;

	// UŒ‚‰ñ”
	int attackNumMax_;
	int curAttackCount_;

	// ”­Ë‚µ‚Ä‚¢‚é’e‚Ì¯•Ê”Ô†
	int shotIndex_;

	// ”­ËˆÊ’u
	VECTOR throwPos_;

	// ”­Ë•ûŒü
	VECTOR throwDir_;

	// ’…’e’n“_
	std::array<VERTEX3D, 4> shotPointVertex_;

	/// @brief ŠgU’e¶¬
	void CreateCluster(void);

	/// @brief ŠgU’e¶¬ˆ—
	/// @return ŠgU’eƒIƒuƒWƒFƒNƒg
	std::unique_ptr<PBulletNormal> CreateClusterBullet(void);

	/// @brief ˆÊ’u‚É‰ÁZˆ—
	VECTOR CalcShotDirection(void);

	/// @brief ’e‚Ì’…’e’n“_
	void DrawShotOrbitPoint(const VECTOR& _shotPos);
};