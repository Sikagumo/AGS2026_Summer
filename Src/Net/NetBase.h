#pragma once

#include <vector>

#include "NetStructures.h"

class NetManager;

/// @brief ネットワーク処理のベースクラス
class NetBase
{
public:
	/// @brief コンストラクタ
	/// @param _netManager ネットワークマネージャーの参照
	NetBase(NetManager& _netManager);

	/// @brief デストラクタ
	virtual ~NetBase(void) = default;

	/// @brief 接続中の更新処理
	virtual void UpdateConnecting(void) = 0;

	/// @brief ゲームシーン遷移時の更新処理
	virtual void UpdateGotoGame(void) = 0;

	/// @brief ゲームプレイ中の更新処理
	virtual void UpdateGamePlaying(void) = 0;

	/// @brief ユーザー受信時の処理
	virtual void OnReceiveUser(void) {};

	/// @brief 複数ユーザー受信時の処理
	/// @param _users 受信したユーザーのリスト
	virtual void OnReceiveUsers(const std::vector<NET_JOIN_USER>& _users) {};

	/// @brief アクション履歴受信時の処理
	/// @param _actionHistory 受信したアクション履歴
	virtual void OnReceiveAction(const NET_ACTION_HIS& _actionHistory) {};

	/// @brief ボスアクション受信時の処理
	/// @param _action 受信したボスアクション
	virtual void OnReceiveBossAction(const NET_BOSS_ACTION& _action) {};

protected:

	// 定数関連
	static constexpr float SEND_INTERVAL_USER = 2.0f;           // ユーザー送信間隔
	static constexpr float SEND_INTERVAL_ACTION = 1.0f / 60.0f; // アクション送信間隔

	// ネットワークマネージャーの参照
	NetManager& netManager_; 

	// タイマー関連
	float timerUser_;   // ユーザー送信用タイマー
	float timerAction_; // アクション送信用タイマー
};