#include "SceneResult.h"

#include <DxLib.h>

#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Application.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Camera/Camera.h"
#include "SceneTitle.h"
#include "../../Utility/UtilityMath.h"

SceneResult::SceneResult(bool _isGameOver)
	: isGameOver_(_isGameOver)
	, img_(-1)
{
}

void SceneResult::Load(void)
{
	if (isGameOver_)
	{
		img_= ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_RESULT_BUT);
	}
	else
	{
		img_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_RESULT_WIN);
	}
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_RESULT
		, ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::BGM_RESULT));
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

	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_RESULT);
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
	const int IMAGET_TITLE_Y = Application::SCREEN_SIZE_Y / 3+30;
	const int IMAGET_TITLE_X = Application::SCREEN_HALF_X / 2+50;

	DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 1.0f, UtilityMath::DEG2RAD, img_, true);

	if (isGameOver_)
	{
		DrawRotaGraph(IMAGET_TITLE_X, IMAGET_TITLE_Y, 1.0f, UtilityMath::DEG2RAD, imageResult_[2], true);
	}
	else
	{
		DrawRotaGraph(IMAGET_TITLE_X, IMAGET_TITLE_Y, 1.0f, UtilityMath::DEG2RAD, imageResult_[0], true);
	}

	DrawString(Application::SCREEN_HALF_X+ (Application::SCREEN_HALF_X/2), Application::SCREEN_SIZE_Y - 20, "クリックしてタイトルへ", 0xffffff, true);
}

void SceneResult::Release(void)
{
}

void SceneResult::UpdateGui(void)
{
}
