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

class SceneLobby : public SceneBase
{
public:

	/// @brief コンストラクタ
	/// @param _isMulti マルチプレイか否か
	SceneLobby(bool _isMulti);

	/// @brief デストラクタ
    ~SceneLobby(void) = default;

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

    void UpdateGui(void)override {};

private:

    const bool IS_MULTI;

    bool isSelectMenu_;
    int selectedIdx_;
    int inputIntervalCounter_;

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
    std::array<int, static_cast<int>(UI_SINGLE::MAX)>
        uiHandles_;


    // マウスカーソル用の円コライダー
    std::unique_ptr<Collider2DCircle> cursorCollider_;

    std::array<std::unique_ptr<Collider2DBase>, 4>
        uiCollisions_;

    std::array<Collider2DBase::TAG_2D, 4>
        uiCollisionTags_;
    
    void InitUISingle(void);

    void UpdateSingle(void);
    void UpdateMulti(void);
};

