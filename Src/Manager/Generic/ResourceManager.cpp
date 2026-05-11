#include "ResourceManager.h"
#include "Resource.h"
#include <DxLib.h>
#include <string>
#include <unordered_map>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"

ResourceManager* ResourceManager::instance_ = nullptr;

// リソースファイルのパス
#ifdef _DEBUG
const std::string PATH_DATA = "Data/";

// 暗号化済みのリソースフォルダパス
#else

const std::string PATH_DATA = "_ResourceData/";
#endif


// ファイルパスの割り当て
const std::string ResourceManager::PATH_EFFECT = PATH_DATA + "Effect/";
const std::string ResourceManager::PATH_IMAGE  = PATH_DATA + "Image/";
const std::string ResourceManager::PATH_MODEL  = PATH_DATA + "Model/";
const std::string ResourceManager::PATH_ANIM   = PATH_DATA + "Model/Animation/";
const std::string ResourceManager::PATH_SE     = PATH_DATA + "Sound/SE/";
const std::string ResourceManager::PATH_BGM    = PATH_DATA + "Sound/BGM/";
const std::string ResourceManager::PATH_MOVIE  = PATH_DATA + "Movie/";


void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}

	instance_->Initialize();
}

ResourceManager::ResourceManager(void)
{
	
}


void ResourceManager::Initialize(void)
{
	using LOAD_TYPE = Resource::LOAD_TYPE;

	/* 画像 */
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_TITLE, PATH_IMAGE + "Title.png");


	/* 複数画像 */

	// 画像枚数
	int imagesAllNum = 0;

	// １画像の横枚数
	int imagesNumX = 0;
	int imagesNumY = 0;

	imagesAllNum = 11;
	imagesNumX = 1;
	imagesNumY = 11;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TEXTS, PATH_IMAGE + "Text.png"
		, imagesAllNum, imagesNumX, imagesNumY);


	/* モデル */
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_SKYDOME, PATH_MODEL + "SkyDome/SkyDome.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER, PATH_MODEL + "Player/Player.mv1");


	/* BGM */
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_TITLE, PATH_BGM + "TitleBGM.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_GAME, PATH_BGM + "GameBGM.mp3");

	/* 効果音 */
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_CLICK, PATH_SE + "Click.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_SELECT, PATH_SE + "Select.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_DAMAGE, PATH_SE + "Damage.mp3");
}
void ResourceManager::_SetResource(Resource::LOAD_TYPE _loadType, SRC _src, std::string _path
								   , int _allNum, int _numX, int _numY)
{
	Resource* ret = new Resource();
	Resource res;
	if (_allNum == -1)
	{
		// その他読み込み
		resourcesMap_.emplace(_src, Resource(_loadType, _path));
	}
	else
	{
		// 複数画像読み込み
		resourcesMap_.emplace(_src,
			Resource(_loadType, _path, _allNum, _numX, _numY));
	}
	
}


void ResourceManager::Release(void)
{
	/* メモリ解放処理 */

	if (!resourcesMap_.empty())
	{
		// リソースリストをクリア(空の時は行わない)
		resourcesMap_.clear();
	}
	if (!loadedMap_.empty())
	{
		for (auto& [src, resource] : loadedMap_)
		{
			// 読み込み済みリソース解放
			resource->Release();
			delete resource;
		}

		// 読み込み済みリソースリストをクリア
		loadedMap_.clear();
	}
}
void ResourceManager::DestroyInstance(void)
{
	/*　インスタンス削除処理　*/
	instance_->Release();
	delete instance_;
}


Resource ResourceManager::Load(SRC _src)
{
	/* 読み込み処理 */
	Resource* res = _Load(_src);

	if (res == nullptr) return Resource();

	Resource ret = *res;

	return *res;
}
const int ResourceManager::LoadHandleId(SRC _src)
{
	// リソースの
	return Load(_src).GetHandleId();
}
void ResourceManager::LoadHandleIds(SRC _src, int* _target)
{
	// 複数画像ではない場合、処理終了
	if (resourcesMap_[_src].GetLoadType() != Resource::LOAD_TYPE::IMAGES) { return; }

	// 複数画像の対象にコピー
	Load(_src).CopyHandle(_target);

#ifdef _DEBUG
	if (*_target == -1)
	{
		OutputDebugString("\n複数画像が読み込まれませんでした。画像数/画像１枚のサイズ/画像パス名を確認してください。\n");
	}
#endif
}

std::string ResourceManager::GetHandlePath(SRC _src)
{
	return Load(_src).GetHandlePath();
}

Resource* ResourceManager::_Load(SRC src)
{
	// 読み込み済みリストを検索
	const auto& loaded = loadedMap_.find(src);

	//読み込み済みリストに対象がある時、要素を返す
	if (loaded != loadedMap_.end()) return loaded->second;


	// リソースリスト内を検索
	const auto& resource = resourcesMap_.find(src);

	// リソースリストに登録されてない時、NULLを返す
	if (resource == resourcesMap_.end()) return nullptr;


	// リソースリスト登録済み時、読み込み処理
	resource->second.Load();

	// 念のためにコピーコンストラクタ
	Resource* ret = new Resource(resource->second);

	// 読み込み済みリストに格納
	loadedMap_.emplace(src, ret);

	return ret;
}


int ResourceManager::LoadModelDuplicate(SRC src)
{
	/* 3Dモデル重複利用時の読み込み */

	// 読み込み処理
	Resource* resource = _Load(src);

	// 読み込み失敗
	if (resource == nullptr)
	{
		return -1;
	}

	// 重複するモデルのハンドルを取得
	int id = MV1DuplicateModel(resource->GetHandleId());

	// 重複モデルリストにハンドル追加
	resource->SetDuplicateModelId(id);

	return id;
}