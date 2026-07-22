#pragma once
#include "../SceneBase.h"
#include <DxLib.h>
#include <array>
#include <memory>
#include "../../Common/Vector2.h"
#include "../../Object/Actor/Chara/Player/PlayerBase.h"
#include "../../Object/Collider2D/Collider2DBase.h"
#include "../../Object/Collider2D/Collider2DCircle.h"
#include "../../Object/Collider2D/Collider2DBox.h"
#include "../../Net/NetStructures.h"

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
    void UpdateGui(void) override {};

private:

    // UI配置計算定数
   static constexpr float BUTTON_SCALE = 0.6f;
   static constexpr Vector2F BUTTON_SIZE = { 600.0f * BUTTON_SCALE, 250.0f * BUTTON_SCALE };

    // レイアウト座標設定
    const float LEFT_PANEL_X = 150.0f;
    const float RIGHT_PANEL_X = 1100.0f;
    const float PANEL_START_Y = 200.0f;
    const float PANEL_INTERVAL_Y = 120.0f;

    // UIコライダー用定数
    static constexpr size_t UI_SINGLE_MAX = 8;

    // モード設定関連
    const bool IS_MULTI;

    // UI選択状態関連
    bool isSelectMenu_;
    int mainSelectIndex_;
    int selectedJobIndex_;
    int selectedSkinIndex_;
    int inputIntervalCounter_;

    // シングルプレイUI画像関連
    enum class UI_SINGLE
    {
        NONE = -1,
        SELECT_BOMB,
        NOT_SELECT_BOMB,
        SELECT_BIG,
        NOT_SELECT_BIG,
        SELECT_RECOVERY,
        NOT_SELECT_RECOVERY,
        SELECT_RAPID_FIRE,
        NOT_SELECT_RAPID_FIRE,
        GAME_START,

        MAX
    };
    std::array<int, static_cast<int>(UI_SINGLE::MAX)> uiHandles_;
   
    // コライダー関連
    std::unique_ptr<Collider2DCircle> cursorCollider_;
    std::array<std::unique_ptr<Collider2DBase>, UI_SINGLE_MAX> uiCollisions_;
    std::array<Collider2DBase::TAG_2D, UI_SINGLE_MAX> uiCollisionTags_;

    int previewModelHandle_; // 表示用3Dモデルハンドル
    int currentModelIndex_;   // 現在読み込まれているモデルのインデックス

    // マルチプレイ用UIコライダー関連
    enum class MULTI_UI_TYPE
    {
        PASSCODE_BOX,
        HOST_BUTTON,
        CLIENT_BUTTON,

        MAX
    };
    std::array<std::unique_ptr<Collider2DBase>,
        static_cast<size_t>(MULTI_UI_TYPE::MAX)> multiUiCollisions_;

    // 初期化処理関連
    /// @brief シングルプレイ用UI初期化処理
    void InitUISingle(void);

    /// @brief マルチプレイ用UI初期化処理
    void InitUIMulti(void);

    // 更新処理関連
    /// @brief シングルプレイ用更新処理
    void UpdateSingle(void);

    /// @brief プレビュー用3Dモデルの更新・読み込み
    void UpdatePreviewModel(void);

    // 描画処理関連
    /// @brief 中央モデル・画像描画処理
    void DrawCenterModel(void);

    /// @brief 選択UI（左右パネル）描画処理
    void DrawSelectionPanels(void);

    // マルチプレイ状態関連
    enum class LOBBY_STATE
    {
        SELECT_EQUIPMENT, // 装備・スキン選択フェーズ
        SELECT_MODE,      // ホスト/クライアント選択・IP入力
        CONNECTING,       // 接続中
        IN_ROOM           // ルーム内
    };

    LOBBY_STATE multiState_;
    int buttonSelectIndex_;
    int passcode_[4];
    int selectOctet_;
    bool isEditing_;
    bool myReadyState_;
    int readyImageHandle_;   // READY画像用ハンドル
    int waitingImageHandle_; // WAITING画像用ハンドル

    // カラー定数関連
    const unsigned int COLOR_WHITE = GetColor(255, 255, 255);
    const unsigned int COLOR_YELLOW = GetColor(255, 255, 0);
    const unsigned int COLOR_GRAY = GetColor(100, 100, 100);
    const unsigned int COLOR_GREEN = GetColor(0, 255, 0);

    // マルチプレイ処理関数関連
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

    // ミニウィンドウの状態
    enum class SELECT_STATE
    {
        MAIN,           // 左右メインUI（武器 / 見た目）選択
        WEAPON_WINDOW,  // 中央：武器選択ミニウィンドウ表示中
        SKIN_WINDOW     // 中央：スキン選択ミニウィンドウ表示中
    };

    SELECT_STATE selectState_;

    // メインUI用
    std::array<std::unique_ptr<Collider2DBase>, 2> mainUiCollisions_;

    // ミニウィンドウ内のボタン用（武器4つ、スキン4つ）
    std::array<std::unique_ptr<Collider2DBase>, 
        static_cast<size_t>(PlayerBase::JOB_TYPE::MAX)> weaponUiCollisions_;
    std::array<std::unique_ptr<Collider2DBase>, 
        static_cast<size_t>(PlayerBase::SKIN_TYPE::MAX)> skinUiCollisions_;

    void DrawWeaponWindow(void);

    void DrawSkinWindow(void);
};