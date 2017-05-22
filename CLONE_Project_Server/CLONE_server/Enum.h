#pragma once

//player type
enum ePlayerType
{
	PLAYER_RED = 0,
	PLAYER_BLUE,
	PLAYER_SOLO,
	PLAYER_END
};

//player & bot state -> made by client
enum eObjState
{
	OBJ_STATE_MOVE = 0,
	OBJ_STATE_RUN,
	OBJ_STATE_IDEL,
	OBJ_STATE_ATT,
	OBJ_STATE_DIE,
	OBJ_STATE_JUMP,
	OBJ_STATE_SIT,
	OBJ_STATE_END
};

//client
//bead type
enum eBeadType
{
	BEAD_C = 0,
	BEAD_L,
	BEAD_O,
	BEAD_N,
	BEAD_E,
	BEAD_END
};
//client
//Trans Type
enum ANGLE
{
	ANGLE_X = 0,
	ANGLE_Y,
	ANGLE_Z,
	ANGLE_END
};

//stage type
enum eStageType
{
	STAGE_READY = 0,
	STAGE_PLAY,
	STAGE_RESULT,
	STAGE_END
};

//success send
enum eSendDataType
{
	COMPLETE_PLAYER = 0,
	COMPLETE_BOT,
	COMPLETE_BEAD,
	COMPLETE_END
};

//game type
enum eGameType
{
	GAME_ROOM = 0,
	GAME_LOADING,
	GAME_PLAY,
	GAME_END
};

//game mode type
enum eGameModeType
{
	MODE_SOLO = 0,
	MODE_TEAM,
	MODE_END
};

//timer type
enum eTimerType
{
	TIMER_TIME = 0,
	TIMER_SYNC,
	TIMER_END
};

//worker thread operation type
enum eWorkOperationType
{
	OP_SEND = 0,
	OP_RECV,
	OP_EVENT_COUNTDOWN,
	OP_EVENT_BOT_MOVE,
	OP_EVENT_MOVE_ROOM,
	OP_END
};

//event type
enum eEventType
{
	EVENT_COUNTDOWN = 0,
	EVENT_BOT_MOVE,
	EVENT_MOVE_ROOM,
	EVENT_END
};