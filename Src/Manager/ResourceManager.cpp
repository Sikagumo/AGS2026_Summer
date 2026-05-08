#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Initialize();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Initialize(void)
{
	//static std::string PATH_EFF = Application::PATH_EFFECT;

	/* リソース読み込み処理 */
	
	// 画像読み込み
	SetResourceImage();

	// モデル読み込み
	SetResourceModel();

	// アニメーションモデル読み込み
	SetResourceAnim();

}
void ResourceManager::SetResourceImage(void)
{
	Resource* res;
	const std::string PATH_IMG = Application::PATH_IMAGE;

	
}
void ResourceManager::SetResourceModel(void)
{
	Resource* res;
	const std::string PATH_MDL = Application::PATH_MODEL;

	
}
void ResourceManager::SetResourceAnim(void)
{
	Resource* res;
	const std::string PATH_ANIM = Application::PATH_MODEL;

	
}
void ResourceManager::SetResourceEffect(void)
{
	Resource* res;
	const std::string PATH_EFF = Application::PATH_EFFECT;

	
}
void ResourceManager::SetResourceSound(void)
{
	Resource* res;
	const std::string PATH_SND = Application::PATH_SOUND;

	
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
