#pragma once
#include <vector>
#include <unordered_map>
#include <DxLib.h>
#include <string>

#include "../../Common/Vector2.h"

/// @brief 入力管理クラス(シングルトン)
class KeyConfInputManager
{
public:

	/// @brief 入力デバイス名
	enum class INPUT_TYPE
	{
		KEY_BOARD,  // キーボード
		MOUSE,      // マウス
		JOYPAD,     // ジョイパット
	};

	/// @brief 実入力情報
	struct InputInfo
	{
		INPUT_TYPE type = INPUT_TYPE::KEY_BOARD; // 入力デバイスの種類
		unsigned int id = -1; // キーコードやボタンIDなどの識別子
	};

	/// @brief 保存データのヘッダ構造
	struct KeyConfigHeader
	{
		char     signature[4] = {""};  // ファイル識別ようのシグネチャ
		float    version = 1.0f;       // バージョン
		uint32_t dataNum = 0;          // 入力イベントの数
	};

	/// @brief スティックの入力値
	struct StickInfo
	{
		int lx = 0;     // 左スティック 左右
		int ly = 0;     // 左スティック 上下
		int rx = 0;     // 右スティック 左右
		int ry = 0;     // 右スティック 上下
	};

	/// @brief マウスの感度設定
	struct MouseSensitivity
	{
		float x = 1.0f;         // X軸感度
		float y = 1.0f;         // Y軸感度
		bool invertX = false;   // X軸反転
		bool invertY = false;   // Y軸反転
	};

	/// @brief 右スティックの感度設定
	struct RStickSensitivity
	{
		float x = 1.0f;          // X軸感度
		float y = 1.0f;          // Y軸感度
		bool invertX = false;    // X軸反転
		bool invertY = false;    // Y軸反転
		float deadZone = 0.35f; // デットゾーンのしきい値
	};

	/// @brief 感度設定ファイルのヘッダ構造
	struct SensitivityHeader
	{
		char  signature[4] = {""};   // ファイル識別用のシグネチャ
		float version = 1.0f;        // バージョン
	};

	// XInputのアナログ値の最大値
	static constexpr float XINPUT_VAL_MAX = 32767.0f;

	/// @brief 左スティックのデッドゾーンしきい値
	static constexpr float LEFT_STICK_DEAD_ZONE = 0.35f;

	/// @brief インスタンの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static KeyConfInputManager& GetInstance(void);
	
	/// @brief インスタンスの破棄
	void DestroyInstance(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 押し続けている間 true を返す
	/// @param _name イベント名
	/// @return 押されていれば true
	bool isPressed(const std::string& _name) const;


	/// @brief 離した瞬間のみ true を返す
	/// @param _name イベント名
	/// @return 離した瞬間のみ true
	bool isTrigerUp(const std::string& _name) const;

	/// @brief 押した瞬間のみ true を返す
	/// @param _name イベント名
	/// @return 押した瞬間のみ true
	bool isTrigerDown(const std::string& _name) const;


	/// @brief マウスカーソルの座標を取得 
	/// @return スクリーン座標のマウスカーソル位置
	Vector2 GetMousePosition(void) const;

	/// @brief マウスの移動量を取得し、カーソルを画面中央に固定・非表示にする
	/// @return 前フレームからの移動量
	Vector2F GetMouseVelocityAndFixCenter(void);

	/// @brief マウスの感度設定を変更する
	/// @param sensitivity 設定する感度データ
	void SetMouseSensitivity(const MouseSensitivity& _sensitivity);

	const MouseSensitivity& GetMouseSensitivity(void) const;

	/// @brief 左スティックの入力方向を正規化したXZベクトルで返す
	/// @return 正規化済みの方向ベクトル
	VECTOR GetLeftStickDirection(void) const;

	/// @brief 右スティックの入力量を返す
	/// @return 右スティックの入力量
	Vector2F GetRIghtStick(void) const;

	/// @brief 左スティックの生の入力値を返す
	/// @return 左スティックの生の入力値
	Vector2F GetLeftStickRaw(void) const;

	/// @brief 右スティックの生の入力値を返す
	/// @return 右スティックの生の入力値
	Vector2F GetRightStickRaw(void) const;

	/// @brief 右スティックの感度設定を変更する
    /// @param sensitivity 設定する感度データ
	void SetRStickSensitivity(const RStickSensitivity& _sensitivity);

	/// @brief 現在の右スティックの感度設定を取得する
	/// @return 現在の感度設定
	const RStickSensitivity& GetRStickSensitivity(void) const;

	/// @brief 現在の入力テーブルをファイルに保存する
	void SaveInputTable(void);

	/// @brief ファイルから入力テーブルを読み込む
	void LoadInputTable(void);

	/// @brief 感度設定をファイルに保存する
	void SaveSensitivitySettings(void) const;

	/// @brief 感度設定をファイルから読み込む
	void LoadSensitivitySettings(void);

	/// @brief 使用するパッドの番号を設定する
	/// @param padNo DxLib定数(DX_INPUT_PAD1 ～ DX_INPUT_PAD4)
	void SetUsePadNo(int _padNo);

private:
	// 入力テーブル関連
	using InputTable_t = std::unordered_map<std::string, std::vector<InputInfo>>;  
	InputTable_t inputTable_;                                    // イベント名と実入力のマッピングテーブル
	std::unordered_map<std::string, bool> currentInputState_;    // 現フレームの入力状態(イベント名 → 押されているか)
	std::unordered_map<std::string, bool> previousInputState_;   // 前フレームの入力状態(トリガー判定に使用)

	// スティック関連
	StickInfo stickInfo_;                      // 現フレームのスティック入力値
	RStickSensitivity rStickSensitivity_;  // 右スティックの感度設定

	// マウス関連
	Vector2 mousePosition_;               // マウスカーソルの座標
	MouseSensitivity mouseSensitivity_;   // マウス感度

	// パット番号
	int usePadNo_;

	// シングルトン用インスタンス
	static KeyConfInputManager* instance_;

	/// @brief コンストラクタ
	KeyConfInputManager(void);

	/// @brief デストラクタ 
	~KeyConfInputManager(void) = default;

	/// @brief 入力テーブルをデフォルト設定で初期化する
	void InitInputTable(void);

	/// @brief 右スティックの入力値にデッドゾーンと感度を適用し正規化する
	/// @param x スティックのX入力値
	/// @param y スティックのY入力値
	/// @param outX 処理後のX値
	/// @param outY 処理後のY値
	void ApplyRightStickSensitivity(int _x, int _y, float& _outX, float& _outY) const;

	// コピー禁止 
	KeyConfInputManager(const KeyConfInputManager&) = delete;
	KeyConfInputManager& operator=(const KeyConfInputManager&) = delete;

	// ムーブ禁止
	KeyConfInputManager(KeyConfInputManager&&) = delete;
	KeyConfInputManager& operator=(KeyConfInputManager&&) = delete;

};

