#include <DxLib.h>
#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
{
}


void WeaponBase::Update(void)
{
	// 各キャラクターごとの更新処理
	UpdateProcess();


	UpdateProcessPost();
}





void WeaponBase::ReleasePost(void)
{
}

void WeaponBase::DrawPre(void)
{
	MV1DrawModel(transform_.modelId);
}

