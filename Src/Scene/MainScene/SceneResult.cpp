#include "SceneResult.h"

#include <DxLib.h>

#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Application.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Camera/Camera.h"
#include "SceneTitle.h"
#include "../../Utility/UtilityMath.h"

SceneResult::SceneResult(bool _isGameOver)
	: isGameOver_(_isGameOver)
{
}

void SceneResult::Load(void)
{
	
}

void SceneResult::EndLoad(void)
{
}

void SceneResult::Initialize(void)
{
	SetMouseDispFlag(true);

	for (size_t i = 0; i < imageResult_.size(); ++i)
	{
		imageResult_[i] = -1;
	}

	ResourceManager::GetInstance().LoadHandleIds(ResourceManager::SRC::IMGS_RESULT, imageResult_.data());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::NONE);
}

void SceneResult::Update(void)
{
	auto& keyConfInputManager = KeyConfInputManager::GetInstance();

	if (keyConfInputManager.isTrigerDown("OK"))
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneTitle>());
	}
}

void SceneResult::Draw(void)
{
	const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 3;

	if (isGameOver_)
	{
		DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 2.0f, UtilityMath::DEG2RAD, imageResult_[2], true);
	}
	else
	{
		DrawRotaGraph(Application::SCREEN_HALF_X, IMAGET_TITLE_Y, 2.0f, UtilityMath::DEG2RAD, imageResult_[0], true);
	}

	DrawString(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y + 10, "クリックしてタイトルへ", 0x000000, true);
}

void SceneResult::Release(void)
{
}

void SceneResult::UpdateGui(void)
{
}
