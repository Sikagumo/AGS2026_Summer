#include "Loading.h"

#include <DxLib.h>
#include <iostream>

#include "../Application.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Utility/UtilityMath.h"

Loading* Loading::instance_ = nullptr;

void Loading::CreateInstance(void)
{
	if (!instance_)
	{
		instance_ = new Loading();
	}
}

Loading* Loading::GetInstance(void)
{
	return instance_;
}

void Loading::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Loading::Loading(void)
	: isLoading_(false)
	, progress_(0.0f)
	, imageHandle_(-1)
{
}

Loading::~Loading(void)
{
}

void Loading::Initialize(void)
{
	isLoading_ = false;
	progress_ = 0.0f;
	imageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_PEACH);
}

void Loading::StartAsyncLoad(std::function<void()> loadFunc)
{
	if (isLoading_)
	{
		return;
	}

	Initialize();
	isLoading_ = true;

	if (loadFunc)
	{
		loadFunc();
	}
}

void Loading::Update(void)
{
	if (!isLoading_)
	{
		return;
	}

	const int LOAD_COUNT = GetASyncLoadNum(); 

	if (progress_ < 100.0f)
	{
		progress_ += 0.5f;
	}

	if (progress_ >= 100.0f)
	{
		if (LOAD_COUNT == 0)
		{
			EndAsyncLoad();
		}
		else
		{
			progress_ = 99.9f;
		}
	}
}

void Loading::Draw(void)
{
	ClearDrawScreen();

	// 背景を黒で塗りつぶす
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);

	float time = TimeManager::GetInstance().GetGameTime();
	float speed = 3.0f;
	float radius = 350.0f;
	float currentAngle = time * speed;

	VECTOR center = VGet(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 0.0f);

	VECTOR pos = UtilityMath::GetCirclePos(center, radius, time * speed);

	float rotationAngle = currentAngle + (DX_PI_F / 2.0f);

	DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), 0.2f, rotationAngle, imageHandle_, true);
}

void Loading::EndAsyncLoad(void)
{
	isLoading_ = false;
	progress_ = 100.0f;
}

bool Loading::IsLoading(void) const
{
	return isLoading_;
}

int Loading::GetProgress(void) const
{
	return static_cast<int>(progress_);
}

void Loading::SetProgress(float progress)
{
	if (progress < 0.0f)
	{
		progress = 0.0f;
	}
	if (progress > 100.0f)
	{
		progress = 100.0f;
	}
	progress_ = progress;
}