#pragma once

#include <DxLib.h>
#include <cstdint>

#include "../Common/Quaternion.h"

static constexpr int MAX_PLAYERS = 4;

static constexpr int NUM_FRAME = 10;

enum class NET_MODE
{
	NONE,
	HOST,
	CLIENT
};

enum class NET_DATA_TYPE
{
	NONE,
	USER,
	USERS,
	ACTION_HIST_ALL,
	BOSS_ACTOION
};

enum class GAME_STATE
{
	NONE,
	CONNECTING,
	GOTO_GAME,
	GAME_PLAYING,
};

struct NET_BASIC_DATA
{
	NET_DATA_TYPE type = NET_DATA_TYPE::NONE;

	int key = -1;

	float gameTime = 0.0f;

	uint32_t crc = 0;
};

struct NET_ACTION
{
	unsigned int  frameNo = 0;

	VECTOR pos;

	Quaternion quaRot;

	int animId = 0;

	unsigned char buttons = 0;

	int currentHp = 0;
};

struct NET_BOSS_ACTION
{
	unsigned int frameNo = 0;

	VECTOR pos;

	Quaternion quaRot;

	int  targetPlayerId = -1;

	int animId = 0;

	int bossHp = 0;
};

struct NET_ACTION_HIS
{
	int key = -1;

	NET_ACTION actions[NUM_FRAME];
};

struct NET_JOINT_USER
{
	NET_MODE mode = NET_MODE::NONE;

	int key = -1;

	IPDATA ip = { 0, 0, 0, 0 };

	int port = 0;

	GAME_STATE state = GAME_STATE::NONE;

	int playerType = 0;
};