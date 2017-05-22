#pragma once

#include "Texture.h"

class CResource;
class CDevice;
class CResourceMgr
{
	DECLARE_SINGLETON(CResourceMgr)

public:
	enum ResourceAttri
	{
		RESOURCE_ATTRI_STATIC,
		RESOURCE_ATTRI_DYNAMIC,

		RESOURCE_ATTRI_END
	};
	enum ResourceType
	{
		RESOURCE_TYPE_SHADER,
		RESOURCE_TYPE_BUFFER,
		RESOURCE_TYPE_TEXTURE,
		RESOURCE_TYPE_MESH,
		RESOURCE_TYPE_FONT,

		RESOURCE_TYPE_END
	};

	enum ShaderType
	{
		SHADER_TYPE_TERRAIN,
		SHADER_TYPE_COLOR,
		SHADER_TYPE_TEXTURE,
		SHADER_TYPE_FONT,
		SHADER_TYPE_DEFAULT,
		SHADER_TYPE_BLEND,
		SHADER_TYPE_DEFERRED,
		SHADER_TYPE_STATICMESH,
		SHADER_TYPE_DYNAMICMESH,
		SHADER_TYPE_LOGOBACK,
		SHADER_TYPE_SKYBOX,
		SHADER_TYPE_LINE,
		SHADER_TYPE_DIRLIGHT,
		SHADER_TYPE_POINTLIGHT,
		SHADER_TYPE_DEPTH,
		SHADER_TYPE_DEPTHDYNAMIC,
		SHADER_TYPE_WATER,
		SHADER_TYPE_ANI
	};
	enum BufferType
	{
		BUFFER_TYPE_TERRAIN,
		BUFFER_TYPE_CUBE,
		BUFFER_TYPE_PLANE,
		BUFFER_TYPE_SKYBOX,
		BUFFER_TYPE_WATER,
		BUFFER_TYPE_FOUNTAINWATER,
		BUFFER_TYPE_UI,
		BUFFER_TYPE_LOGO,
	};

	enum MeshType { MESH_TYPE_DYNAMIC, MESH_TYPE_STATIC };


private:
	map<wstring, CResource*>		m_mapResource[RESOURCE_ATTRI_END][RESOURCE_TYPE_END];


public:
	CResource*		FindResource(const ResourceAttri eResourceAtrri,
		const ResourceType eResourceType, const wstring& wstrKey);
	CResource*		CloneResource(const ResourceAttri eResourceAtrri,
		const ResourceType eResourceType, const wstring& wstrKey);

	HRESULT			Add_Shader(CDevice* pDevice, const ResourceAttri eResourceAtrri,
		const ShaderType eShaderType, const wstring& wstrKey);
	HRESULT			Add_Buffer(CDevice* pDevice, const ResourceAttri eResourceAtrri,
		const BufferType eBufferType, const wstring& wstrKey);
	HRESULT			Add_TerrainBuffer(CDevice* pDevice, const ResourceAttri eResourceAtrri,
		const BufferType eBufferType, const wstring& wstrKey,
		const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);
	HRESULT			Add_Texture(CDevice* pDevice, const ResourceAttri eResourceAtrri,
		const CTexture::TextureType& eTextureType, const wstring& wstrKey, const wstring& wstrPath, const WORD &wCnt = 0);
	HRESULT			Add_Mesh(CDevice* pDevice, ResourceAttri eResourceAtrri,
		const MeshType eModelType, const wstring& wstrKey,
		const char* pPath, vector<string> vecAniName = vector<string>());
	HRESULT			Add_Font(CDevice* pDevice, ResourceAttri eResourceAtrri,
		const wstring& wstrKey, const wstring& wstrPath);


public:
	void	Release_Dynamic();
	void	Release_All();


private:
	CResourceMgr();
	~CResourceMgr();
};