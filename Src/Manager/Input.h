#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>
#include "../Common/Vector2.h"

class Input
{
public:

	// ゲームコントローラーの認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class JOYPAD_NO
	{
		KEY_PAD1,			// キー入力とパッド１入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		INPUT_KEY = 4096	// キー入力
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		OTHER = 0,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,
		MAX
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_BTN
	{
		RB_LEFT = 0,	// X
		RB_RIGHT,		// B
		RB_TOP,			// Y
		RB_BOTTOM,		// A

		R_TRIGGER,		// R_TRIGGER
		L_TRIGGER,		// L_TRIGGER
		R_BUTTON,		// R_BUTTON
		L_BUTTON,		// L_BUTTON
		START,			// START_BUTTON
		SELECT,			// SELECT_BUTTON

		CROSS_TOP,		// 十字上
		CROSS_DOWN,		// 十字下
		CROSS_LEFT,		// 十字左
		CROSS_RIGHT,	// 十字右

		L_STICK, // 左スティック
		R_STICK, // 右スティック

		MAX
	};

	enum class JOYPAD_STICK
	{
		L_STICK_UP,		//左スティック上
		L_STICK_DOWN,	//左スティック下
		L_STICK_LEFT,	//左スティック左
		L_STICK_RIGHT,	//左スティック右

		R_STICK_UP,		//右スティック上
		R_STICK_DOWN,	//右スティック下
		R_STICK_LEFT,	//右スティック左
		R_STICK_RIGHT,	//右スティック右
		MAX
	};

	enum class MOUSE
	{
		NONE = -1,

		CLICK_RIGHT,  // 右クリック
		CLICK_LEFT,	  // 左クリック
		CLICK_MIDDLE, // 中央クリック
		MOVE_LEFT,	  // 左移動
		MOVE_RIGHT,	  // 右移動
		MOVE_UP,	  // 上移動
		MOVE_DOWN,	  // 下移動
		WHEEL_FRONT,  // ホイール前(奥)回転
		WHEEL_BACK,	  // ホイール後ろ(手前)回転
		MAX
	};

	enum class CURSOR_MODE
	{
		NONE,     // 変更なし
		LOCKED,   // 画面中央固定
		CONFINED, // ウィンドウ内
		CONFINED_SIDE, // ウィンドウ内の反対側に行く
	};

	// ゲームコントローラーの入力情報
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		unsigned char ButtonsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgDown[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgUp[static_cast<int>(JOYPAD_BTN::MAX)];
		int AKeyLX;
		int AKeyLY;
		int AKeyRX;
		int AKeyRY;
	};

	// コンストラクタ
	Input(void);

	// デストラクタ
	~Input(void) = default;


	void Init(void);
	void Update(void);

	// リソースの破棄
	void Release(void);

	// 判定を行うキーを追加
	void Add(int key);

	// 判定を行うキーをクリア
	void Clear(void);

	// キーの押下判定
	bool IsNew(int key) const;

	// キーの押下判定(押しっぱなしはNG)
	bool IsTrgDown(int key) const;

	// キーを離した時の判定
	bool IsTrgUp(int key) const;

	// マウス位置設定
	void SetMousePos(const Vector2& pos);

	// マウス座標の取得
	Vector2 GetMousePos(void) const { return mousePos_;  };

	// マウスの移動距離を取得
	Vector2 GetMousePosDistance(void)const;

	// マウスのクリック状態を取得(MOUSE_INPUT_LEFT、RIGHT)
	int GetMouse(void) const { return mouseInput_;  };

	// マウスが左クリックされたか
	bool IsClickMouseLeft(void)const { return (mouseInput_ == MOUSE_INPUT_LEFT); };

	// マウスが右クリックされたか
	bool IsClickMouseRight(void) const { return (mouseInput_ == MOUSE_INPUT_RIGHT); };

	// カーソル状態変更
	void SetCursorMode(CURSOR_MODE _mode) { cursorMode_ = _mode;  };

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);

	// ボタンが押された
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const;

	// スティックが倒されたか
	bool IsStickNew(JOYPAD_NO no, JOYPAD_STICK stick) const;
	bool IsStickDown(JOYPAD_NO no, JOYPAD_STICK stick) const;
	bool IsStickUp(JOYPAD_NO no, JOYPAD_STICK stick) const;	
	
	// マウスが押されたか
	bool IsMouseNew(MOUSE mouse) const;
	bool IsMouseTrgUp(MOUSE mouse) const;
	bool IsMouseTrgDown(MOUSE mouse) const;

private:

	//スティックの猶予
	static constexpr int STICK_THRESHOLD = 300;

	// キー情報
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// スティック情報
	struct StickInfo
	{
		JOYPAD_STICK key;
		bool keyOld = false;
		bool keyNew = false;
		bool keyTrgDown = false;
		bool keyTrgUp   = false;
	};

	// マウス
	struct MouseInfo
	{
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;

	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// キー情報
	std::map<int, Input::Info> keyInfos_;
	Input::Info infoEmpty_;

	// マウス情報
	std::map<MOUSE, Input::MouseInfo> mouseInfos_;
	Input::MouseInfo mouseInfoEmpty_;

	// カーソル状態
	CURSOR_MODE cursorMode_;

	// スティック情報
	std::map<JOYPAD_NO, std::vector<Input::StickInfo>> stickInfos_;

	// マウスカーソルの位置
	Vector2 mousePrePos_;
	Vector2 mousePos_;

	//マウスホイール回転量
	int wheelRot_;

	// マウスボタンの入力状態
	int mouseInput_;

	// パッド情報
	JOYPAD_IN_STATE padInfos_[5];

	void UpdateMouse(void);

	// 配列の中からキー情報を取得する
	const Input::Info& Find(int key) const;

	// 配列の中からマウス情報を取得する
	const Input::MouseInfo& FindMouse(MOUSE key) const;

	// スティックの倒れ具合を取得する
	int PadStickOverSize(const JOYPAD_NO no, const JOYPAD_STICK stick);

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadType(JOYPAD_NO no);

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	// コントローラの入力情報を更新する
	void SetJPadInState(JOYPAD_NO jpNo);

};

