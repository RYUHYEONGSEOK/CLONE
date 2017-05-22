#pragma once

#include "Include.h"

#include "Transform.h"
#include "Shader.h"
#include "VIBuffer.h"

class CDevice;
class CBuffer;
class CShader;
class CTexture;
class CComponent;
class CObj
{
public:
	enum ObjState
	{
		OBJ_STATE_MOVE,
		OBJ_STATE_RUN,
		OBJ_STATE_IDEL,
		OBJ_STATE_ATT,
		OBJ_STATE_DIE,
		OBJ_STATE_JUMP,
		OBJ_STATE_SIT,
		OBJ_STATE_END
	};

protected:
	CDevice*			m_pDevice;
	wstring				m_wstrObjID;	//OBJ들 각각의 아이디
	TEAMTYPE			m_eTeamType;	//OBJ들 각각의 팀
	ObjState			m_eObjState;	//OBJ들 각각의 상태
	OBJTYPE				m_eObjType;		//OBJ들 각각의 타입
	STAGESTATETYPE		m_eStageState;	//스테이지의 상태를 OBJ들이 알게할 변수

	unordered_map<wstring, CComponent*>	m_mapComponent;

protected:
	CObj*				m_pCam;
	CTransform*			m_pInfo;
	CBuffer*			m_pBuffer;
	CShader*			m_pShader;
	CShader*			m_pBoundingBoxShader;
	CShader*			m_pDepthShader;
	CTexture*			m_pTexture;	
	LightInfo*			m_pLightInfo;

protected:
	XMFLOAT4X4			m_matView;
	XMFLOAT4X4			m_matProj;

	float				m_fViewZ;
	
	bool				m_bCollision;

public:
	RENDERGROUP			m_eRenderType;

public:
	void	Update_Component();
	void	SetViewProjTransform();

	const float& GetViewZ(void) const;

protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);

private:
	void	Release_Component();

public:
	virtual HRESULT	Init()				PURE;
	virtual int		Update()			PURE;
	virtual void	Render()			PURE;
	virtual void	Release()			PURE;

	virtual void	RenderForShadow(LightMatrixStruct tLight)	PURE;

public:
	CComponent*		GetComponent(const wstring&	_wstrComponentKey);

	wstring			GetObjID(void)					{ return m_wstrObjID; }
	ObjState		GetObjState(void)				{ return m_eObjState; }
	OBJTYPE			GetObjType(void)				{ return m_eObjType; }
	STAGESTATETYPE	GetStageState(void)				{ return m_eStageState; }
	TEAMTYPE		GetTeamType(void)				{ return m_eTeamType; }
	D3DXVECTOR3		GetPos(void)					{ return m_pInfo->m_vPos;}
	D3DXVECTOR3		GetDir(void)					{ return m_pInfo->m_vDir; }
	
	void		SetObjID(const wstring& wstrID)		{ m_wstrObjID = wstrID; }
	void		SetObjState(ObjState eObjState)		{ m_eObjState = eObjState; }
	void		SetObjType(OBJTYPE eType)			{ m_eObjType = eType; }
	void		SetCam(CObj* pObj)					{ m_pCam = pObj; }
	void		SetPos(D3DXVECTOR3 vPos)			{ m_pInfo->m_vPos = vPos; }
	void		SetDir(D3DXVECTOR3 vDir)			{ m_pInfo->m_vDir = vDir; }
	void		SetStageState(STAGESTATETYPE eType) { m_eStageState = eType; }
	void		SetTeamType(TEAMTYPE eType)			{ m_eTeamType = eType; }
	float		GetAngleY(void) { return m_pInfo->m_fAngle[ANGLE_Y]; }
	void		SetAngleY(float _fAng) { m_pInfo->m_fAngle[ANGLE_Y] = _fAng; }

protected:
	explicit CObj(CDevice* pDevice);
public:
	virtual ~CObj();
};