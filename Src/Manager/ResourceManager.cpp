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
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
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

	res = new Resource(RES_TYPE::IMG, PATH_IMG + "Title.png");
	resourcesMap_.emplace(SRC::IMG_TITLE, res);

	res = new Resource(RES_TYPE::IMG, PATH_IMG + "PushSpace.png");
	resourcesMap_.emplace(SRC::IMG_PUSH_SPACE, res);

	res = new Resource(RES_TYPE::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::IMG_SHADOW, res);
}
void ResourceManager::SetResourceModel(void)
{
	Resource* res;
	const std::string PATH_MDL = Application::PATH_MODEL;

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Stage/MainStage/MainStage.mv1");
	resourcesMap_.emplace(SRC::MODEL_STAGE_MAIN, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Enemy/Rat/Rat.mv1");
	resourcesMap_.emplace(SRC::MODEL_RAT, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Enemy/Robot/Robot.mv1");
	resourcesMap_.emplace(SRC::MODEL_ROBOT, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Enemy/Robot/Cone.mv1");
	resourcesMap_.emplace(SRC::MODEL_VIEW_RANGE, res);



	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Player/Player.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Stage/PitfallPlanet/PitfallPlanet.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLANET_PITFALL, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "Stage/SpherePlanet/SpherePlanet.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLANET_SPHERE, res);

	res = new Resource(RES_TYPE::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::MODEL_SKYDOME, res);
}
void ResourceManager::SetResourceAnim(void)
{
	Resource* res;
	const std::string PATH_ANIM = Application::PATH_MODEL;

	res = new Resource(RES_TYPE::ANIM, PATH_ANIM + "Player/Idle.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_IDLE, res);

	res = new Resource(RES_TYPE::ANIM, PATH_ANIM + "Player/Run.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_RUN, res);

	res = new Resource(RES_TYPE::ANIM, PATH_ANIM + "Player/FastRun.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_RUN_FAST, res);

	res = new Resource(RES_TYPE::ANIM, PATH_ANIM + "Player/Jumping.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_JUMPING, res);
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
