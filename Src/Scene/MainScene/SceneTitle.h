#pragma once

#include <array>
#include <memory>

#include "../SceneBase.h"
#include "../../Common/Vector2.h"
#include "../../Object/Collider2D/Collider2DBase.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"

class SceneTitle : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneTitle(void);

	/// @brief デストラクタ
	~SceneTitle(void) override = default;

	/// @brief リソースの読み込み開始
	void Load(void) override;

	/// @brief リソースの読み込み完了処理
	void EndLoad(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 衝突の影響だけを更新する処理 
	void UpdateCollision(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

private:
	enum class MENU_ITEM 
	{
		SOLO,
		MULTI,
		OPTION,
		EXIT,
		COUNT
	};

	std::array<Collider2DBase::TAG_2D, 4> buttonTags;

	// メニュー数
	static constexpr int MENU_BUTTON_NUM = 4;

	// 画像関連
	int imageTitle_;                  // タイトルロゴの画像ハンドル
	std::array<int, 8> imageMenu_;    // メニュー項目（4つ）の画像ハンドル

	// 選択インデックス（パッド操作用）
	int selectedIdx_;

	// 前回のマウス座標
	Vector2F prevMousePos_;

	// 2D衝突判定関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;        // マウスカーソル用の円コライダー
	std::unique_ptr<Collider2DBox> soloPlayButtonCollider_;   // ソロプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> multiPlayButtonCollider_;  // マルチプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> optionButtonCollider_;     // 設定ボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> exitButtonCollider_;       // 終了ボタン用の矩形コライダー

	/// @brief デバック描画 
	void DrawDebug(void);
};