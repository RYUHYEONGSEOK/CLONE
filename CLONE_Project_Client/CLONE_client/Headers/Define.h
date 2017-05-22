#pragma once

//Windowed Option Flag
//#define _WINDOWED

//MAX RESOURCE
#define RESOURCE_CNT_STAGE 54

//Water Height
#define WATER_HEIGHT -1.5f
#define WATER_CNTX 9
#define WATER_CNTZ 40

//MeshMap Option
#define HALFMAPX 80
#define HALFMAPZ 40

//Terrin Option
#define VTXITV 1
#define VTXCNTX	129
#define VTXCNTZ 129

//Path Size
#define MAXPATH	256
#define MINPATH 128

//ALL RenderTarget Num
#define RENDERTARGET_NUM 5
//RenderTarget Deferred Num
#define RENDERTARGET_DEFERRED_NUM 3
//RenderTarget Light Num
#define RENDERTARGET_LIGHT_NUM 2
//ShaderResourceView
#define SHADERRESOURCEVIEW_NUM 3

//KillLog Pos Option 
#define KILLLOG_ORIGEN_X	WINCX - (WINCX / 8.5f)
#define KILLLOG_ORIGEN_Y	WINCY / 20.f
#define KILLLOG_AUG_Y		WINCY / 20.f	// y값 증가량