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


	/* 画像関連 */
	// タイトルロゴの画像ハンドル
	int imageTitle_;
	
	// メニュー項目（4つ）の画像ハンドル
	std::array<int, 8> imageMenu_;

	// 桃の画像
	int peachHandle_;
	
	// 桃のノーマルマップ
	int peachNormalHandle_;

	// 波の画像
	int waveHandle_;
	
	// 波のノーマルマップ
	int waveNormalHandle_;
	
	// 鬼ヶ島の画像
	int oniSimaHandle_;
	
	// 鬼ヶ島のノーマルマップ
	int oniSimaNormalHandle_;

	int titleNormalHandle_;
	int backgroundHandle_;



	// 時間
	float time_;

	// 選択インデックス（パッド操作用）
	int selectedIdx_;
	int inputIntervalCounter_;

	// 前回のマウス座標
	Vector2F prevMousePos_;

	// マテリアル関連
	ShaderMaterial peachMaterial_;   // 桃のマテリアル
	ShaderMaterial waveMaterial_;    // 波のマテリアル
	ShaderMaterial oniSimaMaterial_; // 鬼ヶ島のマテリアル
	ShaderMaterial titleMaterial_;   // タイトルのマテリアル

	// 2. GUIコンポーネント
	std::shared_ptr<ShaderEditorComponent> peachGui_;
	std::shared_ptr<ShaderEditorComponent> waveGui_;
	std::shared_ptr<ShaderEditorComponent> oniSimaGui_;
	std::shared_ptr<ShaderEditorComponent> titleGui_;


	/* 2D衝突判定関連 */
	// マウスカーソル用の円コライダー
	std::unique_ptr<Collider2DCircle> cursorCollider_;
	
	// ソロプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> soloPlayButtonCollider_;
	
	// マルチプレイボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> multiPlayButtonCollider_;
	
	// 設定ボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> optionButtonCollider_;
	
	// 終了ボタン用の矩形コライダー
	std::unique_ptr<Collider2DBox> exitButtonCollider_;
	
	std::unique_ptr<Collider2DBox> peachCollider_;
	std::unique_ptr<Collider2DBox> waveCollider_;
	std::unique_ptr<Collider2DBox> oniSimaCollider_;

	bool isSelectMenu_;

	/// @brief UI初期化処理
	void InitUI(void);

	/// @brief デバック描画 
	void DrawDebug(void);

	/// @brief タイトルメニュー別遷移処理
	void ProcessMenuState(void);

	/// @brief Gui用の更新処理
	void UpdateGui(void) override;
};