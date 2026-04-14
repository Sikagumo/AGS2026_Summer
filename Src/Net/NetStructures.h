#pragma once
#pragma once
#include <DxLib.h>

static constexpr int MAX_PLAYERS = 4;
static constexpr int MAX_SEND_BYTES = 1024;
static constexpr int NUM_FRAME = 10;

// 通信モード
enum class NET_MODE
{
	NONE,
	HOST,
	CLIENT,
};

// ゲーム進行状態
enum class GAME_STATE
{
	NONE,
	CONNECTING,
	GOTO_GAME,
	GAME_PLAYING,
};

// ゲームプレイヤー種別
enum class PLAYER_TYPE
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4,
};

// 通信種別
enum class NET_DATA_TYPE
{
	NONE,
	USER,
	USERS,
	ACTION_HIS_ALL,
};

// 基本通信データ
struct NET_BASIC_DATA
{
	// 通信種別
	NET_DATA_TYPE type = NET_DATA_TYPE::NONE;
	// ユーザの任意キー
	int key = -1;
	// ゲームの進行時間
	float gameTime = 0.0f;
	uint32_t crc;
};

// ユーザデータ
struct NET_JOIN_USER
{
	// 通信モード(ホスト or クライアント)
	NET_MODE mode = NET_MODE::NONE;
	// ユーザの任意キー
	int key = -1;
	// ユーザのIPアドレス
	IPDATA ip = { 0, 0, 0, 0 };
	// ユーザのポート
	int port = -1;
	// ユーザ1、2、3、4...
	PLAYER_TYPE playerType = PLAYER_TYPE::PLAYER_1;
	// 全体のゲーム進行状態
	GAME_STATE gameState = GAME_STATE::CONNECTING;
};

struct NET_JOIN_USERS
{
	NET_JOIN_USER users[MAX_PLAYERS];
};

enum class PLAYER_ACTION
{
	NONE = 0,
	MOVE_LEFT = 1,
	MOVE_RIGHT = 2,
	JUMP = 4,
	JUMP_TRG = 8,
	JUMP_CANCEL = 16,
	SHOT = 32,
	WEAPON_CHANGE = 64
};

struct NET_ACTION
{
	// ユーザの任意キー
	int key = -1;
	// ゲーム進行フレーム
	unsigned int frameNo = 0;
	// アクションキー
	unsigned int actBits;
};

struct NET_ACTION_HIS
{
	// ユーザの任意キー
	int key = -1;
	// 過去フレーム分のアクション情報
	NET_ACTION actions[NUM_FRAME];
};

struct NET_USERS_ACTION_HIS
{
	// 全員分の過去フレーム分のアクション情報
	NET_ACTION_HIS actionsHistories[MAX_PLAYERS];
};
