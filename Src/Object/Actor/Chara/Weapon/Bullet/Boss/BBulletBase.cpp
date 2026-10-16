#include "BBulletBase.h"

BBulletBase::BBulletBase(void)
	: isAttack_(false)
	, isAlive_(false)
	, speed_(0.0f)
	, radius_(0.0f)
	, dir_({ 0.0f, 0.0f, 0.0f })
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