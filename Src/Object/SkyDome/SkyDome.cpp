#include "SkyDome.h"
#include <Dxlib.h>
#include "../Actor/ActorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

SkyDome::SkyDome(const Transform& _transform):
	followTransform_(_transform),
	state_(STATE::NONE),
	ActorBase()
{
}

void SkyDome::InitLoad(void)
{
	transform_.SetModel(resMng_.GetHandleId(ResourceManager::SRC::MODEL_SKYDOME));
}
void SkyDome::InitTransform(void)
{
	transform_.InitTransform(SCALE,
							 Quaternion::Identity(), Quaternion::AngleAxis(180.0f, AsoUtility::AXIS_Y),
							 AsoUtility::VECTOR_ZERO);
}
void SkyDome::InitCollider(void)
{
}
void SkyDome::InitAnimation(void)
{
}
void SkyDome::InitPost(void)
{
	// Zバッファの使用&書き込み無効(突き抜け対策)
	MV1SetUseZBuffer(transform_.modelId, false);
	MV1SetWriteZBuffer(transform_.modelId, false);

	SceneManager::SCENE_ID sceneId = sceneMng_.GetSceneID();
	if (sceneId == SceneManager::SCENE_ID::GAME)
	{
		ChangeStateFollow();
	}
	else
	{
		ChangeStateStay();
	}
}

void SkyDome::Update(void)
{
	switch (state_)
	{
		case STATE::NONE:
			UpdateNone();
		break;

		case STATE::STAY:
			UpdateStay();
		break;

		case STATE::FOLLOW:
			UpdateFollow();
		break;
	}
}
void SkyDome::UpdateNone(void)
{
}
void SkyDome::UpdateStay(void)
{
	transform_.Rotate(AsoUtility::AXIS_Y, ROT_SPEED);
}
void SkyDome::UpdateFollow(void)
{
	// 回転
	transform_.Rotate(AsoUtility::AXIS_Y, ROT_SPEED);

	transform_.pos = followTransform_.pos;

	transform_.Update();
}


void SkyDome::Draw(void)
{
	// ライト効果を無効化して描画
	SetUseLighting(FALSE);
	ActorBase::Draw();
	SetUseLighting(TRUE);
}

void SkyDome::ChangeState(STATE _state)
{
	state_ = _state;
	switch(state_)
	{
		case STATE::NONE:
			ChangeStateNone();
		break;

		case STATE::STAY:
			ChangeStateStay();
		break;

		case STATE::FOLLOW:
			ChangeStateFollow();
		break;
	}
}
void SkyDome::ChangeStateNone(void)
{
}
void SkyDome::ChangeStateStay(void)
{
	state_ = STATE::STAY;
	transform_.pos = followTransform_.pos;
}
void SkyDome::ChangeStateFollow(void)
{
	state_ = STATE::FOLLOW;
	transform_.pos = followTransform_.pos;
}
