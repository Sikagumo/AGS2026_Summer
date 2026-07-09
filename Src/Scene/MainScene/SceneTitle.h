#pragma once

#include <array>
#include <memory>

#include "../SceneBase.h"
#include "../../Common/Vector2.h"
#include "../../Object/Collider2D/Collider2DBase.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"
#include "../../ImGUI/ShaderEditorComponent.h"
#include "../../Shader/ShaderMaterial.h"

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
	int peachHandle_;                 // 桃の画像
	int peachNormalHandle_;           // 桃のノーマルマップ
	int waveHandle_;                  // 波の画像
	int waveNormalHandle_;            // 波のノーマルマップ
	int oniSimaHandle_;               // 鬼ヶ島の画像
	int oniSimaNormalHandle_;         // 鬼ヶ島のノーマルマップ

	// 時間
	float time_;

	// 選択インデックス（パッド操作用）
	int selectedIdx_;

	// 前回のマウス座標
	Vector2F prevMousePos_;

	// マテリアル関連
	ShaderMaterial peachMaterial_;   // 桃のマテリアル
	ShaderMaterial waveMaterial_;    // 波のマテリアル
	ShaderMaterial oniSimaMaterial_; // 鬼ヶ島のマテリアル

	// 2. GUIコンポーネント
	std::shared_ptr<ShaderEditorComponent> peachGui_;
	std::shared_ptr<ShaderEditorComponent> waveGui_;
	std::shared_ptr<ShaderEditorComponent> oniSimaGui_;

	// 2D衝突判定関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;        // マウスカーソル用の円コライダー
	std::unique_ptr<Collider2DBox> soloPlayButtonCollider_;   // ソロプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> multiPlayButtonCollider_;  // マルチプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> optionButtonCollider_;     // 設定ボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> exitButtonCollider_;       // 終了ボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> peachCollider_;
	std::unique_ptr<Collider2DBox> waveCollider_;
	std::unique_ptr<Collider2DBox> oniSimaCollider_;

	int psHandle_;

	bool isSelectMenu_;

	/// @brief デバック描画 
	void DrawDebug(void);

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;

	/// @brief 
	void ProcessMenuState(void);
};