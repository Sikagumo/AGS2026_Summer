#pragma once

/// @brief 2Dコライダーの共通親クラス
class Collider2DBase
{
public:

	/// @brief 2D形状の種別
	enum class SHAPE_2D
	{
		CIRCLE,  // 円
		BOX,     // 矩形
	};

	/// @brief 衝突の識別タグ
	enum class TAG_2D
	{
		NONE,                    // 未設定
		SOLO_PLAY_BUTTON,        // ソロプレイボタン
		MULTI_PLAY_BUTTON,       // マルチプレイボタン
		OPTION_BUTTON,           // オプションボタン
		EXIT_BUTTON,             // 終了ボタン
		SELECT_SINGLE_BOMB,      // 武器選択：ボム
		SELECT_SINGLE_BIG,       // 武器選択：ビッグ弾
		SELECT_SINGLE_RECOVERY,  // 武器選択：回復
		SELECT_SINGLE_RAPIDFIRE, // 武器選択：連射
		SELECT_SKIN_HUMAN,       // スキン選択：人間
		SELECT_SKIN_DOG,         // スキン選択：犬
		SELECT_SKIN_MONKEY,      // スキン選択：猿
		SELECT_SKIN_BIRD,        // スキン選択：鳥
		MOUSE_CURSOR,            // マウスカーソル
		PAUSE_GAME_BUTTON,       // ポーズ画面：ゲームに戻るボタン
		PAUSE_TITLE_BUTTON,      // ポーズ画面：タイトルに戻るボタン
		PAUSE_YESNO_BUTTON,      // ポーズ画面：確認のYes/Noボタン
		PAUSE_MOUSE_CURSOR,      // ポーズ画面：ポーズ中のマウスカーソル
		PEACH,                   // 桃オブジェクト
		WAVE,                    // 衝撃波オブジェクト
		ONI_GASHIMA,             // 鬼ヶ島オブジェクト
		UI_MAIN_WEAPON,          // メイン画面：武器選択
		UI_MAIN_SKIN,            // メイン画面：スキン選択
		UI_MAIN_READY,           // メイン画面：任務開始
		UI_WINDOW_WEAPON,        // ミニウィンドウ：武器項目
		UI_WINDOW_SKIN,          // ミニウィンドウ：スキン項目
	};

	/// @brief コンストラクタ
	/// @param _shapeType 2D形状の種類
	/// @param _collisionTag 衝突の2D識別タグ
	Collider2DBase(SHAPE_2D _shapeType, TAG_2D _collisionTag);

	/// @brief デストラクタ
	virtual ~Collider2DBase(void) = default;

	/// @brief 2D形状タイプの取得
	/// @return 2D形状タイプ
	SHAPE_2D GetShapeType(void) const;

	/// @brief 2Dタグの取得 
	/// @return 衝突2Dタグ
	TAG_2D GetCollisionTag(void) const;

	/// @brief デバッグ描画
	/// @param _color 描画色
	virtual void DrawDebug(int _color) const = 0;

protected:

	// コライダー設定関連
	SHAPE_2D shapeType_;      // 2D形状の種別
	TAG_2D collisionTag_;     // 衝突の2D識別タグ
};