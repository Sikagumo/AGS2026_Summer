#include "Loading.h"
#include <DxLib.h>
#include <iostream>
#include "../Application.h"
#include "../Manager/Generic/ResourceManager.h"

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

	if (ProcessMessage() != 0)
	{
		isLoading_ = false;
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
	const int screenWidth = Application::SCREEN_SIZE_X;
	const int screenHeight = Application::SCREEN_SIZE_Y;

	ClearDrawScreen();

	// 背景を黒で塗りつぶす
	DrawBox(0, 0, screenWidth, screenHeight, GetColor(0, 0, 0), TRUE);

	// 💡 調整用パラメータ
	// 画面の幅（例: 1280）からマージンを引いた分だけ動くようにする
	const int margin = 100;
	const int heightOffset = 100; // プラスで下に、マイナスで上に

	// 画像のサイズを取得
	int imageWidth = 0;
	int imageHeight = 0;
	GetGraphSize(imageHandle_, &imageWidth, &imageHeight);

	// X座標の開始地点（左端）と終了地点（右端）を計算
	const int startX = -imageWidth; // 完全に画面の外からスタート
	const int endX = screenWidth;   // 完全に画面の外へ抜ける

	// 現在の進捗率を基に、左端から右端まで線形補間
	const int imageX = static_cast<int>(startX + (endX - startX) * (progress_ / 100.0f));

	// Y座標（画面中央ベースで調整）
	const int centerY = (screenHeight / 2) + heightOffset;
	const int imageY = centerY - imageHeight / 2;

	DrawRotaGraph(imageX, imageY, 0.5f, 0.0f, imageHandle_, true);
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