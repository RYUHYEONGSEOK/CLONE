#pragma once

//Trans Type
enum ANGLE { ANGLE_X, ANGLE_Y, ANGLE_Z, ANGLE_END, };
//Resource Type
enum TEXTURETYPE { TEXTURE_NORMAL, TEXTURE_CUBE };
enum MESHTYPE { MESH_STATIC, MESH_DYNAMIC, };
//Render Type
enum RENDERGROUP { RENDERTYPE_PRIORITY, RENDERTYPE_NONEALPHA, RENDERTYPE_ALPHA, RENDERTYPE_WATER, RENDERTYPE_UI, RENDERTYPE_END };
enum RENDERSTATETYPE { RS_NORMAL, RS_WIREFRAME, RS_END };
enum DXGIFORMATTYPE { DXGI_16, DXGI_32 , DXGI_END };
enum DRAWTYPE { DRAW_NORMAL, DRAW_INDEX, DRAW_END };
//Light Type
enum LIGHTINGTYPE { LIGHT_DIR, LIGHT_POINT, LIGHT_SPOT, LIGHT_END };

//In Game Type
enum OBJTYPE
{
	OBJ_PLAYER,
	OBJ_ENEMY,
	OBJ_BOT,
	OBJ_BEAD,
	OBJ_ETC,
	OBJ_END
};
enum BEADTYPE
{
	BEAD_C,
	BEAD_L,
	BEAD_O,
	BEAD_N,
	BEAD_E,
	BEAD_END
};
enum TEAMTYPE
{
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SOLO,
	TEAM_END
};

enum STAGESTATETYPE 
{ 
	STAGESTATE_READY, 
	STAGESTATE_PLAY, 
	STAGESTATE_DEFT, 
	STAGESTATE_ROUNDOVER, 
	STAGESTATE_VIC, 
	STAGESTATE_END 
};