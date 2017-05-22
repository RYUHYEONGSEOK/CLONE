#pragma once

//clinet -> server
typedef enum eCS_Packet_Type
{
	//room
	CS_INIT_ROOM = 1,
	CS_CLIENT_READY,
	CS_CLIENT_CHANGE,
	CS_OPTION_CHANGE,
	CS_CHAT_ROOM,				//수정중

	//game	
	CS_INPUT_KEY,
	CS_ANIMATION_END,
	CS_CHAT_GAME,				//수정중
	CS_COLLISION_PLAYER_PLAYER,	//수정중
	CS_COLLISION_PLAYER_BOT,	//수정중
	CS_COLLISION_PLAYER_BEAD,	//수정중

	CS_END
}CS_PACKET_TYPE;

//server -> client
typedef enum eSC_Packet_Type
{
	//room
	SC_SYNC_ROOM = 1,
	SC_CONNECT_ROOM,
	SC_CLIENT_READY,
	SC_CLIENT_CHANGE,
	SC_OPTION_CHANGE,
	SC_CHAT_ROOM,				//수정중
	SC_ALL_UNREADY,

	//room & game
	SC_REMOVE_CLIENT,

	//game
	SC_SERVER_GAME_TIME,
	SC_CHAT_GAME,				//수정중
	SC_INIT_PLAYER,
	SC_INIT_BOT,
	SC_INIT_BEAD,
	SC_CHANGE_STATE_PLAYER,
	SC_CHANGE_STATE_BOT,
	SC_CHANGE_STATE_BEAD,		//수정중
	SC_SYNC_PLAYER,
	SC_SYNC_BOT,
	SC_CHECK_COLLISION,			//수정중
	SC_COUNT_DOWN,
	SC_GAME_STATE,				//수정중

	SC_END
}SC_PACKET_TYPE;


#pragma pack (push, 1)
//header
typedef struct tPacketHeader
{
	UINT size;
	BYTE type;
}HEADER;


//client -> server
//room
struct CS_InitRoom
	: public tPacketHeader
{
	char m_szName[MAX_NAME];

	CS_InitRoom()
	{
		ZeroMemory(m_szName, sizeof(char) * MAX_NAME);

		size = sizeof(*this);
		type = CS_INIT_ROOM;
	}
};
struct CS_Ready
	: public tPacketHeader
{
	bool m_bIsHost;

	CS_Ready()
	{
		size = sizeof(*this);
		type = CS_CLIENT_READY;
	}
};
struct CS_Change
	: public tPacketHeader
{
	CS_Change()
	{
		size = sizeof(*this);
		type = CS_CLIENT_CHANGE;
	}
};
struct CS_OptionChagne
	: public tPacketHeader
{
	bool m_bIsHost;

	bool m_bDownMode;
	bool m_bUpMode;
	bool m_bDownRound;
	bool m_bUpRound;
	bool m_bDownBot;
	bool m_bUpBot;

	CS_OptionChagne()
	{
		size = sizeof(*this);
		type = CS_OPTION_CHANGE;
	}
};
struct CS_ChatRoom
	: public tPacketHeader
{
	char m_szChat[MAX_CHAT];

	CS_ChatRoom()
	{
		ZeroMemory(m_szChat, sizeof(char) * MAX_CHAT);

		size = sizeof(*this);
		type = CS_CHAT_ROOM;
	}
};
//game
struct CS_InputKey
	: public tPacketHeader
{
	DWORD	m_dwKey;

	CS_InputKey()
	{
		size = sizeof(*this);
		type = CS_INPUT_KEY;
	}
};
struct CS_AnimationEnd
	: public tPacketHeader
{
	CS_AnimationEnd()
	{
		size = sizeof(*this);
		type = CS_ANIMATION_END;
	}
};
struct CS_ChatGame
	: public tPacketHeader
{
	char m_szChat[MAX_CHAT];

	CS_ChatGame()
	{
		ZeroMemory(m_szChat, sizeof(char) * MAX_CHAT);

		size = sizeof(*this);
		type = CS_CHAT_GAME;
	}
};
struct CS_CollisionPlayerPlayer
	: public tPacketHeader
{
	int m_iMyID;
	int m_iTargetPlayerID;

	CS_CollisionPlayerPlayer()
	{
		size = sizeof(*this);
		type = CS_COLLISION_PLAYER_PLAYER;
	}
};
struct CS_CollisionPlayerBot
	: public tPacketHeader
{
	int m_iMyID;
	int m_iBotID;

	CS_CollisionPlayerBot()
	{
		size = sizeof(*this);
		type = CS_COLLISION_PLAYER_BOT;
	}
};
struct CS_CollisionPlayerBead
	: public tPacketHeader
{
	int m_iMyID;
	int m_iBeadID;

	CS_CollisionPlayerBead()
	{
		size = sizeof(*this);
		type = CS_COLLISION_PLAYER_BEAD;
	}
};


//server -> client
//room
struct SC_SyncRoom
	: public tPacketHeader
{
	bool m_bIsHost;
	bool m_bIsReady;
	int	 m_iTeamIndex;
	int	 m_iID;

	SC_SyncRoom()
	{
		size = sizeof(*this);
		type = SC_SYNC_ROOM;
	}
};
struct SC_ConnectRoom
	: public tPacketHeader
{
	bool m_bIsHost;
	bool m_bIsGameMode;
	int	 m_iTeamIndex;
	int	 m_iID;

	int  m_iRoundCount;
	int  m_iBotCount;

	SC_ConnectRoom()
	{
		size = sizeof(*this);
		type = SC_CONNECT_ROOM;
	}
};
struct SC_Ready
	: public tPacketHeader
{
	bool m_bIsAll;

	bool m_bReady;
	int	 m_iID;

	SC_Ready()
	{
		size = sizeof(*this);
		type = SC_CLIENT_READY;
	}
};
struct SC_Change
	: public tPacketHeader
{
	int	 m_iTeamIndex;
	int  m_iID;

	SC_Change()
	{
		size = sizeof(*this);
		type = SC_CLIENT_CHANGE;
	}
};
struct SC_OptionChange
	: public tPacketHeader
{
	bool m_bGameMode;
	int m_iRoundCount;
	int m_iBotCount;

	SC_OptionChange()
	{
		size = sizeof(*this);
		type = SC_OPTION_CHANGE;
	}
};
struct SC_ChatRoom
	: public tPacketHeader
{
	char m_szChat[MAX_CHAT];
	int	 m_iID;

	SC_ChatRoom()
	{
		ZeroMemory(m_szChat, sizeof(char) * MAX_CHAT);

		size = sizeof(*this);
		type = SC_CHAT_ROOM;
	}
};
struct SC_AllUnready
	: public tPacketHeader
{
	SC_AllUnready()
	{
		size = sizeof(*this);
		type = SC_ALL_UNREADY;
	}
};
//room & client
struct SC_RemoveClient
	: public tPacketHeader
{
	int m_iID;
	int m_iHostID;

	SC_RemoveClient()
	{
		size = sizeof(*this);
		type = SC_REMOVE_CLIENT;
	}
};
//game
struct SC_ServerGameTime
	: public tPacketHeader
{
	int m_iTime;

	SC_ServerGameTime()
	{
		size = sizeof(*this);
		type = SC_SERVER_GAME_TIME;
	}
};
struct SC_ChatGame
	: public tPacketHeader
{
	int	 m_iID;
	char m_szChat[MAX_CHAT];

	SC_ChatGame()
	{
		ZeroMemory(m_szChat, sizeof(char) * MAX_CHAT);

		size = sizeof(*this);
		type = SC_CHAT_GAME;
	}
};
struct SC_InitPlayer
	: public tPacketHeader
{
	bool		m_bIsSoloOrTeam[MAX_USER];
	bool		m_bIsRedOrBlue[MAX_USER];
	D3DXVECTOR3 m_vPlayerPos[MAX_USER];

	SC_InitPlayer()
	{
		ZeroMemory(m_bIsSoloOrTeam, sizeof(bool) * MAX_USER);
		ZeroMemory(m_bIsRedOrBlue, sizeof(bool) * MAX_USER);
		ZeroMemory(m_vPlayerPos, sizeof(D3DXVECTOR3) * MAX_USER);

		size = sizeof(*this);
		type = SC_INIT_PLAYER;
	}
};
struct SC_InitBot
	: public tPacketHeader
{
	int			m_iBotCount;
	D3DXVECTOR3 m_vBotPos[MAX_BOT_COUNT];

	SC_InitBot()
	{
		ZeroMemory(m_vBotPos, sizeof(D3DXVECTOR3) * MAX_BOT_COUNT);

		size = sizeof(*this);
		type = SC_INIT_BOT;
	}
};
struct SC_InitBead
	: public tPacketHeader
{
	D3DXVECTOR3 m_vBeadPos[BEAD_END];

	SC_InitBead()
	{
		size = sizeof(*this);
		type = SC_INIT_BEAD;
	}
};
struct SC_ChangeStatePlayer
	: public tPacketHeader
{
	int			m_iPlayerID;
	DWORD		m_iPlayerKeyState;
	float		m_fPlayerAngleY;
	D3DXVECTOR3 m_vPlayerPos;

	SC_ChangeStatePlayer()
	{
		size = sizeof(*this);
		type = SC_CHANGE_STATE_PLAYER;
	}
};
struct SC_ChangeStateBot
	: public tPacketHeader
{
	int			m_iBotID;
	int			m_iBotState;
	int			m_iBotTimeCount;
	float		m_fAngleY;
	D3DXVECTOR3 m_vBotPos;

	SC_ChangeStateBot()
	{
		size = sizeof(*this);
		type = SC_CHANGE_STATE_BOT;
	}
};
struct SC_ChangeStateBead
	: public tPacketHeader
{
	int m_iBeadID;
	int m_iOwnerPlayerID;

	SC_ChangeStateBead()
	{
		size = sizeof(*this);
		type = SC_CHANGE_STATE_BEAD;
	}
};
struct SC_SyncPlayer
	: public tPacketHeader
{
	float		m_fPlayerAngleY[MAX_USER];
	D3DXVECTOR3 m_vPlayerPos[MAX_USER];

	SC_SyncPlayer()
	{
		ZeroMemory(m_fPlayerAngleY, sizeof(float) * MAX_USER);
		ZeroMemory(m_vPlayerPos, sizeof(D3DXVECTOR3) * MAX_USER);

		size = sizeof(*this);
		type = SC_SYNC_PLAYER;
	}
};
struct SC_SyncBot
	: public tPacketHeader
{
	int			m_iBotCount;
	float		m_fBotAngleY[MAX_BOT_COUNT];
	D3DXVECTOR3 m_vBotPos[MAX_BOT_COUNT];

	SC_SyncBot()
	{
		ZeroMemory(m_fBotAngleY, sizeof(float) * MAX_BOT_COUNT);
		ZeroMemory(m_vBotPos, sizeof(D3DXVECTOR3) * MAX_BOT_COUNT);

		size = sizeof(*this);
		type = SC_SYNC_BOT;
	}
};
struct SC_CheckCollision
	: public tPacketHeader
{
	//1.Player Player
	//2.Player Bot
	//3.Player Bead
	int m_iType;

	int m_iMainID;
	int m_iTargetID;

	SC_CheckCollision()
	{
		size = sizeof(*this);
		type = SC_CHECK_COLLISION;
	}
};
struct SC_CountDown
	: public tPacketHeader
{
	int m_iCountDownNum;

	SC_CountDown()
	{
		size = sizeof(*this);
		type = SC_COUNT_DOWN;
	}
};
struct SC_GameState
	: public tPacketHeader
{
	int		m_iGameState;
	bool	m_bGameWinOrLose;

	SC_GameState()
	{
		size = sizeof(*this);
		type = SC_GAME_STATE;
	}
};
#pragma pack (pop)