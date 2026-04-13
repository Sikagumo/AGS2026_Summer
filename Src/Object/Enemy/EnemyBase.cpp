#include "EnemyBase.h"
#include "../Actor/CharaBase.h"
#include "../../Utility/AsoUtility.h"
#include "../Player/Player.h"
#include "../../Manager/SceneManager.h"

EnemyBase::EnemyBase(const EnemyBase::EnemyData& data, Player& _player)
	:
	player_(_player),
	CharaBase::CharaBase(),
	type_(data.type),
	hp_(data.hp),
	defaultPos_(data.defaultPos),
	moveRange_(data.moveRange)
{
	// 初期座標の設定
	transform_.pos = defaultPos_;
}

void EnemyBase::ChangeState(int state)
{
	// 各状態遷移の初期処理
	stateChanges_[state]();
}

void EnemyBase::Draw(void)
{
	CharaBase::Draw();

	if (sceneMng_.GetIsDebugMode())
	{

		// 移動可能範囲のデバッグ描画
		//DrawSphere3D(defaultPos_, moveRange_, 16, 0x000099, 0x000099, false);
	}
}

bool EnemyBase::InMovableRange(void) const
{
	bool ret = false;

	// 初期位置からの距離
	float dis = static_cast<float>(AsoUtility::SqrMagnitude(defaultPos_, transform_.pos));

	// 指定距離判定
	if (dis < (moveRange_ * moveRange_))
	{
		return true;
	}
	return ret;
}