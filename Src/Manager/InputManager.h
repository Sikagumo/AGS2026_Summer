#pragma once
#include "../Common/Vector2.h"
#include <unordered_map>
#include <DxLib.h>

class InputManager
{
public:

	/// @brief 入力状態
	enum class INPUT_TYPE
	{
		KEYBOARD_ONLY,
		KEYBOARD_MOUSE,
		CONTROLLER,
		MAX,
	};

	/// @brief コントローラー認識番号
	enum class PAD_NO
	{
		PAD_KEY,
		PAD1,			  // パッド１入力
		PAD2,             // パッド２入力
		PAD3,             // パッド３入力
		PAD4,             // パッド４入力
		INPUT_KEY = 4096, // キー入力
	};

	/// @brief ゲームコントローラータイプ
	enum class JOYPAD_TYPE
	{
		OTHER = 0,		  // その他
		XBOX_360,		  // XBOX 360
		XBOX_ONE,		  // XBOX ONE
		DUAL_SHOCK_4,
		DUAL_SENSE,		  // PlayStationコントローラ
		SWITCH_JOY_CON_L, // スイッチの右コントローラー
		SWITCH_JOY_CON_R, // スイッチの左コントローラー
		SWITCH_PRO_CTRL,  // スイッチプロコン
		MAX
	};


	/// @brief ゲームコントローラーボタン
	enum class PAD_BTN
	{
		DOWN = 0,  // A
		RIGHT,     // B
		LEFT,      // X
		UP,        // Y
		L_BUTTON,  // L
		R_BUTTON,  // R
		BACK,      // BACK
		START,     // START
		L_TRIGGER, // LT
		R_TRIGGER, // RT
		L_STICK, // Lスティック
		R_STICK, // Rスティック

		MAX	// 要素数
	};

	/// @brief ゲームコントローラスティック
	enum class JOYPAD_ALGKEY
	{
		LEFT = 0, // 左スティック
		RIGHT,    // 右スティック
		D_PAD,    // 十字キー

		MAX
	};


private:

	/// @brief キー情報
	struct Key
	{
		int keyType;   // キーの種類
		bool inputOld; // 1フレーム前の入力状態
		bool inputNew; // 現フレームの入力状態
		bool trgDown;  // 現フレームで入力されたか否か
		bool trgUp;	   // 現フレームで入力が終了したか否か
	};

	/// @brief  マウス情報
	struct Mouse
	{
		int type;	   // 入力の種類
		bool inputOld; // 1フレーム前の入力状態
		bool inputNew; // 現フレームの入力状態
		bool trgDown;  // 現フレームで入力されたか否か
		bool trgUp;	   // 現フレームで入力が終了したか否か
	};

	/// @brief ゲームコントローラー入力情報
	struct Joypad
	{
		// ボタンの前フレーム入力判定
		unsigned char buttonOld[static_cast<int>(PAD_BTN::MAX)];

		// ボタンの現在フレーム入力判定
		unsigned char buttonNew[static_cast<int>(PAD_BTN::MAX)];

		bool isOld[static_cast<int>(PAD_BTN::MAX)];
		bool isNew[static_cast<int>(PAD_BTN::MAX)];

		// 入力時の判定
		bool isTrgDown[static_cast<int>(PAD_BTN::MAX)];

		// 離した時の判定
		bool isTrgUp[static_cast<int>(PAD_BTN::MAX)];


		// スティックの横入力の傾き具合
		int algKeyX[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		// スティックの縦入力の傾き具合
		int algKeyY[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		unsigned int dPad;

		// スティックの前フレームの入力判定
		bool isOldAlgKey[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		// スティックの現在フレーム入力判定
		bool isNewAlgKey[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		// スティックの入力時の判定
		bool isTrgDownAlgKey[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		// スティックを離した時の判定
		bool isTrgUpAlgKey[static_cast<int>(JOYPAD_ALGKEY::MAX)];

		// 振動判定
		bool isVibration;

		// 振動の強さ(0～100)
		unsigned int vibrationPow;

		// 振動の時間
		float vibrationTime;
	};


public:

	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float ALGKEY_THRESHOLD = 0.5f;

	// 十字キーの入力受付しきい値(0.0～1.0)
	static constexpr float DPAD_THRESHOLD = (1.0f * 4500.0f);
	
	// マウスホイール移動最大
	static constexpr int MOUSE_WHEEL_MAX = 2;


	using KeyMap    = std::unordered_map<int, InputManager::Key>;
	using MouseMap  = std::unordered_map<int, InputManager::Mouse>;


	/// @brief インスタンスを明示的に生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return 入力マネージャ
	static InputManager& GetInstance(void);


	/// @brief 初期化処理
	void Init(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief インスタンス削除
	void Destroy(void);

	void InitInput(void);

#pragma region キーボード入力

	/// @brief 判定を行うキーを追加
	/// @param _type 追加対象
	void AddKey(unsigned int _type);

	/// @brief キーの入力判定
	/// @param _keyType 判定するキー
	/// @return 入力しているか否か
	bool KeyIsNew(unsigned int _keyType) const { return FindKey(_keyType).inputNew; };

	/// @brief キーを入力した瞬間の判定
	/// @param _keyType 判定するキー
	/// @return キー入力したか否か
	bool KeyIsTrgDown(unsigned int _keyType) const { return FindKey(_keyType).trgDown; };

	/// @brief キーを離した瞬間の判定
	/// @param _keyType 判定するキー
	/// @return キーを離したか否か
	bool KeyIsTrgUp(unsigned int _keyType) const { return FindKey(_keyType).trgUp; };

	/// @brief 全てのキーの入力判定
	bool KeyIsNewAll(void) const;

#pragma endregion

#pragma region マウス入力

	/// @brief 判定を行うキーを追加
	/// @param _type 追加対象
	void AddMouse(unsigned int _type);

	/// <summary>
	/// マウス座標の取得
	/// </summary>
	/// <returns>マウス座標</returns>
	Vector2 GetMousePos(void) const;

	/// @brief マウス入力処理
	/// @param _mouseType 判定するマウスキー
	/// @return クリックしているか否か
	bool MouseIsNew(unsigned int _mouseType) const { return FindMouse(_mouseType).inputNew; };

	/// @brief マウスクリックを入力した瞬間の判定
	/// @param _mouseType 判定するマウスキー
	/// @return クリックしたか否か
	bool MouseIsTrgDown(unsigned int _mouseType) const { return FindMouse(_mouseType).trgDown; };

	/// @brief マウスクリックを離した瞬間の判定
	/// @param _mouseType 判定するマウスクリックの種類
	/// @return クリックを離したか否か
	bool MouseIsTrgUp(unsigned int _mouseType) const { return FindMouse(_mouseType).trgUp; };

	/// <summary>
	/// マウスの回転量を取得(正の値：上(奥)方向)
	/// </summary>
	int GetMouseWheelRot(void);

#pragma endregion

#pragma region コントローラー入力

	/// <summary>
	/// コントローラの入力判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnNew(PAD_NO _padNum, PAD_BTN button) const;
	/// <summary>
	/// コントローラの入力判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnNew(int _padNum, int button) const;

	/// <summary>
	/// コントローラのキーを押したか判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnTrgDown(PAD_NO _padNum, PAD_BTN button) const;
	/// <summary>
	/// コントローラのキーを押したか判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnTrgDown(int _padNum, int button) const;

	/// <summary>
	/// コントローラのキーを離したか判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnTrgUp(PAD_NO _padNum, PAD_BTN button) const;
	/// <summary>
	/// コントローラのキーを離したか判定
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="button">コントローラ番号</param>
	bool PadIsBtnTrgUp(int _padNum, int button) const;

	/// @brief コントローラのスティックの入力判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティックの種類
	bool PadIsAlgKeyNew(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;
	/// @brief コントローラのスティックの入力判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティック番号
	bool PadIsAlgKeyNew(int _padNum, int _algKey)const;

	/// @brief コントローラのスティックを入力した時の判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティックの種類
	bool PadIsAlgKeyTrgDown(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;
	/// @brief コントローラのスティックを入力した時の判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティックの種類
	bool PadIsAlgKeyTrgDown(int _padNum, int _algKey)const;

	/// @brief コントローラのスティックを離した時の判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティックの種類
	bool PadIsAlgKeyTrgUp(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;
	/// @brief コントローラのスティックを離した時の判定
	/// @param _padNum コントローラの対象
	/// @param _algKey スティックの種類
	bool PadIsAlgKeyTrgUp(int _padNum, int _algKey)const;

	/// <summary>
	/// コントローラのスティック横移動量を取得
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="_algKey">スティック番号</param>
	int PadAlgKeyX(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;
	/// <summary>
	/// コントローラのスティック横移動量を取得
	/// </summary>
	/// <param name="_padNum">コントローラの対象</param>
	/// <param name="_algKey">スティック番号</param>
	int PadAlgKeyX(int _padNum, int _algKey)const;

	/// @brief コントローラのスティック縦移動量を取得
	/// @param _padNum コントローラの対象
	/// @param _algKey スティック番号
	int PadAlgKeyY(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;
	/// @brief コントローラのスティック縦移動量を取得
	/// @param _padNum コントローラの対象
	/// @param _algKey スティック番号
	int PadAlgKeyY(int _padNum, int _algKey)const;

	/// @brief コントローラのスティックの傾き方向を取得(XZ平面)
	/// @param _padNum コントローラの対象
	/// @param _algKey スティック番号
	VECTOR GetAlgKeyDirXZ(PAD_NO _padNum, JOYPAD_ALGKEY _algKey)const;

	/// @brief 入力状態割り当て
	void SetInputType(INPUT_TYPE _type) { inputType_ = _type; };

	/// @brief 入力状態取得
	INPUT_TYPE GetInputType(void)const { return inputType_; };
	

#pragma endregion


private:

	// シングルトンインスタンス
	static InputManager* instance_;

	// アナログキーの最大値
	static constexpr float ALGKEY_VAL_MAX = 1000.0f;

	// 入力状態
	INPUT_TYPE inputType_;


#pragma region キーボードのメンバ変数
	// キー情報配列
	KeyMap keys_;
	InputManager::Key keyInfoEmpty_;
#pragma endregion


	/*　マウスのメンバ変数　*/

	// マウス情報配列
	
	MouseMap mouses_;
	InputManager::Mouse mouseInfoEmpty_;

	Vector2 mousePos_; // マウスカーソル位置
	int mouseInput_;   // マウスボタン入力状態	

	// マウスホイール回転量
	Mouse mouseWheel_;


	/*　コントローラのメンバ変数　*/

	// パッド情報
	Joypad padInfos_[static_cast<int>(PAD_NO::PAD4)];

	// DirectInputの入力状態
	DINPUT_JOYSTATE joyDInState_;

	// コントローラボタンの入力状態
	XINPUT_STATE joyXInState_;


	/// @brief デフォルトコンストラクタ
	InputManager(void);

	/// @brief デフォルトデストラクタ
	~InputManager(void) = default;

	// コピーコンストラクタ対策
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;


	/// <summary>
	/// 配列内から対象のキーの入力を取得
	/// </summary>
	/// <param name="keyType">探索するキーの種類</param>
	/// <returns>キーの情報(定数)</returns>
	const InputManager::Key& FindKey(unsigned int keyType)const;

	/// @brief 判定する入力キーを割り当て
	void SetInputKey(void);

#pragma region マウスメンバ変数

	/// <summary>
	/// 配列内から対象のマウスボタンの入力を取得
	/// </summary>
	/// <param name="keyType">探索するマウスボタンの種類</param>
	/// <returns>マウスボタンの情報(定数)</returns>
	const InputManager::Mouse& FindMouse(unsigned int mouseType) const;

	/// @brief 判定するマウスを割り当て
	void SetInputMouse(void);

#pragma endregion

#pragma region コントローラ識別

	/// @brief コントローラの入力情報
	/// @param _padNum コントローラの対象
	Joypad GetPadInputState(PAD_NO _padNum);

	/// <summary>
	/// 接続されたコントローラを識別して取得
	/// </summary>
	/// <param name="_padNum">判定するコントローラ番号</param>
	/// <returns>コントローラ識別情報</returns>
	JOYPAD_TYPE GetPadType(PAD_NO _padNum);

	/// <summary>
	/// DirectInputの入力取得
	/// </summary>
	/// <param name="_padNum">判定するコントローラ番号</param>
	/// <returns>DirectInput入力情報</returns>
	DINPUT_JOYSTATE GetPadDInputState(PAD_NO _padNum);

	/// <summary>
	/// コントローラボタンの入力取得
	/// </summary>
	/// <param name="_padNum">判定するコントローラ番号</param>
	/// <returns>コントローラボタン入力情報</returns>
	XINPUT_STATE GetPadXInputState(PAD_NO _padNum);

	/// <summary>
	/// コントローラを識別して取得
	/// </summary>
	/// <param name="jpadNum">判定するコントローラ番号</param>
	/// <returns>コントローラ識別情報</returns>
	void SetPadInState(PAD_NO jpadNum);

	/// <summary>
	/// 接続されたコントローラの識別を取得
	/// </summary>
	/// <param name="jPadNo"></param>
	JOYPAD_TYPE GetJPadType(PAD_NO jPadNo);

#pragma endregion
};