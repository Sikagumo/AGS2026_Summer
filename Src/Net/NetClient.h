#pragma once
#include "NetBase.h"

/// @brief ネットワーククライアントの処理を管理するクラス
class NetClient : public NetBase
{
public:
    // ベースクラスのコンストラクタを使用
    using NetBase::NetBase;

    /// @brief 接続中の更新処理
    void UpdateConnecting(void) override;

    /// @brief ゲームシーン遷移時の更新処理
    void UpdateGotoGame(void) override;

    /// @brief ゲームプレイ中の更新処理
    void UpdateGamePlaying(void) override;

private:

    // ユーザー情報送信のしきい値時間
    static constexpr float SEND_INTERVAL_LIMIT = 0.1f; 
};