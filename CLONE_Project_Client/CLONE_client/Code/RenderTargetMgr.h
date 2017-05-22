#pragma once

class CRenderTarget;
class CDevice;
class CObj;
class CBuffer;
class CRenderTargetMgr
{
public:
	DECLARE_SINGLETON(CRenderTargetMgr)

private:
	typedef unordered_map<const TCHAR*, CRenderTarget*>		MAPTARGET;
	MAPTARGET		m_mapTarget;

private:
	typedef list<CRenderTarget*>		MRTLIST;
	typedef unordered_map<const TCHAR*, MRTLIST>			MAPMRTLIST;
	MAPMRTLIST		m_mapMRTlist;

public:
	void Set_ConstantTable(CBuffer* pBuffer, const TCHAR* pTargetTag);
	void Set_Cam(const TCHAR * pTargetTag, CObj* pCam);

public:
	HRESULT Add_RenderTarget(CDevice* pDevice, CObj* pCam, const TCHAR* pTargetTag
		, const UINT& iSizeX, const UINT& iSizeY, DXGI_FORMAT Format, D3DXCOLOR Color);
	HRESULT Add_MRT(const TCHAR* pMRTTag
		, const TCHAR* pTargetTag);
	HRESULT Add_DebugBuffer(const TCHAR* pTargetTag
		, const float& fSizeX, const float& fSizeY);
	void Render_DebugBuffer(const TCHAR* pTargetTag);
	void Render_LightTarget(const TCHAR* pTargetTag);

public:
	void Begin_MRT_Defferd(const TCHAR* pMRTTag);
	void End_MRT_Defferd(const TCHAR* pMRTTag);

	void Begin_MRT_Shadow(const TCHAR* pMRTTag);
	void End_MRT_Shadow(const TCHAR* pMRTTag);

	void Begin_MRT_Light(const TCHAR* pMRTTag);
	void End_MRT_Light(const TCHAR* pMRTTag);

public:
	CRenderTarget* Find_RenderTarget(const TCHAR* pTargetTag);
	list<CRenderTarget*>* Find_MRT(const TCHAR* pMRTTag);

private:
	void Release(void);

private:
	CRenderTargetMgr(void);
public:
	~CRenderTargetMgr(void);
};