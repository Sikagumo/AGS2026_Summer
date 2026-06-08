#pragma once
#include <string>

class FpsController;

class Application
{

public:

	// スクリーンサイズ
	static constexpr int SCREEN_ASPECT = 100;/*120*/
	static constexpr int SCREEN_SIZE_X = (16 * SCREEN_ASPECT);
	static constexpr int SCREEN_SIZE_Y = (9 * SCREEN_ASPECT);

	// スクリーンサイズの半分
	static constexpr int SCREEN_HALF_X = SCREEN_SIZE_X / 2;
	static constexpr int SCREEN_HALF_Y = SCREEN_SIZE_Y / 2;

	static constexpr int FRAME_RATE = 60;

	// データパス関連
	//-------------------------------------------
	static const std::string PATH_DATA;
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_ANIM;
	static const std::string PATH_EFFECT;
	static const std::string PATH_SOUND;
	static const std::string PATH_CSV;

	static const std::string PATH_KEY_CONFIG;
	static const std::string PATH_KEY_CONFIG_GAMEPAD;
	static const std::string PATH_KEY_CONFIG_KEYBOARD;
	//-------------------------------------------

	// 重力
	static constexpr float GRAVITY = 9.81f;
	static constexpr float GRAVITY_SCALE = 0.7f;

	// インスタンスを明示的に生成
	static void CreateInstance(void);

	// インスタンスの取得
	static Application& GetInstance(void);

	// 初期化
	void Init(void);

	// ゲームループの開始
	void Run(void);

	// リソースの破棄
	void DestroyInstance(void);

	// 初期化成功／失敗の判定
	bool IsInitFail(void) const;

	// 解放成功／失敗の判定
	bool IsReleaseFail(void) const;

	// 重力の取得
	float GetGravityPow(void) const { return GRAVITY * GRAVITY_SCALE; }

	/// @brief ゲーム終了 
	void GameEnd(void);

	/// @brief ゲーム終了 
	/// @return ゲームの終了フラグ
	bool GetGameEnd(void);

private:

	// 静的インスタンス
	static Application* instance_;

	// FPSコントローラー
	FpsController* fpsController_;

	// 初期化失敗
	bool isInitFail_;

	// 解放失敗
	bool isReleaseFail_;

	// ゲーム終了フラグ
	bool isGameEnd_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);

	// デストラクタも同様
	~Application(void) = default;

	// コピーコンストラクタ対策
	Application(const Application&)			   = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&)			  = delete;
	Application& operator=(Application&&) = delete;

	// エフェクシアの初期化
	void InitEffekseer(void);

};