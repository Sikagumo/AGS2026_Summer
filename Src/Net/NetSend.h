#pragma once
#include "NetStructures.h"

/// @brief ネットワークのデータ送信を担うクラス
class NetSend
{
public:

	/// @brief コンストラクタ
	/// @param _sendSocketId 送信用ソケットのID
	NetSend(int _sendSocketId);

	/// @brief デストラクタ
	~NetSend(void) = default;

	/// @brief データを送信する
	/// @param _type 送信するデータの種類
	void Send(NET_DATA_TYPE _type);

private:

	// 通信パラメータ関連
	int sendSocketId_; // 送信用ソケットID

	/// @brief 自分のユーザー情報を送信する
	void SendUser(void);

	/// @brief 全ユーザーのリストを送信する
	void SendUsers(void);

	/// @brief アクション履歴を送信する
	void SendActionHistoryAll(void);

	/// @brief ボスのアクションを送信する
	void SendBossAction(void);

	/// @brief アクションを送信する
	void SendAction(void);

	/// @brief ゲーム開始の合図を送信する
	void SendGoGameScene(void);

	/// @brief 全クライアントのアクション履歴を中継して送信する
	void SendAllClientActions(void);

	/// @brief ルーム退出の合図を送信する
	void SendLeaveRoom(void);

	/// @brief 通信用の基本ヘッダデータを作成する
	/// @param _type データの種類
	/// @param _crc CRCチェック用の値
	/// @return 作成された基本データ構造体
	NET_BASIC_DATA MakeBasicData(NET_DATA_TYPE _type, std::uint32_t _crc);

	/// @brief UDPでホストに向けてデータを送信する
	/// @param _bufferPointer 送信するデータのポインタ
	/// @param _dataSize 送信するデータのサイズ
	void SendUDP_Host(const void* _bufferPointer, int _dataSize);

	/// @brief UDPで全クライアントに向けてデータを送信する
	/// @param _bufferPointer 送信するデータのポインタ
	/// @param _dataSize 送信するデータのサイズ
	void SendUDP_Client(const void* _bufferPointer, int _dataSize);
};