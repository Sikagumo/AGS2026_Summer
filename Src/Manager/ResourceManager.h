#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{
	using RES_TYPE = Resource::TYPE;

public:

	// リソース名
	enum class SRC
	{
		// 画像
		IMG_TITLE,
		IMG_SHADOW,
		IMG_PUSH_SPACE,
		IMG_CONGRATULATIONS,

		// モデル
		MODEL_PLAYER,
		MODEL_RAT,
		MODEL_ROBOT,
		MODEL_VIEW_RANGE,
		MODEL_GOAL_STER,
		MODEL_PLANET_PITFALL,
		MODEL_PLANET_SPHERE,
		MODEL_STAGE_MAIN,
		MODEL_SKYDOME,

		// 外部アニメーション
		ANIM_PLAYER_IDLE,
		ANIM_PLAYER_RUN,
		ANIM_PLAYER_RUN_FAST,
		ANIM_PLAYER_ATTACK2,
		ANIM_PLAYER_ATTACK3,
		ANIM_PLAYER_FALLING,
		ANIM_PLAYER_JUMPING,
		ANIM_PLAYER_JUMP_ATTACK,
		ANIM_PLAYER_JUMP_HOVER,
		ANIM_PLAYER_JUMP_RESING,
		ANIM_PLAYER_SHOT,
		ANIM_PLAYER_VICTORY,
		ANIM_PLAYER_WARP,
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	

	int GetHandleId(SRC src) { return Load(src).handleId_; };
	int& GetHandleIds(SRC src) { return *Load(src).handleIds_; };

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// リソースのロード
	const Resource& Load(SRC src);

	// 内部ロード
	Resource& _Load(SRC src);

	// 画像読み込み
	void SetResourceImage(void);

	// モデル読み込み
	void SetResourceModel(void);

	// アニメーション読み込み
	void SetResourceAnim(void);

};
