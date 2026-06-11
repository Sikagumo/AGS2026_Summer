#include "BBulletBase.h"

BBulletBase::BBulletBase(void)
{
}

BBulletBase::~BBulletBase(void)
{
}

void BBulletBase::Update(void)
{
	// 各キャラクターごとの更新処理
	UpdateProcess();

	transform_.Update();
	UpdateProcessPost();
}

void BBulletBase::Draw(void)
{
	DrawPre();
}
