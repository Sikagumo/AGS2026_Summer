#pragma once

#include <DxLib.h>
#include <cstdint>

#include "../Common/Quaternion.h"

// ネットワーク定数関連
static constexpr int MAX_PLAYERS = 4;                    // 最大接続プレイヤー数
static constexpr int NUM_FRAME = 10;                     // アクション履歴を保持するフレーム数
static constexpr int MAX_SEND_BYTES = 4096;              // 最大送信バイト数
static constexpr int HOST_PORT = 65000;                  // ホストの受信ポート番号
static constexpr int CLIENT_PORT = 65001;                // クライアントポート番号(現状は自動割当のため未使用)
static constexpr IPDATA LOCALHOST_IP = { 127, 0, 0, 1 }; // ローカルホストIP

/// @brief ネットワーク接続の役割モード
enum class NET_MODE
{
	NONE,   // 未設定
	HOST,   // 部屋の作成者
	CLIENT  // 部屋への参加者
};

/// @brief 通信パケットのデータ種別識別子
enum class NET_DATA_TYPE
{
	NONE,              // 未設定
	USER,              // 単一ユーザー情報
	USERS,             // 全ユーザーのリスト情報
	ACTION_HIST_ALL,   // 全員の過去アクション履歴
	ACTION_HIST_RELAY, // ホストが他クライアントのアクションを中継する
	BOSS_ACTION,       // ボスの行動情報（タイポ修正: ACTOION -> ACTION）
	GO_GAME_SCENE,     // 全員が準備完了かどうか
	LEAVE_ROOM         // 退出通知
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
	float gameTime = 500.0f;                  // パケット送信時のゲーム内時刻

	// データ検証関連
	uint32_t crc = 0; // データの破損検知用CRC32チェックサム
};

/// @brief セッションに参加している接続ユーザーの情報構造体
struct NET_JOIN_USER
{
	// 接続情報関連
	NET_MODE mode = NET_MODE::NONE;    // ネットワークモード
	int key = -1;                      // ユーザー識別子
	IPDATA ipAddress = { 0, 0, 0, 0 }; // IPアドレス
	int port = -1;                     // ポート番号
	int roomWordId = -1;               // ルームの合言葉ID

	// プレイヤー状態関連
	PLAYER_TYPE playerType = PLAYER_TYPE::PLAYER_1; // プレイヤー種別
	GAME_STATE gameState = GAME_STATE::CONNECTING;  // ゲームの進行ステート

	// プレイヤー選択情報関連
	int selectedJobType = 0;  // 選択した武器
	int selectedSkinType = 0; // 選択したスキン
};

/// @brief 全ユーザーリスト
struct NET_JOIN_USERS
{
	// 全ユーザーの配列
	NET_JOIN_USER users[MAX_PLAYERS]; 
};

/// @brief プレイヤーの1フレーム分の同期アクションデータ構造体（3D対応）
struct NET_ACTION
{
	// 識別関連
	int key = -1;                 // ユーザー識別子
	unsigned int frameNumber = 0; // フレーム番号

	// トランスフォーム関連
	VECTOR position = { 0.0f, 0.0f, 0.0f }; // 座標
	Quaternion rotation;                    // 回転（クォータニオン）

	// 状態関連
	int animationId = 0;         // 再生中のアニメーションID
	int currentHp = 0;           // 現在のHP
	unsigned int actionBits = 0; // アクション状態のビットフラグ
	bool isAttack = false;       // 攻撃中かどうか
};

/// @brief ボスキャラクターの1フレーム分の同期アクションデータ構造体
struct NET_BOSS_ACTION
{
	// フレーム番号
	unsigned int frameNumber = 0; 

	// トランスフォーム関連
	VECTOR position = { 0.0f, 0.0f, 0.0f }; // 座標
	Quaternion rotation;                    // 回転（クォータニオン）

	// ターゲット・状態関連
	int targetPlayerId = -1;     // 狙っているプレイヤーのID
	int animationId = 0;         // 再生中のアニメーションID
	int bossHp = 2000;           // ボス本体のHP
	int missilePodTargetId = 0;  // ミサイルポッドのターゲットID
	int cannonTargetId = 0;      // キャノンのターゲットID
	int attackSelect = -1;       // ホストが選んだ攻撃タイプ

	// 各部位（ウェポン）のHP関連
	int weaponMachineGunLeftHp = 1250;  // 左マシンガンのHP
	int weaponMachineGunRightHp = 1250; // 右マシンガンのHP
	int weaponMissilePodLeftHp = 750;   // 左ミサイルポッドのHP
	int weaponMissilePodRightHp = 750;  // 右ミサイルポッドのHP
	int weaponRailGunHp = 2000;         // レールガンのHP
	int weaponCannonLeftHp = 1000;      // 左キャノンのHP
	int weaponCannonRightHp = 1000;     // 右キャノンのHP
};

/// @brief 複数フレーム分のプレイヤーアクション履歴構造体
struct NET_ACTION_HIS
{
	// ユーザー識別用キー
	int key = -1;

	// 過去数フレーム分のアクションデータ配列
	NET_ACTION actions[NUM_FRAME]; 
};

/// @brief 複数プレイヤーのアクション履歴をまとめた中継パケット
struct NET_ACTION_HIS_ALL
{
	// 履歴データ関連
	int count = 0;                         // 格納されている履歴の数
	NET_ACTION_HIS histories[MAX_PLAYERS]; // アクション履歴の配列
};