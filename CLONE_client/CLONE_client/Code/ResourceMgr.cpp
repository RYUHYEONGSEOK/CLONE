#include "stdafx.h"
#include "ResourceMgr.h"
//Utility
#include "Functor.h"
#include "Resource.h"
//Buffer & Shader
#include "TerrainCol.h"
#include "TerrainShader.h"
#include "UIBuffer.h"
#include "TextureShader.h"
#include "SkyBoxBuffer.h"
#include "SkyBoxShader.h"
#include "StaticMesh.h"
#include "StaticMeshShader.h"
#include "DynamicMesh.h"
#include "DynamicMeshShader.h"
#include "Font.h"
#include "FontShader.h"
#include "PlaneBuffer.h"
#include "LineShader.h"
#include "DefaultShader.h"
#include "BlendShader.h"
#include "DeferredShader.h"
#include "LogoBackShader.h"
#include "LogoBuffer.h"
#include "DirLightShader.h"
#include "PointLightShader.h"
#include "WaterBuffer.h"
#include "DepthShader.h"
#include "DepthDynamicShader.h"
#include "WaterShader.h"
#include "FountainBuffer.h"
#include "ReflectionShader.h"
#include "RefactionShader.h"
#include "RefactionForDynamicShader.h"

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
{

}

CResourceMgr::~CResourceMgr()
{
	Release_All();
}

CResource* CResourceMgr::FindResource(const ResourceAttri eResourceAtrri,
	const ResourceType eResourceType,
	const wstring& wstrKey)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][eResourceType].find(wstrKey);

	if (iter == m_mapResource[eResourceAtrri][eResourceType].end())
	{
		TAGMSG_BOX(wstrKey.c_str(), L"리소스 원형이 존재하지 않습니다.");
		return NULL;
	}

	return iter->second;
}

CResource* CResourceMgr::CloneResource(const ResourceAttri eResourceAtrri,
	const ResourceType eResourceType,
	const wstring& wstrKey)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][eResourceType].find(wstrKey);

	if (iter == m_mapResource[eResourceAtrri][eResourceType].end())
	{
		TAGMSG_BOX(wstrKey.c_str(), L"리소스 원형이 존재하지 않습니다.");
		return NULL;
	}

	return iter->second->Clone();
}

HRESULT CResourceMgr::Add_Shader(CDevice* pDevice,
	const ResourceAttri eResourceAtrri,
	const ShaderType eShaderType,
	const wstring& wstrKey)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][RESOURCE_TYPE_SHADER].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_SHADER].end())
		return S_OK;


	CResource*		pResource = NULL;

	switch (eShaderType)
	{
	case SHADER_TYPE_TERRAIN:		pResource	= CTerrainShader::Create(pDevice, L"../bin/Shader/TerrainShader.Shader"); break;
	case SHADER_TYPE_TEXTURE:		pResource	= CTextureShader::Create(pDevice, L"../bin/Shader/TextureShader.Shader"); break;
	case SHADER_TYPE_STATICMESH:	pResource	= CStaticMeshShader::Create(pDevice, L"../bin/Shader/StaticMeshShader.Shader"); break;
	case SHADER_TYPE_DYNAMICMESH:	pResource	= CDynamicMeshShader::Create(pDevice, L"../bin/Shader/DynamicMeshShader.Shader"); break;
	case SHADER_TYPE_SKYBOX:		pResource	= CSkyBoxShader::Create(pDevice, L"../bin/Shader/SkyBoxShader.Shader");	break;
	case SHADER_TYPE_FONT:			pResource	= CFontShader::Create(pDevice, L"../bin/Shader/FontShader.Shader"); break;
	case SHADER_TYPE_LINE:			pResource	= CLineShader::Create(pDevice, L"../bin/Shader/LineShader.Shader"); break;
	case SHADER_TYPE_DEFAULT:		pResource	= CDefaultShader::Create(pDevice, L"../bin/Shader/DefaultShader.Shader"); break;
	case SHADER_TYPE_BLEND:			pResource	= CBlendShader::Create(pDevice, L"../bin/Shader/BlendShader.Shader"); break;
	case SHADER_TYPE_DEFERRED:		pResource	= CDeferredShader::Create(pDevice, L"../bin/Shader/DeferredShader.Shader"); break;
	case SHADER_TYPE_LOGOBACK:		pResource	= CLogoBackShader::Create(pDevice, L"../bin/Shader/LogoBackShader.Shader"); break;
	case SHADER_TYPE_DIRLIGHT:		pResource	= CDirLightShader::Create(pDevice, L"../bin/Shader/DirLightShader.Shader"); break;
	case SHADER_TYPE_POINTLIGHT:	pResource	= CPointLightShader::Create(pDevice, L"../bin/Shader/PointLightShader.Shader"); break;
	case SHADER_TYPE_DEPTH:			pResource	= CDepthShader::Create(pDevice, L"../bin/Shader/DepthShader.Shader"); break;
	case SHADER_TYPE_DEPTHDYNAMIC:	pResource	= CDepthDynamicShader::Create(pDevice, L"../bin/Shader/DepthDynamicShader.Shader"); break;
	case SHADER_TYPE_WATER:			pResource	= CWaterShader::Create(pDevice, L"../bin/Shader/WaterShader.Shader"); break;
	case SHADER_TYPE_REFLECTION:	pResource	= CReflectionShader::Create(pDevice, L"../bin/Shader/ReflectionShader.Shader"); break;
	case SHADER_TYPE_REFACTION:		pResource	= CRefactionShader::Create(pDevice, L"../bin/Shader/RefactionShader.Shader"); break;
	case SHADER_TYPE_REFACTIONFORDYNAMIC: pResource = CRefactionForDynamicShader::Create(pDevice, L"../bin/Shader/RefactionForDynamicShader.Shader"); break;
	default:
		return E_FAIL;
	}

	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_SHADER].insert(make_pair(wstrKey, pResource));
	return S_OK;
}

HRESULT CResourceMgr::Add_Buffer(CDevice* pDevice,
	const ResourceAttri eResourceAtrri,
	const BufferType eBufferType,
	const wstring& wstrKey)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].end())
		return S_OK;

	CResource*		pResource = NULL;

	switch (eBufferType)
	{
		//case BUFFER_TYPE_CUBE:		pResource = CCubeBuffer::Create(_pDevice);							break;
		case BUFFER_TYPE_PLANE:			pResource = CPlaneBuffer::Create(pDevice);							break;
		case BUFFER_TYPE_SKYBOX:		pResource = CSkyBoxBuffer::Create(pDevice);							break;
		case BUFFER_TYPE_UI:			pResource = CUIBuffer::Create(pDevice);								break;
		case BUFFER_TYPE_WATER:			pResource = CWaterBuffer::Create(pDevice, WATER_CNTX, WATER_CNTZ);	break;
		case BUFFER_TYPE_LOGO:			pResource = CLogoBuffer::Create(pDevice);							break;
		case BUFFER_TYPE_FOUNTAINWATER: pResource = CFountainBuffer::Create(pDevice);						break;
	default:
		return E_FAIL;
	}

	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].insert(make_pair(wstrKey, pResource));
	return S_OK;
}

HRESULT CResourceMgr::Add_TerrainBuffer(CDevice * pDevice,
	const ResourceAttri eResourceAtrri,
	const BufferType eBufferType,
	const wstring & wstrKey,
	const WORD & wCntX, const WORD & wCntZ, const WORD & wItv)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].end())
		return S_OK;

	CResource*		pResource = NULL;

	switch (eBufferType)
	{
		case BUFFER_TYPE_TERRAIN:	pResource = CTerrainCol::Create(pDevice, wCntX, wCntZ, wItv); break;

	default:
		return E_FAIL;
	}

	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_BUFFER].insert(make_pair(wstrKey, pResource));

	return S_OK;
}

HRESULT CResourceMgr::Add_Texture(CDevice* pDevice,
	const ResourceAttri eResourceAtrri,
	const CTexture::TextureType& eTextureType,
	const wstring& wstrKey,
	const wstring& wstrPath,
	const WORD &wCnt/* = 0*/)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][RESOURCE_TYPE_TEXTURE].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_TEXTURE].end())
		return S_OK;

	CResource* pResource = CTexture::Create(pDevice, wstrPath, eTextureType, wCnt);
	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_TEXTURE].insert(make_pair(wstrKey, pResource));

	return S_OK;
}

HRESULT CResourceMgr::Add_Mesh(CDevice* pDevice,
	const ResourceAttri eResourceAtrri,
	const MeshType eModelType,
	const wstring& wstrKey,
	const char* pPath,
	vector<string> vecAniName /*= vector<string>()*/)
{
	map<wstring, CResource*>::iterator iter
		= m_mapResource[eResourceAtrri][RESOURCE_TYPE_MESH].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_MESH].end())
		return S_OK;


	CResource*		pResource = NULL;

	switch (eModelType)
	{
		case CResourceMgr::MESH_TYPE_DYNAMIC:		pResource = CDynamicMesh::Create(pDevice, pPath, vecAniName); break;
		case CResourceMgr::MESH_TYPE_STATIC:		pResource = CStaticMesh::Create(pDevice, pPath);	break;

	default:
		return E_FAIL;
	}

	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_MESH].insert(make_pair(wstrKey, pResource));
	return S_OK;
}

HRESULT CResourceMgr::Add_Font(CDevice* pDevice,
	ResourceAttri eResourceAtrri,
	const wstring& wstrKey,
	const wstring& wstrPath)
{
	map<wstring, CResource*>::iterator iter
	= m_mapResource[eResourceAtrri][RESOURCE_TYPE_FONT].find(wstrKey);

	if (iter != m_mapResource[eResourceAtrri][RESOURCE_TYPE_FONT].end())
	return S_OK;

	CResource* pResource = CFont::Create(pDevice, wstrPath);
	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_mapResource[eResourceAtrri][RESOURCE_TYPE_FONT].insert(make_pair(wstrKey, pResource));
	return E_FAIL;

	return S_OK;
}

void CResourceMgr::Release_Dynamic()
{
	for (int i = 0; i < RESOURCE_TYPE_END; ++i)
	{
		for_each(m_mapResource[RESOURCE_ATTRI_DYNAMIC][i].begin(),
			m_mapResource[RESOURCE_ATTRI_DYNAMIC][i].end(), CDeleteMap());
		m_mapResource[RESOURCE_ATTRI_DYNAMIC][i].clear();
	}
}

void CResourceMgr::Release_All()
{
	for (int i = 0; i < RESOURCE_ATTRI_END; ++i)
	{
		for (int j = 0; j < RESOURCE_TYPE_END; ++j)
		{
			for_each(m_mapResource[i][j].begin(),
				m_mapResource[i][j].end(), CDeleteMap());
			m_mapResource[i][j].clear();
		}
	}
}
