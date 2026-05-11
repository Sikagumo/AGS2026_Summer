#include "../../../../Manager/Generic/ResourceManager.h"
#include "../../../../Utility/UtilityMath.h"
#include "../../../../Utility/MatrixUtility.h"
#include "../../../Common/Transform.h"
#include "Boss.h"

Boss::Boss():
	transformFeet_(),
	transformBody_(),

	CharaBase()
{
}

Boss::~Boss()
{
}

void Boss::Release()
{
}

void Boss::InitLoad(void)
{
	transformFeet_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_FEET));
	transformBody_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_BOSS_BODY));
	transformFeet_.scl = BOSS_SIZE;
	transformBody_.scl = BOSS_SIZE;
	transformFeet_.quaRot= Quaternion::Identity();
	transformFeet_.quaRotLocal =
		Quaternion::Mult(transformFeet_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));
	transformBody_.quaRot = Quaternion::Identity();
	transformBody_.quaRotLocal =
		Quaternion::Mult(transformBody_.quaRotLocal,
			Quaternion::AngleAxis(UtilityMath::Deg2RadF(INIT_ROT), UtilityMath::AXIS_Y));

	transformFeet_.pos= BOSS_INIT_POS;
	transformBody_.pos = MV1GetFramePosition(transformFeet_.modelId,JOINT_NO);

	transformFeet_.Update();
	transformBody_.Update();

}

void Boss::InitTransform(void)
{
}

void Boss::InitCollider(void)
{
}

void Boss::InitAnimationPost(void)
{
}

void Boss::InitPost(void)
{
}

void Boss::UpdateProcess(void)
{
}

void Boss::UpdateProcessPost(void)
{
}

void Boss::DrawPre(void)
{
	MV1DrawModel(transformFeet_.modelId);
	MV1DrawModel(transformBody_.modelId);
}
