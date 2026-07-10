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


private:

    bool isMulti_;
    bool isSelectMenu_;
    int selectedIdx_;

    enum class UI_SINGLE
    {
        NONE = -1,
        BOMB,
        BIG,
        RECOVERY,
        RAPID_FIRE,
        MAX
    };
    std::array<std::unique_ptr<Collider2DBase>, static_cast<int>(UI_SINGLE::MAX)>
        uiCollisions_;

    std::array<Collider2DBase::TAG_2D, static_cast<int>(UI_SINGLE::MAX)>
        uiCollisionTags_;
    
    void InitUISingle(void);

    void UpdateSingle(void);
    void UpdateMulti(void);
};

