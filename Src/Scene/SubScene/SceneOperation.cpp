#include "SceneOperation.h"

#include <DxLib.h>

#include "../../Application.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1;             // 無効なリソースハンドルを示す値
	constexpr float UI_BACK_SCALE_X = 1.0f;        // 背景UIのX方向拡大率
	constexpr float UI_BACK_SCALE_Y = 1.5f;        // 背景UIのY方向拡大率
	constexpr unsigned int BLEND_ALPHA_HALF = 128;   // 半透明ブレンドのアルファ値
	constexpr float OPERATION_IMAGE_SCALE = 0.5f;    // 操作説明画像の表示スケール
	constexpr int BACKGROUND_COLOR_BLACK = 0x000000; // 背景を描画する黒色のカラーコード
}

SceneOperation::SceneOperation(void)
	: operationImageHandle_(INVALID_HANDLE)
	, backGroundHandle_(INVALID_HANDLE)
{
}

void SceneOperation::Load(void)
{
	operationImageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_OPERATION);
	backGroundHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_SELECT_UI_BACK);
}

void SceneOperation::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneOperation::Initialize(void)
{
}

void SceneOperation::Update(void)
{
	if (KeyConfInputManager::GetInstance().IsTriggerDown("CANCEL"))
	{
		SceneManager::GetInstance().PopScene();
	}
}

void SceneOperation::Draw(void)
{
	int graphWidth = 0;
	int graphHeight = 0;

	GetGraphSize(backGroundHandle_, &graphWidth, &graphHeight);

	// 背景を半透明の黒色で覆う
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_HALF);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BACKGROUND_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 背景UIを描画
	DrawRotaGraph3(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		graphWidth / 2,
		graphHeight / 2,
		UI_BACK_SCALE_X,
		UI_BACK_SCALE_Y,
		0.0f,
		backGroundHandle_,
		true
	);

	// 操作説明画像を描画
	DrawRotaGraph(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		OPERATION_IMAGE_SCALE,
		0.0f,
		operationImageHandle_,
		true
	);
}

void SceneOperation::Release(void)
{
}