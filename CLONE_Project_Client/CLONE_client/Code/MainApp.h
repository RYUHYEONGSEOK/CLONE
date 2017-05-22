#pragma once

class CDevice;
class CTimeMgr;
class CInput;
class CSceneMgr;
class CRenderTargetMgr;
class CLightMgr;
class CResourceMgr;
class CObjMgr;
class CNaviMgr;
class CFrustum;
class CMainApp
{
private:
	CDevice*			m_pDevice;
	CTimeMgr*			m_pTimeMgr;
	CInput*				m_pInput;
	CSceneMgr*			m_pSceneMgr;
	CRenderTargetMgr*	m_pRenderTargetMgr;
	CLightMgr*			m_pLightMgr;
	CResourceMgr*		m_pResourceMgr;
	CObjMgr*			m_pObjMgr;
	CNaviMgr*			m_pNaviMgr;
	CFrustum*			m_pFrustum;

public:
	HRESULT		InitApp(void);
	void		Update(void);
	void		Render(void);
	void		Release(void);
	
public:
	CMainApp(void);
	~CMainApp(void);
};