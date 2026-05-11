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
		

		// モデル
		

		// 外部アニメーション
		
		// エフェクト

		//サウンド
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

	// エフェクト読み込み
	void SetResourceEffect(void);

	// サウンド読み込み
	void SetResourceSound(void);

};
