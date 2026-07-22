#pragma once

#include <DxLib.h>
#include <cstdint>

#include "../Common/Quaternion.h"

// 最大接続プレイヤー数
static constexpr int MAX_PLAYERS = 4;

// アクション履歴を保持するフレーム数
static constexpr int NUM_FRAME = 10;

// 最大送信バイト数
static constexpr int MAX_SEND_BYTES = 1024;

// ホストの受信ポート番号
static constexpr int HOST_PORT = 65000;

// クライアントポート番号(現状は自動割当のため未使用。将来固定化する場合に使用)
static constexpr int CLIENT_PORT = 65001;

// ローカルホストIP
static constexpr IPDATA LOCALHOST_IP = { 127, 0, 0, 1 };

/// @brief ネットワーク接続の役割モード
enum class NET_MODE
{
	NONE,   // 未設定
	HOST,   // 部屋の作成者（サーバー側）
	CLIENT  // 部屋への参加者（クライアント側）
};

/// @brief 通信パケットのデータ種別識別子
enum class NET_DATA_TYPE
{
	NONE,             // 未設定
	USER,             // 単一ユーザー情報
	USERS,            // 全ユーザーのリスト情報
	ACTION_HIST_ALL,  // 全員の過去アクション履歴
	BOSS_ACTOION,     // ボスの行動情報
	GO_GAME_SCENE     // 全員が準備完了かどうか
};

/// @brief 同期するゲームの進行ステート
enum class GAME_STATE
{
	NONE,          // 未設定
	CONNECTING,    // 接続中・同期中
	GOTO_GAME,     // ゲーム画面への遷移処理中
	GAME_PLAYING   // ゲームプレイ中
};

/// @brief プレイヤー種別
enum class PLAYER_TYPE
{
	PLAYER_1,      // プレイヤー1
	PLAYER_2,      // プレイヤー2
	PLAYER_3,      // プレイヤー3
	PLAYER_4,      // プレイヤー4
};

/// @brief 全通信パケットに共通して付与されるヘッダーデータ構造体
struct NET_BASIC_DATA
{
	// 識別・同期関連
	NET_DATA_TYPE type = NET_DATA_TYPE::NONE; // データの種別
	int key = -1;                             // ユーザー識別子
	float gameTime = 0.0f;                    // パケット送信時のゲーム内時刻

	// データの破損検知用CRC32チェックサム
	uint32_t crc = 0;
};

/// @brief セッションに参加している接続ユーザーの情報構造体
struct NET_JOIN_USER
{
	NET_MODE mode = NET_MODE::NONE;
	int key = -1;
	IPDATA ip = { 0, 0, 0, 0 };
	int port = -1;
	int roomWordId = -1;
	PLAYER_TYPE playerType = PLAYER_TYPE::PLAYER_1;
	GAME_STATE gameState = GAME_STATE::CONNECTING;
};

/// @brief 全ユーザーリスト
struct NET_JOIN_USERS
{
	NET_JOIN_USER users[MAX_PLAYERS];
};

/// @brief プレイヤーの1フレーム分の同期アクションデータ構造体（3D対応）
struct NET_ACTION
{
	int key = -1;
	unsigned int frameNo = 0;
	VECTOR pos = { 0.0f, 0.0f, 0.0f };
	Quaternion quaRot;
	int animId = 0;
	int currentHp = 0;
	unsigned int actBits = 0;
	bool isAttack = false;
};

/// @brief ボスキャラクターの1フレーム分の同期アクションデータ構造体
struct NET_BOSS_ACTION
{
	// フレーム番号
	unsigned int frameNo = 0;

	// トランスフォーム関連
	VECTOR pos = { 0.0f, 0.0f, 0.0f };   // 位置座標
	Quaternion quaRot;                   // 回転（クォータニオン）

	// ターゲット・状態関連
	int targetPlayerId = -1;  // 狙っているプレイヤーのID
	int animId = 0;           // 再生中のアニメーションID
	int bossHp = 2000;           // ボスの現在HP

	// 各ウェポンHP関連
	int weaponMglHp = 1250;
	int weaponMgrHp = 1250;
	int weaponMpLHp = 750;
	int weaponMpRHp = 750;
	int weaponRgHp = 2000;
	int weaponCannonLHp = 1000;
	int weaponCannonRHp = 1000;
};

/// @brief 複数フレーム分のプレイヤーアクション履歴構造体（パケットロス対策用）
struct NET_ACTION_HIS
{
	// ユーザー識別用キー
	int key = -1;

	// 過去数フレーム分のデータ配列
	NET_ACTION actions[NUM_FRAME];
};
