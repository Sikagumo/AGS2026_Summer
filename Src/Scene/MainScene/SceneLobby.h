#pragma once

#include <array>
#include <memory>
#include <map>
#include <DxLib.h>

#include "../SceneBase.h"
#include "../../Common/Vector2.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Collider2D/Collider2DBase.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"
#include "../../Object/Common/AnimationController.h"
#include "../../Net/NetStructures.h"
#include "../../Application.h"

/// @brief ロビー画面シーンクラス
class SceneLobby : public SceneBase
{
public:

	/// @brief コンストラクタ
	/// @param _isMulti マルチプレイか否か
	SceneLobby(bool _isMulti);

	/// @brief デストラクタ
	~SceneLobby(void) override;

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

protected:

	/// @brief GUIの更新処理
	void UpdateGui(void) override
	{
	}

private:

	/// @brief シングルプレイUI画像種別
	enum class UI_SINGLE
	{
		NONE = -1,                 /// 未選択
		SELECT_BOMB,               /// 選択中：爆弾
		NOT_SELECT_BOMB,           /// 非選択：爆弾
		SELECT_BIG,                /// 選択中：巨大化
		NOT_SELECT_BIG,            /// 非選択：巨大化
		SELECT_RECOVERY,           /// 選択中：回復
		NOT_SELECT_RECOVERY,       /// 非選択：回復
		SELECT_RAPID_FIRE,         /// 選択中：連射
		NOT_SELECT_RAPID_FIRE,     /// 非選択：連射
		SELECT_SKIN_MOMO,          /// 選択中：スキン桃
		NOT_SELECT_SKIN_MOMO,      /// 非選択：スキン桃
		SELECT_SKIN_SARU,          /// 選択中：スキン猿
		NOT_SELECT_SKIN_SARU,      /// 非選択：スキン猿
		SELECT_SKIN_KIGI,          /// 選択中：スキンキジ
		NOT_SELECT_SKIN_KIGI,      /// 非選択：スキンキジ
		SELECT_SKIN_INU,           /// 選択中：スキン犬
		NOT_SELECT_SKIN_INU,       /// 非選択：スキン犬
		GAME_START,                /// ゲーム開始ボタン
		FORMATION,                 /// 編成ボタン
		MAX                        /// 要素数
	};

	/// @brief マルチプレイ用UIタイプ
	enum class MULTI_UI_TYPE
	{
		PASSCODE_BOX,              /// パスコード入力ボックス
		HOST_BUTTON,               /// ホスト作成ボタン
		CLIENT_BUTTON,             /// クライアント参加ボタン
		MAX                        /// 要素数
	};

	/// @brief マルチプレイ進行状態
	enum class LOBBY_STATE
	{
		SELECT_EQUIPMENT,          /// 装備・スキン選択フェーズ
		SELECT_MODE,               /// ホスト/クライアント選択・IP入力フェーズ
		CONNECTING,                /// サーバー/ホストへの接続中
		IN_ROOM                    /// ルーム待機中
	};

	/// @brief ロビーのUIメインテキスト種別
	enum class UI_MAIN_TEXT
	{
		WEAPON,                    /// 武器選択テキスト
		SKIN,                      /// スキン選択テキスト
		ROOM,                      /// ルーム作成テキスト
		JOIN,                      /// ルーム参加テキスト
		MAX                        /// 要素数
	};

	/// @brief ロビーの準備状態テキスト種別
	enum class UI_READY_TEXT
	{
		WAITING,                   /// 待機中テキスト
		READY,                     /// 準備完了テキスト
		PASSWORD,                  /// パスワードテキスト
		GO,                        /// 出撃可能テキスト
		MAX                        /// 要素数
	};

	/// @brief 確認ウィンドウ用テキスト種別
	enum class CONFIRM_TEXT
	{
		YES,                       /// はい
		NO,                        /// いいえ
		MAX                        /// 要素数
	};

	/// @brief ミニウィンドウ選択状態
	enum class SELECT_STATE
	{
		MAIN,                      /// 左右メインUI選択状態
		WEAPON_WINDOW,             /// 中央：武器選択ミニウィンドウ表示状態
		SKIN_WINDOW                /// 中央：スキン選択ミニウィンドウ表示状態
	};

	// UI配置計算関連の定数
	static constexpr float BUTTON_SCALE = 0.3f;                                               // ボタンの拡大率
	static constexpr Vector2F BUTTON_SIZE = { 600.0f * BUTTON_SCALE, 250.0f * BUTTON_SCALE }; // ボタンのサイズ

	// レイアウト座標関連の定数
	static constexpr int LEFT_PANEL_X = Application::SCREEN_HALF_X - 500;   // 左パネルのX座標
	static constexpr int RIGHT_PANEL_X = Application::SCREEN_HALF_X + 500;  // 右パネルのX座標
	static constexpr int PANEL_START_Y = Application::SCREEN_HALF_Y - 50;   // パネルの開始Y座標
	static constexpr int PANEL_INTERVAL_Y = 120;                            // パネルの間隔Y座標

	// シングルプレイUIの最大数
	static constexpr size_t UI_SINGLE_MAX = 8; 

	// マルチプレイフラグ
	const bool IS_MULTI; 

	// UI選択状態関連
	bool isSelectMenu_;          // メニュー選択中フラグ
	int mainSelectIndex_;        // メイン選択インデックス
	int selectedJobIndex_;       // 選択中のジョブインデックス
	int selectedSkinIndex_;      // 選択中のスキンインデックス
	int inputIntervalCounter_;   // 入力インターバルカウンター

	// シングルUIのハンドル配列
	std::array<int, static_cast<int>(UI_SINGLE::MAX)> uiHandles_; 

	// コライダー関連
	std::unique_ptr<Collider2DCircle> cursorCollider_;                         // カーソルのコライダー
	std::array<std::unique_ptr<Collider2DBase>, UI_SINGLE_MAX> uiCollisions_;  // UIコライダー配列
	std::array<Collider2DBase::TAG_2D, UI_SINGLE_MAX> uiCollisionTags_;        // UIコライダータグ配列
	std::unique_ptr<Collider2DBase> readyButtonCollision_;                     // 準備ボタンのコライダー
	std::unique_ptr<Collider2DBase> leaveYesCollider_;                         // 退出確認「はい」コライダー
	std::unique_ptr<Collider2DBase> leaveNoCollider_;                          // 退出確認「いいえ」コライダー

	// モデル表示関連
	int previewModelHandle_; // 表示用3Dモデルハンドル
	int currentModelIndex_;  // 現在読み込まれているモデルのインデックス

	// マルチUIコライダー配列
	std::array<std::unique_ptr<Collider2DBase>, static_cast<size_t>(MULTI_UI_TYPE::MAX)> multiUiCollisions_;

	// マルチプレイ配置関連の定数
	static constexpr int HOST_BUTTON_Y = Application::SCREEN_HALF_Y + 70;    // ホストボタンのY座標
	static constexpr int CLIENT_BUTTON_Y = Application::SCREEN_SIZE_Y - 100; // クライアントボタンのY座標
	static constexpr int PASSCODE_NUM_MAX = 10;                              // パスコード最大文字数

	// マルチプレイ用画像・テキストハンドル関連
	std::array<int, static_cast<int>(UI_MAIN_TEXT::MAX)> uiTexHandles_;                  // メインUIテキストハンドル
	std::array<int, static_cast<int>(UI_READY_TEXT::MAX)> uiReadyHandles_;               // 準備テキストハンドル
	std::array<int, static_cast<size_t>(CONFIRM_TEXT::MAX)> selectConfTextHandles_;      // 選択済み確認テキストハンドル
	std::array<int, static_cast<size_t>(CONFIRM_TEXT::MAX)> noSelectConfTextHandles_;    // 未選択確認テキストハンドル
	std::array<int, PASSCODE_NUM_MAX> passcodeTextHandles_;                              // パスコード用テキストハンドル
	int roomBackHandle_;                                                                 // ルーム背景ハンドル
	int selectedMultiHandle_;                                                            // 選択済みマルチUIハンドル
	int selectMultiHandle_;                                                              // 選択中マルチUIハンドル
	int multiTitleHandle_;                                                               // マルチタイトル画像ハンドル
	int connectTexHandle_;                                                               // 接続中テキストハンドル
	int allReadyImageHandle_;                                                            // 全員準備完了画像ハンドル
	int leaveRoomTextHandle_;                                                            // ルーム退出テキストハンドル

	// アニメーションコントローラー
	std::unique_ptr<AnimationController> animController_; 

	// マルチプレイ状態管理関連
	LOBBY_STATE multiState_; // マルチプレイの状態
	int buttonSelectIndex_;  // ボタン選択インデックス
	int passcode_[4];        // パスコード配列
	int selectOctet_;        // 選択中のオクテット
	bool isEditing_;         // 編集状態フラグ
	bool myReadyState_;      // 自身の準備状態

	// カラー関連の定数
	const unsigned int COLOR_WHITE = GetColor(255, 255, 255);   // 白色
	const unsigned int COLOR_YELLOW = GetColor(255, 255, 0);    // 黄色
	const unsigned int COLOR_GRAY = GetColor(100, 100, 100);    // 灰色
	const unsigned int COLOR_GREEN = GetColor(0, 255, 0);       // 緑色

	// ウィンドウ選択状態関連
	SELECT_STATE selectState_; // ミニウィンドウ選択状態

	// メインUI用コライダー配列
	std::array<std::unique_ptr<Collider2DBase>, 2> mainUiCollisions_; 

	// ミニウィンドウUIコライダー関連
	std::array<std::unique_ptr<Collider2DBase>, static_cast<size_t>(PlayerBase::JOB_TYPE::MAX)> weaponUiCollisions_;  // 武器用コライダー配列
	std::array<std::unique_ptr<Collider2DBase>, static_cast<size_t>(PlayerBase::SKIN_TYPE::MAX)> skinUiCollisions_;   // スキン用コライダー配列

	// 武器説明画像
	std::array<int, static_cast<size_t>(PlayerBase::JOB_TYPE::MAX)> weaponInfoHandle_;
	
	// 背景画像関連
	int backgroundHandle_;      // 背景画像ハンドル
	int selectUIBackHandle_;    // 選択UI背景ハンドル
	int selectedUIBackHandle_;  // 選択済みUI背景ハンドル
	int uiBackWidth_;           // UI背景の幅
	int uiBackHeight_;          // UI背景の高さ

	// 退出確認ウィンドウ関連
	bool isLeaveWindow_;     // 退出確認ウィンドウ表示フラグ
	int leaveSelectIndex_;   // 退出確認選択インデックス

	// ロビー表示用モデル関連
	std::array<int, static_cast<size_t>(PlayerBase::SKIN_TYPE::MAX)> lobbySkinHandles_;                                      // ロビー用スキンモデルハンドル配列
	std::array<std::unique_ptr<AnimationController>, static_cast<size_t>(PlayerBase::SKIN_TYPE::MAX)> lobbyAnimControllers_; // ロビー用アニメーションコントローラー配列

	/// @brief シングルプレイ用UI初期化処理
	void InitUISingle(void);

	/// @brief マルチプレイ用UI初期化処理
	void InitUIMulti(void);

	/// @brief シングルプレイ用更新処理
	void UpdateSingle(void);

	/// @brief プレビュー用3Dモデルの更新・読み込み
	void UpdatePreviewModel(void);

	/// @brief 中央モデル・画像描画処理
	void DrawCenterModel(void);

	/// @brief 選択UI描画処理
	void DrawSelectionPanels(void);

	/// @brief マルチプレイ用更新処理
	void UpdateMulti(void);

	/// @brief モード選択時更新処理
	void UpdateSelectMode(void);

	/// @brief 接続中更新処理
	void UpdateConnecting(void);

	/// @brief ルーム内更新処理
	void UpdateInRoom(void);

	/// @brief マルチプレイ用描画処理
	void DrawMulti(void);

	/// @brief モード選択時描画処理
	void DrawSelectMode(void);

	/// @brief 接続中描画処理
	void DrawConnecting(void);

	/// @brief ルーム内描画処理
	void DrawInRoom(void);

	/// @brief ゲームシーンへの遷移処理
	/// @param _users ネットワーク参加ユーザーマップ
	void MoveToGameScene(std::map<int, NET_JOIN_USER>& _users);

	/// @brief 武器とスキンをネットに登録する
	void SetJobToSKin(void);

	/// @brief 退出確認ウィンドウの描画処理
	void DrawLeaveConfirmWindow(void);

	/// @brief 武器選択ウィンドウの描画処理
	void DrawWeaponWindow(void);

	/// @brief スキン選択ウィンドウの描画処理
	void DrawSkinWindow(void);

	/// @brief スキンインデックスに応じたロビー用モデルハンドルを取得
	/// @param _skinIndex スキンのインデックス
	/// @return モデルハンドル
	int GetLobbySkinModelHandle(int _skinIndex) const;

	/// @brief 武器の描画用ハンドル取得
	/// @param _jobType ジョブタイプ
	/// @param _isSelected 選択状態か否か
	/// @return 武器UIのハンドル
	int GetWeaponUIHandle(PlayerBase::JOB_TYPE _jobType, bool _isSelected) const;

	/// @brief スキンの描画用ハンドル取得
	/// @param _skinType スキンタイプ
	/// @param _isSelected 選択状態か否か
	/// @return スキンUIのハンドル
	int GetSkinUIHandle(PlayerBase::SKIN_TYPE _skinType, bool _isSelected) const;
};