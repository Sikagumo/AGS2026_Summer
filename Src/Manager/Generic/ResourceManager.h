#pragma once

#include <unordered_map>
#include <string>
#include "./Resource.h"

class ResourceManager
{
public:

	/// @brief リソース識別列挙
	enum class SRC
	{
		NONE = -1,

		/* 単一画像 */
		IMG_TITLE,
		IMG_PEACH,
		IMG_NOMALMAP_PEACH,
		IMG_WAVE,
		IMG_NOMALMAP_WAVE,
		IMG_ONIGASIMA,
		IMG_NOMALMAP_ONIGASIMA,
		IMG_SHADOW,
		IMG_FALLING_POINT,
		IMG_NOTMALMAP_TITLE,
		IMG_BUCGROUND_TITLE,

		/* 複数画像 */
		IMGS_TEXTS,
		IMGS_TIMER,
		IMGS_TITLE_TEXT,
		IMGS_SELECT,
		IMGS_GAME_TEXT,
		IMGS_HP_TARGET,
		IMGS_HP_PLAYER,
		IMGS_RESULT,

		/* モデル */
		MODEL_SKYDOME,

		MODEL_PLAYER_HUMAN,
		MODEL_PLAYER_DOG,
		MODEL_PLAYER_MONKEY,
		MODEL_PLAYER_BIRD,

		MODEL_BOSS_FEET,
		MODEL_BOSS_CAR,
		MODEL_BOSS_WHEEL_FRONT,
		MODEL_BOSS_WHEEL_BACK,
		MODEL_BOSS_BODY,
		MODEL_BOSS_WEAPON_MG_L,
		MODEL_BOSS_WEAPON_MG_R,
		MODEL_BOSS_WEAPON_CN,
		MODEL_BOSS_WEAPON_RG,
		MODEL_BOSS_WEAPON_RK,

		MODEL_BOSS_BULLET,
		MODEL_BOSS_MISSILE,

		MODEL_ENEMY_ROBO,

		MODEL_STAGE,
		MODEL_STAGE_COLLISION,
		MODEL_TREE_POSITION,

		/* 外部アニメーション */
		ANIM_IDLE,
		ANIM_RUN,
		ANIM_THROW_RUN,
		ANIM_THROW_LEFT,
		ANIM_THROW_RIGHT,
		ANIM_JUMP,
		ANIM_DODGE,
		ANIM_DEFEAT,

		/* エフェクト */
		EFFECT_WAVE,
		EFFECT_LANDING,
		EFFECT_BOSS_HIT,
		EFFECT_MG,
		EFFECT_LASER,
		EFFECT_MISSILE,
		EFFECT_PLAYER_BLAST,

		/* 映像 */

		/* BGM */
		BGM_TITLE_SEA,
		BGM_TITLE_THUNDER,
		BGM_GAME,

		/* 効果音 */
		SE_SELECT,
		SE_PLAYER_DAMAGE,
		SE_BOSS_MG_FIRE,
		SE_BOSS_LANDING,
		SE_BOSS_ROAD,
		SE_BOSS_HIT,
	};


	/*　各ハンドルパス　*/

	// エフェクトファイルパス
	static const std::string PATH_EFFECT;

	// 画像ファイルパス
	static const std::string PATH_IMAGE;

	// 3Dモデルファイルパス
	static const std::string PATH_MODEL;

	// アニメーションファイルパス
	static const std::string PATH_ANIM;

	// BGMファイルパス
	static const std::string PATH_BGM;

	// 効果音ファイルパス
	static const std::string PATH_SE;

	// 動画ファイルパス
	static const std::string PATH_MOVIE;


	static void CreateInstance(void);
	static ResourceManager& GetInstance(void) { return *instance_; };
	void DestroyInstance(void);

	void Initialize(void);


	/// @brief リソースのロード
	/// @param _src 読み込み対象
	Resource Load(SRC _src);

	/// @brief リソースのハンドルを取得
	/// @param _src 読み込み対象
	const int LoadHandleId(SRC _src);
	const int LoadHandleId(int _src) { return LoadHandleId(static_cast<SRC>(_src)); };

	/// @brief 複数画像のハンドルを画像ハンドルに割り当て
	/// @param _src 読み込み対象
	/// @param _target 複数画像配列の先頭パス
	void LoadHandleIds(SRC _src, int* _target);

	/// @brief 複数画像のハンドルを画像ハンドルの部分割り当て
	/// @param _src 複数画像の読み込み対象
	/// @param _imageNum 画像対象
	const int LoadHandleIdsOnce(SRC _src, int _imageNum);

	/// @brief 3Dモデル重複利用時の読み込み
	/// @param _src 読み込み対象
	int LoadModelDuplicate(SRC _src);

	/// @brief リソースのハンドルを取得
	/// @param _src 読み込み対象
	std::string GetHandlePath(SRC _src);
	std::string GetHandlePath(int _src) { return GetHandlePath(static_cast<SRC>(_src)); };


private:

	//静的インスタンス
	static ResourceManager* instance_;

	// リソース管理対象
	std::unordered_map<SRC, Resource> resourcesMap_;

	// 読み込み済みリソース
	std::unordered_map<SRC, Resource*> loadedMap_;


	/// @brief 解放処理
	void Release(void);


	ResourceManager(void);
	~ResourceManager(void) = default;

	/* コピーコンストラクタ対策 */
	ResourceManager(const ResourceManager&)			   = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&)			  = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;
	

	/// @brief リソース取得処理(複数画像)
	/// @param _loadType 読み込み方法
	/// @param _src リソース対象
	/// @param _path パス
	/// @param _allNum 画像の数
	/// @param _numX 最大横画像数
	/// @param _numY 最大縦画像数
	void _SetResource(Resource::LOAD_TYPE _loadType, SRC _src, std::string _path
					  , int _allNum = -1, int _numX = -1, int _numY = -1);

	/// @brief 内部ロード処理
	/// @param _source 読み込み対象
	Resource* _Load(SRC _source);
};