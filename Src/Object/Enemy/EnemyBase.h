#pragma once
#include <DxLib.h>	
#include <functional>
#include "../Actor/CharaBase.h"
class Player;

class EnemyBase : public CharaBase
{
public:

	// 種別
	enum class TYPE
	{
		RAT,
		ROBOT,
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE = 9,
		WALK = 13,
	};


	// エネミーデータ
	struct EnemyData
	{
		int id;
		EnemyBase::TYPE type;
		int hp;
		VECTOR defaultPos;
		float moveRange;
	};

	// コンストラクタ
	EnemyBase(const EnemyBase::EnemyData& data, Player& _player);

	// デストラクタ
	virtual ~EnemyBase(void) override = default;

	// 描画
	virtual void Draw(void) override;


protected:

	Player& player_;

	// 初期位置
	const VECTOR defaultPos_;


	// 種別
	TYPE type_;

	// HP
	int hp_;

	// 移動範囲
	float moveRange_;

	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;

	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;


	// 状態遷移
	void ChangeState(int state);

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {}

	// 衝突判定の初期化
	void InitCollider(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override {}

	// 更新系
	virtual void UpdateProcessPost(void) override {}

	// 移動可能範囲判定
	bool InMovableRange(void) const;

	void Search(void);

	void SearchField(void);
};