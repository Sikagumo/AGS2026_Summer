#include"SceneBase.h"

SceneBase::SceneBase(void)
	: isDebugMode_(false)
	, isLoading_(false)
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Initialize(void)
{
	isLoading_ = false;
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}

void SceneBase::Release(void)
{
}

void SceneBase::Load(void)
{
	isLoading_ = true;
}

void SceneBase::EndLoad(void)
{
	isLoading_ = false;
}

bool SceneBase::IsLoading(void) const
{
	return isLoading_;
}
