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

	static constexpr int IMG_TITLE_X = (Application::SCREEN_HALF_X);
	static constexpr int IMG_TITLE_Y = (Application::SCREEN_HALF_Y - 100);

	static constexpr int IMG_PUSH_X = (Application::SCREEN_HALF_X);
	static constexpr int IMG_PUSH_Y = (Application::SCREEN_HALF_Y + 175);

	static constexpr float SPHERE_SCALE = 0.7f;
	static constexpr float SPHERE_ROT_SPEED = 1.0f;
	static constexpr VECTOR SPHERE_ROT = {90.0f, 0.0f, 0.0f};
	static constexpr VECTOR SPHERE_POS = { -250.0f, -100.0f, -100.0f };

	static constexpr float PLAYER_SCALE = 0.4f;
	static constexpr VECTOR PLAYER_ROT = { 0.0f, -90.0f, 0.0f };
	static constexpr VECTOR PLAYER_ROT_LOCAL = { 0.0f, 180.0f, 0.0f };
	static constexpr VECTOR PLAYER_POS = { -250.0f, -32.0f, -105.0f };

	
	int imgShadow_;
	int imgTitle_;
	int imgPushSpace_;

	SkyDome* skyDome_;

	// スカイドーム用の空のトランスフォーム
	Transform empty_;


	Transform bigPlant_;
	Transform spherePlant_;
	Transform player_;
	AnimationController* playerAnim_;
};
