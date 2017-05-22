#pragma once

class CDevice;
class CScene;
class CObj;
class CBuffer;
class CRenderer
{
private:
	//ID3DX11Effect*	m_pEffect;

public:
	bool			m_bDeferredStage;

private:
	CDevice*		m_pDevice;
	CScene*			m_pScene;
	CBuffer*		m_pScreenBuffer;

public:
	//fps ฐüทร
	TCHAR		m_szFps[128];
	static DWORD	m_dwFPSCnt;
	static float	m_fTimer;

private:
	typedef list<CObj*>		RENDERLIST;
	RENDERLIST				m_RenderGroup[RENDERTYPE_END];

public:
	HRESULT	Init();
	void	Render(const float& fTime);

private:
	void Release(void);

public:
	void AddRenderGroup(RENDERGROUP eType, CObj* pGameObject);
	void AddRenderGroupFront(RENDERGROUP eType, CObj* pGameObject);
	void RemoveRenderGroup(RENDERGROUP eType, CObj* pGameObject);
	void ClearRenderGroup(void);

private:
	void Render_Priority(void);
	void Render_Shadow(void);
	void Render_NoneAlpha(void);
	void Render_Alpha(void);
	void Render_UI(void);
	void Render_FPS(const float& fTime);

private://Deferred
	void Render_Deferred(void);
	void Render_Light(void);
	void Render_Blend(void);
	void Render_DebugBuffer(void);

public:
	static CRenderer* Create(CDevice* pDevice);

public:
	void SetScene(CScene* pScene) { m_pScene = pScene; }
	
private:
	explicit CRenderer(CDevice* pDeivce);
public:
	~CRenderer(void);
};
