#pragma once

class CNaviCell;
class CDevice;
class CLineBuffer;
class CShader;
class CObj;
class CNaviMgr
{
public:
	DECLARE_SINGLETON(CNaviMgr)

public:
	void Reserve_CellContainerSize(CDevice* pDevice, const DWORD& dwSize, eMapType eMapType);
	HRESULT AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, eMapType eMapType);
	void LinkCell(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex, eMapType eMapType);
	DWORD MoveOnNaviMesh_Cam(D3DXVECTOR3* pEye, D3DXVECTOR3* pAt, const D3DXVECTOR3* pCamDir, float fCamDistance, float fCamY, const DWORD& dwIndex, eMapType eMapType);
	bool GetNaviIndex(D3DXVECTOR3 * pPos, DWORD& Index, eMapType eMapType);
	void Render_NaviMesh(eMapType eMapType);

private:
	void Release(void);

public:
	void SetCam(CObj* pCam) { m_pCam = pCam; }

private:
	CDevice*				m_pDevice;
	CObj*					m_pCam;
	CLineBuffer*			m_pBuffer[MAP_END];
	CShader*				m_pShader;

private:
	XMMATRIX				m_matWorld;

public:
	typedef vector<CNaviCell*>		VECCELL;
	VECCELL							m_vecCell[MAP_END];

private:
	DWORD			m_dwIdxCnt[MAP_END];

private:
	CNaviMgr(void);
public:
	~CNaviMgr(void);
};