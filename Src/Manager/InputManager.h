#pragma once
#include <map>
#include <functional>
#include <vector>
#include <memory>
#include "../Common/Vector2.h"
#include "Input.h"

class InputManager
{
public:

	/// @brief 入力状況の種類
	enum class TYPE
	{
		NONE,

		PLAYER_MOVE_RIGHT, // 移動右
		PLAYER_MOVE_LEFT,  // 移動左
		PLAYER_MOVE_UP,  // 移動上
		PLAYER_MOVE_DOWN, // 移動下
		PLAYER_CHANGE, // 位置移動
		
		CAMERA_MOVE_LEFT,  // カメラ左移動
		CAMERA_MOVE_RIGHT, // カメラ右移動
		CAMERA_MOVE_UP,	   // カメラ上移動
		CAMERA_MOVE_DOWN,  // カメラ下移動

		GAME_STATE_CHANGE, // ゲーム状態遷移

		SELECT_RIGHT,	   // 選択右
		SELECT_LEFT,	   // 選択左
		SELECT_DOWN,	   // 選択下
		SELECT_UP,		   // 選択上
		SELECT_DECISION,   // 選択決定
		SELECT_CANCEL,	   // 選択キャンセル

		DEBUG_SCENE_CHANGE, // デバッグシーン遷移
		DEBUG_MODE,			// デバッグモード
		DEBUG_POINT_SET,	// デバッグ位置割り当て
		DEBUG_POINT_DELETE, // デバッグ位置削除

		PAUSE,					// ポーズ(開閉)

		MAX,
	};


	/// @brief インスタンス生成(初回のみ) 
	static void CreateInstance(void);

	/// @brief 入力マネージャ取得 
	static InputManager& GetInstance(void) { return *instance_; };


	/// @brief 初期化処理
	void Init(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 解放処理
	void Release(void);

	/// @brief リソースの破棄
	static void Destroy(void);


	/// @brief 指定した状況での各入力機器の押下判定
	/// @param _type 状況の種類
	/// @param _padNo パッド番号(デフォルトはPAD1)
	/// @returns true=[入力中], false=[未入力]
	bool IsNew(const TYPE _type, const Input::JOYPAD_NO _padNo = Input::JOYPAD_NO::PAD1);

	/// @brief 指定した状況での各入力機器の押下判定(最初のみ)
	/// @param _type 状況の種類
	/// @param padNo パッド番号(デフォルトはPAD1)
	/// @returns true=[入力中], false=[未入力]
	bool IsTrgDown(const TYPE _type, const Input::JOYPAD_NO _padNo = Input::JOYPAD_NO::PAD1);

	/// @brief 指定した状況での各入力機器の離した判定
	/// @param _type 状況の種類
	/// @param _padNo パッド番号(デフォルトはPAD1)
	/// @returns true=[離したタイミング], false=[まだ押下中]
	bool IsTrgUp(const TYPE _type, const Input::JOYPAD_NO _padNo = Input::JOYPAD_NO::PAD1);

	/// @brief マウス位置を設定
	void SetMousePos(const Vector2& _pos);

	/// @brief カーソル状態遷移
	void SetCursorMode(Input::CURSOR_MODE _mode);

	/// @brief マウス座標の取得
	Vector2 GetMousePos(void) const;

	/// @brief マウスの移動量 取得
	Vector2 GetMouseMove(void) const;

	/// @brief 左スティックの倒れた度合いを取得
	/// @param _num パッド番号
	Vector2 GetKnockLStickSize(Input::JOYPAD_NO _num = Input::JOYPAD_NO::PAD1) const;
	
	/// @brief 右スティックの倒れた度合いを取得
	/// @param _num パッド番号
	Vector2 GetKnockRStickSize(Input::JOYPAD_NO _num = Input::JOYPAD_NO::PAD1) const;

	/// @brief 左スティックの倒れた方向を取得
	/// @param _num パッド番号
	/// @param _threshold パッドの傾き制限
	VECTOR GetDirXY_LStick(Input::JOYPAD_NO _num = Input::JOYPAD_NO::PAD1, float _threshold = STICK_THRESHOLD) const;
	
	/// @brief 右スティックの倒れた方向を取得
	/// @param _num パッド番号
	/// @param _threshold パッドの傾き制限
	VECTOR GetDirXY_RStick(Input::JOYPAD_NO _num = Input::JOYPAD_NO::PAD1, float _threshold = STICK_THRESHOLD) const;


private:

	static constexpr bool MOUSE_VISIBLE = FALSE;
	static constexpr float STICK_THRESHOLD = 1000.0f;
	static constexpr float MOUSE_THRESHOLD = 100.0f;

	static InputManager* instance_;


	// 入力トリガーの情報
	struct TriggerInfo
	{
		// キーボード
		std::vector<int> keys;

		// パッドのボタン
		std::vector<Input::JOYPAD_BTN> padButtons;

		// パッドのスティック
		std::vector<Input::JOYPAD_STICK> padSticks;

		// マウス
		Input::MOUSE mouse = Input::MOUSE::MAX;
	};

	// 入力判定クラス
	std::unique_ptr<Input> input_;

	// 入力状況別のトリガーを管理するマップ
	std::map<TYPE, TriggerInfo> triggerMap_;

	// 入力状況に応じた押下処理を管理するマップ
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcNewMap_;
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcTrgDownMap_;
	std::map<TYPE, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>>> funcTrgUpMap_;


	/// @brief 入力状況に応じたトリガーを登録
	/// @param _type 入力の種類
	/// @param _keys キーボード入力
	/// @param _padButtons パッドのボタン入力
	/// @param _padSticks パッドのスティック入力
	/// @param _mouse マウスの入力
	void RegisterTrigger(const TYPE _type, 
						 const std::vector<int> _keys,
						 const std::vector<Input::JOYPAD_BTN> _padButtons,
						 const std::vector<Input::JOYPAD_STICK> _padSticks,
						 const Input::MOUSE _mouse = Input::MOUSE::NONE);

	/// @brief 処理の登録
	/// @param _type 入力の種類
	/// @param newFuncs IsNew()系の関数ポインタ
	/// @param trgDownFuncs IsTrgDown()系の関数ポインタ
	/// @param trgUpFuncs IsTrgUp()系の関数ポインタ
	void RegisterTriggerFunction(const InputManager::TYPE _type, 
								 std::vector< std::function<bool(TYPE, Input::JOYPAD_NO)> > _newFuncs,
								 std::vector< std::function<bool(TYPE, Input::JOYPAD_NO)> > _trgDownFuncs,
								 std::vector< std::function<bool(TYPE, Input::JOYPAD_NO)> > _trgUpFuncs);

	// キー関係の入力判定
	bool IsNewKey(const TYPE type);
	bool IsTrgDownKey(const TYPE type);
	bool IsTrgUpKey(const TYPE type);

	// パッドボタンの入力判定
	bool IsNewPadButton(const TYPE _type, const Input::JOYPAD_NO _padNo);
	bool IsTrgDownPadButton(const TYPE _type, const Input::JOYPAD_NO _padNo);
	bool IsTrgUpPadButton(const TYPE _type, const Input::JOYPAD_NO _padNo);

	// パッドスティックの入力判定
	bool IsNewPadStick(const TYPE _type, const Input::JOYPAD_NO _padNo);
	bool IsTrgDownPadStick(const TYPE _type, const Input::JOYPAD_NO _padNo);
	bool IsTrgUpPadStick(const TYPE _type, const Input::JOYPAD_NO _padNo);

	// マウスの入力判定
	bool IsNewMouse(const TYPE _type);
	bool IsTrgDownMouse(const TYPE _type);
	bool IsTrgUpMouse(const TYPE _type);

	//void CursorProcess(void);


	/// @brief コンストラクタ
	InputManager(void);

	/// @brief デストラクタ
	~InputManager(void) = default;

	// コピーコンストラクタ対策
	InputManager(const InputManager&)			 = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&)			= delete;
	InputManager& operator=(InputManager&&) = delete;
};