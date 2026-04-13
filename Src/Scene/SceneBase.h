#pragma once
class ResourceManager;
class SceneManager;
class SoundManager;
class NetManager;

class SceneBase
{

public:

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	// 初期化
	virtual void Init(void) = 0;

	// 更新
	virtual void Update(void) = 0;

	// 描画
	virtual void Draw(void) = 0;

	// 解放
	virtual void Release(void) = 0;

protected:

	int bgmHandleId_;

	// リソース管理
	ResourceManager& resMng_;

	// シーン管理
	SceneManager& sceneMng_;
	// サウンド管理
	SoundManager& soundMng_;
	// ネットワーク管理
	NetManager& netMng_;

};
