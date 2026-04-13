#pragma once
#include "SceneBase.h"
#include "../Application.h"
#include "../Object/Common/Transform.h"
class AnimationController;
class SkyDome;

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	

	
	

	SkyDome* skyDome_;

	// スカイドーム用の空のトランスフォーム
	Transform empty_;


	Transform bigPlant_;
	Transform spherePlant_;
	Transform player_;
	AnimationController* playerAnim_;
};
