#pragma once
#include <vector>
#include "./EnemyBase.h"
class EnemyBase;
class ColliderBase;
class Transform;
class Player;

class EnemyManager
{
public:
	// コンストラクタ
	EnemyManager(Player* _player);

	// デストラクタ
	~EnemyManager(void) = default;


	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// エネミー
	const std::vector<EnemyBase*>& GetEemies(void) const { return enemies_; }

	// 衝突対象となるコライダを登録
	void AddHitCollider(const ColliderBase* hitCollider);


	// CSVから敵情報の読取を行う
	void LoadCsvData(void);

	/// @brief 敵生成処理
	EnemyBase* Create(const EnemyBase::EnemyData& data);


private:

	Player& player_;

	// エネミー
	std::vector<EnemyBase*> enemies_;
};