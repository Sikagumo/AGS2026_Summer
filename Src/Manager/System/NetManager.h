#pragma once

#include <mutex>
#include <map>
#include <vector>
#include "../../Net/NetStructures.h"

class NetBase;
class NetSend;

/// @brief ネットワーク管理クラス
class NetManager
{
public:
	/// @brief インスタンスを生成する
	static void CreateInstance(void);

	/// @brief インスタンスを取得する
	/// @return NetManagerの参照
	static NetManager& GetInstance(void);

	/// @brief インスタンスを破棄する
	static void DestroyInstance(void);

	/// @brief 実行を開始する
	/// @param _mode ネットワークのモード（HOST/CLIENT）
	void Run(NET_MODE _mode);

	/// @brief 実行を停止する
	void Stop(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief データを送信する
	/// @param _type 送信するデータの種類
	void Send(NET_DATA_TYPE _type);

	/// @brief 現在のモードを取得する
	/// @return ネットワークモード
	NET_MODE GetMode(void) const { return mode_; }

	/// @brief ホストかどうかを取得する
	/// @return ホストならtrue
	bool IsHost(void) const { return mode_ == NET_MODE::HOST; }

	/// @brief 自分のキーを取得する
	/// @return 自分のキー
	int GetMyKey(void) const { return netPool_.selfUser.key; }

	/// @brief ホストのIPアドレスを設定する
	/// @param _ip 設定するIPアドレス
	void SetHostIp(IPDATA _ip);

	/// @brief ホストのIPアドレスを取得する
	/// @return ホストのIPアドレス
	IPDATA GetHostIp(void) const { return hostIp_; }

	/// @brief 自分の情報を取得する
	/// @return 自分のネットワーク情報
	NET_JOIN_USER GetSelfUser(void) const;

	/// @brief 他のユーザー情報を取得する
	/// @return キーとネットワーク情報のマップ
	std::map<int, NET_JOIN_USER> GetNetUsers(void) const;

	/// @brief 自分の情報を設定する
	/// @param _info 設定するネットワーク情報
	void SetSelfInfo(const NET_JOIN_USER& _info);

	/// @brief ルームワードのIDを設定する
	/// @param _id ルームワードのID
	void SetRoomWordId(int _id);

	/// @brief 自分の最新アクションを履歴に追加する
	/// @param _action 追加するアクション
	void AddSelfAction(const NET_ACTION& _action);

	/// @brief 自分の過去数フレーム分のアクション履歴を取得する
	/// @return 自分のアクション履歴
	NET_ACTION_HIS GetSelfActionHistory(void) const;

	/// @brief 他人のアクション履歴を取得する
	/// @return キーとアクション履歴のマップ
	std::map<int, NET_ACTION_HIS> GetRemoteActionHistory(void) const;

	/// @brief ゲーム開始の合図を受け取ったか取得する
	/// @return 受け取っていればtrue
	bool GetHasReceivedGoGame(void) const { return hasReceivedGoGame_; }

	/// @brief ゲーム開始の合図をリセットする
	void ResetGoGame(void);

	/// @brief ボスのアクションを設定する
	/// @param _action 設定するボスのアクション
	void SetBossAction(const NET_BOSS_ACTION& _action);

	/// @brief ボスのアクションを取得する
	/// @return ボスのアクション
	const NET_BOSS_ACTION GetBossAction(void) const { return netPool_.bossAction; }

	/// @brief ゲーム時間を設定する
	/// @param _time 設定するゲーム時間
	void SetGameTime(float _time);

	/// @brief ゲーム時間を取得する
	/// @return ゲーム時間
	float GetGameTime(void) const { return gameTime_; }

	/// @brief 通信相手とのタイムアウトを検知する
	/// @return タイムアウトしていればtrue
	bool GetIsConnectionLost(void) const;

	/// @brief 接続タイムアウト時間を設定する
	/// @param _timeout タイムアウト時間（秒）
	void SetConnectionTimeout(float _timeout);

private:

	// 定数関連
	static constexpr float CONNECTION_TIMEOUT_DEFAULT = 5.0f; // デフォルトの接続ロスト判定秒数
	static constexpr float DEFAULT_GAME_TIME = 500.0f;        // デフォルトのゲーム時間

	// シングルトンインスタンス
	static NetManager* instance_;

	// クラスポインタ関連
	NetBase* netBase_; // ネットワークベースクラス
	NetSend* netSend_; // ネットワーク送信クラス

	// ステート関連
	bool isRunning_;         // 実行中フラグ
	NET_MODE mode_;          // ネットワークモード
	bool hasReceivedGoGame_; // ゲーム開始合図受信フラグ

	// アクション履歴関連
	NET_ACTION_HIS selfActionHistory_;                  // 自分のアクション履歴
	std::map<int, NET_ACTION_HIS> remoteActionHistory_; // 他人のアクション履歴

	// 通信パラメータ関連
	int receiveSocketId_; // 受信ソケットID
	int sendSocketId_;    // 送信ソケットID
	int roomWordId_;      // ルームの合言葉ID
	float gameTime_;      // ゲームの経過時間
	IPDATA hostIp_;       // ホストのIPアドレス

	// タイムアウトタイマー関連
	float hostTimeoutTimer_;                   // ホストへのタイムアウト計測タイマー
	float connectionTimeout_;                  // 接続タイムアウト判定時間
	std::map<int, float> clientTimeoutTimers_; // クライアントごとのタイムアウト計測タイマー

	// ネットワークプール関連
	struct NET_POOL
	{
		NET_JOIN_USER selfUser;                   // 自分のユーザー情報
		NET_BOSS_ACTION bossAction;               // ボスのアクション情報
		std::map<int, NET_JOIN_USER> remoteUsers; // 他のユーザー情報
	};

	NET_POOL netPool_;             // ネットワークプールのインスタンス
	mutable std::mutex poolMutex_; // プールアクセスを保護するミューテックス

	/// @brief コンストラクタ
	NetManager(void);

	/// @brief デストラクタ
	~NetManager(void);

	/// @brief 受信したUDPパケットの解析とデータ種別ごとの振り分け処理
	void UdpReceiveData(void);
};