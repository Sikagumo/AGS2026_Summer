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
		NONE,
		SOLO_PLAY_BUTTON,
		MULTI_PLAY_BUTTON,
		OPTION_BUTTON,
		EXIT_UTTON,
		SELECT_SINGLE_BOMB,
		SELECT_SINGLE_BIG,
		SELECT_SINGLE_RECOVERY,
		SELECT_SINGLE_RAPIDFIRE,
		SELECT_SKIN_HYMAN,
		SELECT_SKIN_DOG,
		SELECT_SKIN_MONKEY,
		SELECT_SKIN_BIRD,
		MOUSE_CURSOR,
		PAUSE_GAME_BUTTON,
		PAUSE_TITLE_BUTTON,
		PAUSE_YESNO_BOTTON,
		PAUSE_MAOUSE_CURSOR,
		PEACH,
		WAVE,
		ONI_GASHIMA,
		UI_MAIN_WEAPON,  // メイン画面：武器選択
		UI_MAIN_SKIN,    // メイン画面：スキン選択
		UI_MAIN_READY,   // メイン画面：任務開始（準備完了）
		UI_WINDOW_WEAPON,// ミニウィンドウ：武器項目
		UI_WINDOW_SKIN   // ミニウィンドウ：スキン項目
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