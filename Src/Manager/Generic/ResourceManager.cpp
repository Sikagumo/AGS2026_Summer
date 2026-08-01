#include "ResourceManager.h"
#include "Resource.h"
#include <DxLib.h>
#include <string>
#include <unordered_map>
#include "../../Application.h"
#include "../../Utility/UtilityMath.h"

ResourceManager* ResourceManager::instance_ = nullptr;

// リソースファイルのパス
#ifdef _DEBUG
const std::string PATH_DATA = "Data/";

// 暗号化済みのリソースフォルダパス
#else

//const std::string PATH_DATA = "_ResourceData/";
const std::string PATH_DATA = "Data/";
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
	
	// タイトル画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_TITLE, PATH_IMAGE + "Title.png");

	// タイトルのノーマルマップ
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_NOTMALMAP_TITLE, PATH_IMAGE + "Title_n.png");
	
	// 桃の画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_PEACH, PATH_IMAGE + "peach_T.png");

	// 桃のノーマルマップ画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_NOMALMAP_PEACH, PATH_IMAGE + "peach_n.png");

	// 波の画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_WAVE, PATH_IMAGE + "Wave.png");
	
	// 波のノーマルマップ
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_NOMALMAP_WAVE, PATH_IMAGE + "Wave_n.png");

	// 鬼ヶ島の画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_ONIGASIMA, PATH_IMAGE + "OnigaSima.png");

	// タイトル背景の画像
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_BUCGROUND_TITLE, PATH_MODEL + "SkyDome/Skydome.png");

	// 鬼ヶ島のノーマルマップ
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_NOMALMAP_ONIGASIMA, PATH_IMAGE + "OnigaSima_n.png");

	// 選択シーンの背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_BUCGROUND_SELCET, PATH_IMAGE + "JunbiSitu.png");

	// マルチシーンの背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_BUCGROUND_MALUTI, PATH_IMAGE + "MarlutiHeya.png");

	// セレクトUIの背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SELECT_UI_BACK, PATH_IMAGE + "selcetUIBack.png");

	// セレクト中UIの背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SELECTED_UI_BACK, PATH_IMAGE + "selcetedUIBack.png");

	// マルチ選択UI背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SELECT_MULTI_BACK, PATH_IMAGE + "waku.png");

	// マルチ選択中UI背景
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SELECTED_MULTI_BACK, PATH_IMAGE + "wakuPick.png");

	// 操作説明
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_OPERATION, PATH_IMAGE + "Operation.png");

	// チームロゴ
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_AICON, PATH_IMAGE + "Logo.png");

	//〇影
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SHADOW, PATH_IMAGE + "Shadow.png");
	//落下ポイント
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_FALLING_POINT , PATH_IMAGE + "FallingPoint.png");
	//リザルト
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_RESULT_WIN, PATH_IMAGE + "ResultWinBook.png");
	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_RESULT_BUT, PATH_IMAGE + "ResultBatBook.png");

	_SetResource(LOAD_TYPE::IMAGE, SRC::IMG_SOUSA, PATH_IMAGE + "sousa.png");

	/* 複数画像 */

	// 画像枚数
	int imagesAllNum = 0;

	// １画像の横枚数
	int imagesNumX = 0, imagesNumY = 0;

	imagesAllNum = 11;
	imagesNumX = 1;
	imagesNumY = 11;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TEXTS, PATH_IMAGE + "Text.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// タイマーテキスト
	imagesAllNum = 11;
	imagesNumX = 11;
	imagesNumY = 1;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TIMER, PATH_IMAGE + "TextsTime.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// ロックオン対象HPバー
	imagesAllNum = 2;
	imagesNumX = 1;
	imagesNumY = 2;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_HP_TARGET, PATH_IMAGE + "TargetHpBer.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// プレイヤー対象HPバー
	imagesAllNum = 2;
	imagesNumX = 1;
	imagesNumY = 2;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_HP_PLAYER, PATH_IMAGE + "PlayerHpBer.png"
				, imagesAllNum, imagesNumX, imagesNumY);


	// タイトル文字
	imagesAllNum = 8;
	imagesNumX = 1;
	imagesNumY = 8;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_TITLE_TEXT, PATH_IMAGE + "TextsTitle.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// 選択文字
	imagesAllNum = 18;
	imagesNumX = 2;
	imagesNumY = 9;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_SELECT, PATH_IMAGE + "SelectImages.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	// ゲームシーンの文字
	imagesAllNum = 1;
	imagesNumX = 1;
	imagesNumY = 1;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_GAME_TEXT, PATH_IMAGE + "TextsGame.png"
				, imagesAllNum, imagesNumX, imagesNumY);

	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_RESULT, PATH_IMAGE + "EndText.png"
				, imagesAllNum, imagesNumX, imagesNumY);
	
	// ロビーシーンの文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_LOBBY_UI_TEX, PATH_IMAGE + "sentakuTex.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// マルチ用の文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_MULTI_TEX, PATH_IMAGE + "junbi.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// 接続用の文字
	imagesAllNum = 2;
	imagesNumX = 1;
	imagesNumY = 2;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_CONECT_TEX, PATH_IMAGE + "butai.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// ポーズシーンの文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_POUSE_TEX, PATH_IMAGE + "pouzuNomal.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	// ポーズシーンの選択時の文字
	imagesAllNum = 4;
	imagesNumX = 1;
	imagesNumY = 4;
	_SetResource(LOAD_TYPE::IMAGES, SRC::IMGS_SELECT_PUSE_TEX, PATH_IMAGE + "pouzuRain.png"
		, imagesAllNum, imagesNumX, imagesNumY);

	/* エフェクト */
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_WAVE, PATH_EFFECT + "BossAttack/AttackWave.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_LANDING, PATH_EFFECT + "BossAttack/Landing.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_MG, PATH_EFFECT + "BossAttack/MGRotation.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_BOSS_HIT, PATH_EFFECT + "BossAttack/Hit.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_LASER, PATH_EFFECT + "BossAttack/Laser.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_MISSILE, PATH_EFFECT + "BossAttack/MissileExplosion.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_PLAYER_BLAST, PATH_EFFECT + "BlastHit/BlastHit.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_PLAYER_RECOVERY, PATH_EFFECT + "Recovery/Recovery.efkefc");
	_SetResource(LOAD_TYPE::EFFECT, SRC::EFFECT_PLAYER_POISON, PATH_EFFECT + "PoisonHit/PoisonHit.efkefc");


	/* モデル */
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_SKYDOME, PATH_MODEL + "SkyDome/SkyDome.mv1");

	// プレイヤーモデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER_HUMAN, PATH_MODEL + "Player/Chara/Human/PlayerHuman.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER_DOG,	PATH_MODEL + "Player/Chara/Dog/PlayerDog.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER_BIRD,	PATH_MODEL + "Player/Chara/Bird/PlayerBird.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_PLAYER_MONKEY,PATH_MODEL + "Player/Chara/Monkey/PlayerMonkey.mv1");

	// ステージモデル
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_STAGE, PATH_MODEL + "Stage/Stage.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_STAGE_COLLISION, PATH_MODEL + "Stage/StageCollision.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_TREE, PATH_MODEL + "Stage/Tree.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_TREE_POSITION, PATH_MODEL + "Stage/TreePosition.mv1");

	// ボスの武器本体系
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_FEET, PATH_MODEL + "Boss/oni_feet.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_CAR, PATH_MODEL + "Boss/oni_feet_car.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WHEEL_FRONT, PATH_MODEL + "Boss/oni_wheel_hedo.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WHEEL_BACK, PATH_MODEL + "Boss/oni_wheel_back.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_BODY, PATH_MODEL + "Boss/oni.body.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WEAPON_MG_L, PATH_MODEL + "Boss/oni_MG_L.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WEAPON_MG_R, PATH_MODEL + "Boss/oni_MG_R.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WEAPON_CN, PATH_MODEL + "Boss/oni_KN.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WEAPON_RK, PATH_MODEL + "Boss/oni_RK.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_WEAPON_RG, PATH_MODEL + "Boss/oni_RG.mv1");

	// ボスの弾系
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_BULLET, PATH_MODEL + "Boss/BBullet.mv1");
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_BOSS_MISSILE, PATH_MODEL + "Boss/missile.mv1");

	// 雑魚敵
	_SetResource(LOAD_TYPE::MODEL, SRC::MODEL_ENEMY_ROBO, PATH_MODEL + "Enemy/ninOni.mv1");

	/* アニメーション */
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_IDLE, PATH_ANIM + "Idle.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_RUN, PATH_ANIM + "Run.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_THROW_RUN, PATH_ANIM + "Throw_Run.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_THROW_LEFT, PATH_ANIM + "Throw_Left.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_THROW_RIGHT, PATH_ANIM + "Throw_Right.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_JUMP, PATH_ANIM + "Jump.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_DODGE, PATH_ANIM + "Rolling.mv1");
	_SetResource(LOAD_TYPE::ANIM, SRC::ANIM_DEFEAT, PATH_ANIM + "Defeat.mv1");

	/* BGM */
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_TITLE_SEA, PATH_BGM + "Sea.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_TITLE_THUNDER, PATH_BGM + "Thunderstorm.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::BGM_GAME, PATH_BGM + "GameBGM.mp3");

	/* 効果音 */
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_SELECT, PATH_SE + "Select.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_PBULLET_POISON, PATH_SE + "PoisonBulletHit.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_HIT_POISON, PATH_SE + "HitPoison.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_HIT_BLAST, PATH_SE + "HitBlast.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_PLAYER_DAMAGE, PATH_SE + "PlayerDamage.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_BOSS_MG_FIRE, PATH_SE + "MGFire.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_BOSS_LANDING, PATH_SE + "BossLanding.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_BOSS_ROAD, PATH_SE + "Road.mp3");
	_SetResource(LOAD_TYPE::SOUND, SRC::SE_LOBBY_SELCET, PATH_SE + "LobbySelect.mp3");
	

}
void ResourceManager::_SetResource(Resource::LOAD_TYPE _loadType, SRC _src, std::string _path
								   , int _allNum, int _numX, int _numY)
{
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


const int ResourceManager::LoadHandleIdsOnce(SRC _src, int _imageNum)
{
	return Load(_src).GetHandleImagesId(_imageNum);
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