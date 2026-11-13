#include "SceneOperation.h"

#include <DxLib.h>

#include "../../Application.h"
#include "../../Manager/Generic/KeyConfInputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"

namespace
{
	// マジックナンバー排除用の定数
	constexpr int INVALID_HANDLE = -1;               // 無効なリソースハンドルを示す値
	constexpr float UI_BACK_SCALE_X = 1.0f;          // 背景UIのX方向拡大率
	constexpr float UI_BACK_SCALE_Y = 1.5f;          // 背景UIのY方向拡大率
	constexpr unsigned int BLEND_ALPHA_HALF = 128;   // 半透明ブレンドのアルファ値
	constexpr float OPERATION_IMAGE_SCALE = 0.25f;    // 操作説明画像の表示スケール
	constexpr int BACKGROUND_COLOR_BLACK = 0x000000; // 背景を描画する黒色のカラーコード
	constexpr float SPLIT_RATIO_Y = 0.5f;            // 画像を上下に切る位置の割合（0.0f～1.0f）
}

SceneOperation::SceneOperation(void)
	: operationImageHandle_(INVALID_HANDLE)
	, backGroundHandle_(INVALID_HANDLE)
	, currentPageIndex_(0)
	, operationImageWidth_(0)
	, operationTotalHeight_(0)
	, splitPositionY_(0)
{
}

void SceneOperation::Load(void)
{
	operationImageHandle_ = ResourceManager::GetInstance().LoadHandleId(
		ResourceManager::SRC::IMG_OPERATION
	);
	backGroundHandle_ = ResourceManager::GetInstance().LoadHandleId(
		ResourceManager::SRC::IMG_SELECT_UI_BACK
	);
}

void SceneOperation::EndLoad(void)
{
	SceneBase::EndLoad();

	GetGraphSize(operationImageHandle_, &operationImageWidth_, &operationTotalHeight_);

	// 全体の高さに対して SPLIT_RATIO_Y の割合の位置を切る境界線にする
	splitPositionY_ = 1650;
}

void SceneOperation::Initialize(void)
{
	currentPageIndex_ = 0;
}

void SceneOperation::Update(void)
{
	auto& input = KeyConfInputManager::GetInstance();

	// OKが押されたら表示ページを切り替える（0 -> 1 -> 0）
	if (input.IsTriggerDown("OK"))
	{
		currentPageIndex_ = (currentPageIndex_ + 1) % PAGE_MAX;
	}

	if (input.IsTriggerDown("CANCEL"))
	{
		SceneManager::GetInstance().PopScene();
	}
}

void SceneOperation::Draw(void)
{
	int graphWidth = 0;  // 背景画像の横幅
	int graphHeight = 0; // 背景画像の縦幅

	GetGraphSize(backGroundHandle_, &graphWidth, &graphHeight);

	// 背景を半透明の黒色で覆う
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_HALF);
	DrawBox(
		0,
		0,
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		BACKGROUND_COLOR_BLACK,
		true
	);
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

	// ページに応じた切り出し開始Y座標と、切り出す縦幅を計算
	int sourceY = 0;    // 切り出し開始Y座標
	int clipHeight = 0; // 切り出す縦幅

	if (currentPageIndex_ == 0)
	{
		// 1ページ目（上側）：一番上(0) から 切る位置(splitPositionY_) まで
		sourceY = 0;
		clipHeight = splitPositionY_;
	}
	else
	{
		// 2ページ目（下側）：切る位置(splitPositionY_) から 一番下まで
		sourceY = splitPositionY_;
		clipHeight = operationTotalHeight_ - splitPositionY_;
	}

	// 操作説明画像の指定範囲を切り抜いて画面中央に描画
	DrawRectRotaGraph(
		Application::SCREEN_HALF_X,
		Application::SCREEN_HALF_Y,
		0,
		sourceY,
		operationImageWidth_,
		clipHeight,
		OPERATION_IMAGE_SCALE,
		0.0f,
		operationImageHandle_,
		true
	);
}

void SceneOperation::Release(void)
{
}