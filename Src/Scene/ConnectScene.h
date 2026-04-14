#pragma once
#include "../Application.h"
#include "../Common/Vector2.h"
#include "SceneBase.h"

class ConnectScene : public SceneBase
{

public:

	// ボタンサイズ
	const int WIDTH = 200;
	const int HEIGHT = 30;

	int HX = Application::SCREEN_SIZE_X / 2;

	// ボタン位置
	const int B1_Y = 400;
	const Vector2 B1_S_POS = Vector2(HX - WIDTH / 2, B1_Y - HEIGHT / 2);
	const Vector2 B1_E_POS = Vector2(HX + WIDTH / 2, B1_Y + HEIGHT / 2);

	// コンストラクタ
	ConnectScene(void);

	// デストラクタ
	~ConnectScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// タイトル画像
	int imgTitle_;

	// スタート画像
	int imgStart_;

};
