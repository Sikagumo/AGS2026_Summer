#include "StageBase.h"

StageBase::StageBase(void)
	: LimitMaxX_(0.0f)
	, LimitMinX_(0.0f)
	, LimitMaxY_(0.0f)
	, LimitMinY_(0.0f)
{

}

StageBase::~StageBase(void)
{
}

void StageBase::Load(void)
{
	SceneBase::Load();
}

void StageBase::EndLoad(void)
{
	SceneBase::EndLoad();
}

void StageBase::DebugDraw(void) const
{

}
