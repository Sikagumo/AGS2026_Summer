#pragma once
#include "../CharaBase.h"
#include <memory>
#include <vector>
#include <array>
#include <map>
#include "../Weapon/Bullet/Player/PBulletBase.h"
#include "../Weapon/Bullet/Player/PBulletNormal.h"
class PBulletNormal;

class PlayerBase : public CharaBase
{
public:

	/// @brief ジョブ状態
	enum class JOB_TYPE
	{
		NONE = -1,  // 無効

		BOMB,       // 爆破
		CANNON,     // 巨大
		RAPID_FIRE, // 連射
		SUPPORT,    // 回復

		MAX         // 要素数
	};

	/// @brief 発射状態
	enum class SHOT_TYPE
	{
		NONE = -1,

		BOMB,      // 爆破

		BIG,       // 巨大

		RAPID_FIRE, // 連射
		CLUSTER,    // 拡散

		RECOVERY, // 回復
		POISON,   // 毒

		MAX // 要素数
	};

	/// @brief 見た目の状態
	enum class SKIN_TYPE
	{
		HUMAN,  // 人間
		DOG,    // 犬
		MONKEY, // サル
		BIRD,   // キジ

		MAX // 要素数
	};

	// 最大HP
	static constexpr int HP_MAX = 250;

	// デフォルト無敵時間
	static constexpr float TIME_INVINCIBLE = 1.0f;

	// 拡散弾の分割数
	static constexpr int CLUSTER_SPLIT = 16;

	// 拡散弾の一度に生成される数
	static constexpr int CLUSTER_NUM_MAX = ((2 * CLUSTER_SPLIT) + 1);


	/// @brief コンストラクタ
	/// @param _playerNo プレイヤー番号
	/// @param _jobType 弾の種類
	/// @param _startPos 初期位置
	/// @param _playerType プレイヤーの見た目の種類
	PlayerBase(int _playerNo, JOB_TYPE _jobType, const VECTOR& _startPos, SKIN_TYPE _playerType);

	/// @brief デストラクタ
	virtual ~PlayerBase(void)override;

	/// @brief プレイヤー番号取得
	int GetPlayerNo(void)const { return playerNo_; };

	/// @brief 見た目状態取得
	SKIN_TYPE GetPlayerType(void)const { return skinType_; };

	/// @brief ジョブ状態取得
	JOB_TYPE GetJobType(void)const { return jobType_; };

	/// @brief 発射状態取得
	SHOT_TYPE GetShotType(void)const { return shotType_; };

	/// @brief プレイヤーにダメージ処理
	/// @param _damage ダメージ量
	/// @param _isInvincible 無敵化させるか否か
	/// @param _timeInvincible 無敵時間
	/// @param _isIgnoreInvincible 無敵を無視するか否か
	void SetDamage(int _damage, bool _isInvincible = true, float _timeInvincible = TIME_INVINCIBLE, bool _isIgnoreInvincible = false);

	/// @brief HP取得
	int GetCurHp(void)const { return hp_; };

	/// @brief 座標取得
	const VECTOR& GetPos(void)const { return transform_.pos; };
	VECTOR GetBodyPos(void)const { return bodyPos_; };

	/// @brief 3D音声割り当て
	/// @param _pos 再生位置
	/// @param _radius 再生有効半径
	/// @param _isLanding ボス着地音有効するか否か
	/// @param _isMGFire ボスマシンガン発射音を有効にするか否か
	/// @param _isRoad ボス突進音を有効にするか否か
	void SetSoundData(VECTOR _pos, float _radius, bool _isLanding, bool _isMGFire, bool _isRoad);

	/// @brief 弾を取得
	const std::vector<std::unique_ptr<PBulletBase>>&
		GetBullets(void)const { return bullets_; };

	/// @brief 拡散弾を取得
	std::array<std::unique_ptr<PBulletNormal>, (CLUSTER_NUM_MAX)>&
		GetBulletsCluster(void) { return clusterBullets_; };


protected:

	// 左手のフレーム番号
	const std::map<SKIN_TYPE, int>
		FRAME_NUM_FINGER_LEFT
	= {
		{ SKIN_TYPE::HUMAN, 23},
		{ SKIN_TYPE::DOG, 15},
		{ SKIN_TYPE::MONKEY, 23},
		{ SKIN_TYPE::BIRD, 19}
	};

	// 右手のフレーム番号
	const std::map<SKIN_TYPE, int>
		FRAME_NUM_FINGER_RIGHT
		= {
			{ SKIN_TYPE::HUMAN, 39},
			{ SKIN_TYPE::DOG, 23},
			{ SKIN_TYPE::MONKEY, 47},
			{ SKIN_TYPE::BIRD, 31}
	};


	// プレイヤー番号
	const int playerNo_;

	// HP
	int hp_;

	// 初期位置
	const VECTOR START_POS;

	// 無敵時間
	float timeInv_;
	float timeInvDodge_;

	VECTOR bodyPos_;

	// 通常弾
	std::vector<std::unique_ptr<PBulletBase>>
		bullets_;

	// 拡散弾
	std::array<std::unique_ptr<PBulletNormal>, (CLUSTER_NUM_MAX)>
		clusterBullets_;

	// ジョブ状態
	JOB_TYPE jobType_;

	// 発射状態
	SHOT_TYPE shotType_;

	// 見た目状態
	SKIN_TYPE skinType_;

	/// @brief 3D音声更新
	void UpdateSound(void);
};
