#include "SceneOperation.h"

#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Application.h"
#include "../../Manager/Generic/SceneManager.h"

SceneOperation::SceneOperation(void)
	: operationImageHandle_(-1)
	, backGroundHandle_(-1)
{
}

void SceneOperation::Load(void)
{
	operationImageHandle_ = ResourceManager::GetInstance().
		LoadHandleId(ResourceManager::SRC::IMG_OPERATION);

	backGroundHandle_ = ResourceManager::GetInstance().
		LoadHandleId(ResourceManager::SRC::IMG_SELECT_UI_BACK);
}

void SceneOperation::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneOperation::Initialize(void)
{
}

void SceneOperation::Update(void)
{
	if (KeyConfInputManager::GetInstance().isTrigerDown("CANCEL"))
	{
		SceneManager::GetInstance().PopScene();
	}
}

void SceneOperation::Draw(void)
{
	int width = 0;
	int height = 0;

	float uiBackScaleX = 1.0f;

	float uiBackScaleY = 1.5f;

	GetGraphSize(backGroundHandle_, &width, &height);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph3(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, width / 2, height / 2,
		uiBackScaleX, uiBackScaleY, 0.0f, backGroundHandle_, true);

	DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 0.5f, 0.0f,
		operationImageHandle_, true);
}

void SceneOperation::Release(void)
{
}
