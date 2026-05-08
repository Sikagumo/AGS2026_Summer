#include "StageManager.h"
#include "../System/Loading.h"
#include "../../Stage/StageBase.h"

StageManager::StageManager(void)
	: currentStage_(nullptr)
{

}

StageManager::~StageManager(void)
{

}

void StageManager::ChangeStage(std::shared_ptr<StageBase> nextStage)
{
	Loading::GetInstance()->StartAsyncLoad([this, nextStage]()
		{
			if (currentStage_)
			{
				currentStage_->Release();
			}
			currentStage_ = nextStage;

			if (currentStage_)
			{
				currentStage_->Load();

				currentStage_->EndLoad();

				currentStage_->Initialize();
			}
		});
}

void StageManager::Update(void)
{
	if (Loading::GetInstance()->IsLoading())return;

	if (currentStage_)
	{
		currentStage_->Update();
	}
}

void StageManager::Draw(void)
{
	if (Loading::GetInstance()->IsLoading()) return;

	if (currentStage_)
	{
		currentStage_->Draw();
	}
}

void StageManager::Release(void)
{
	if (currentStage_)
	{
		currentStage_->Release();

		currentStage_.reset();
	}
}

bool StageManager::IsLoading(void) const
{
	return Loading::GetInstance()->IsLoading();
}