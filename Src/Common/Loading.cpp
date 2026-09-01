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
	, progress_(MIN_PROGRESS)
	, imageHandle_(-1)
{
	imageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_PEACH);
}

Loading::~Loading(void)
{
}

void Loading::Initialize(void)
{
	isLoading_ = false;
	progress_ = MIN_PROGRESS;
}

void Loading::StartAsyncLoad(std::function<void()> _loadFunc)
{
	SetUseASyncLoadFlag(true);
	if (isLoading_)
	{
		return;
	}

	Initialize();
	isLoading_ = true;

	if (_loadFunc)
	{
		_loadFunc();
	}
	SetUseASyncLoadFlag(false);
}

void Loading::Update(void)
{
	if (!isLoading_)
	{
		return;
	}

	const int LOAD_COUNT = GetASyncLoadNum();  // 残りの非同期ロード数
	const float PROGRESS_SPEED = 0.5f;         // 1フレームごとの進捗増加量
	const float WAIT_PROGRESS = 99.9f;         // ロード完了待ちの進捗率

	if (progress_ < MAX_PROGRESS)
	{
		progress_ += PROGRESS_SPEED;
	}

	if (progress_ >= MAX_PROGRESS)
	{
		if (LOAD_COUNT == 0)
		{
			EndAsyncLoad();
		}
		else
		{
			progress_ = WAIT_PROGRESS;
		}
	}
}

void Loading::Draw(void)
{
	const unsigned int BLACK_COLOR = GetColor(0, 0, 0);

	// 背景を黒で塗りつぶす
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BLACK_COLOR, true);

	const float ROTATION_SPEED = 3.0f;  // 回転速度
	const float CIRCLE_RADIUS = 350.0f; // 回転の半径

	float gameTime = TimeManager::GetInstance().GetGameTime(); // ゲームの経過時間
	float currentAngle = gameTime * ROTATION_SPEED;            // 現在の角度

	VECTOR centerPos = VGet(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y, 0.0f); 
	VECTOR drawPos = UtilityMath::GetCirclePos(centerPos, CIRCLE_RADIUS, currentAngle);    

	float rotationAngle = currentAngle + (DX_PI_F / 2.0f); 

	DrawRotaGraph(static_cast<int>(drawPos.x), static_cast<int>(drawPos.y), 0.2f, rotationAngle, imageHandle_, true);
}

void Loading::EndAsyncLoad(void)
{
	isLoading_ = false;
	progress_ = MAX_PROGRESS;
}

bool Loading::IsLoading(void) const
{
	return isLoading_;
}

int Loading::GetProgress(void) const
{
	return static_cast<int>(progress_);
}

void Loading::SetProgress(float _progress)
{
	if (_progress < MIN_PROGRESS)
	{
		_progress = MIN_PROGRESS;
	}
	if (_progress > MAX_PROGRESS)
	{
		_progress = MAX_PROGRESS;
	}
	progress_ = _progress;
}