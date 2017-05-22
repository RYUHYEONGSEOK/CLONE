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
	void Reserve_CellContainerSize(CDevice* pDevice, const DWORD& dwSize);
	HRESULT AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);
	void LinkCell(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex);
	DWORD MoveOnNaviMesh_Cam(D3DXVECTOR3* pEye, D3DXVECTOR3* pAt, const D3DXVECTOR3* pCamDir, float fCamDistance, float fCamY, const DWORD& dwIndex);
	bool GetNaviIndex(D3DXVECTOR3 * pPos, DWORD& Index);
	void Render_NaviMesh(void);

private:
	void Release(void);

public:
	void SetCam(CObj* pCam) { m_pCam = pCam; }

private:
	CDevice*				m_pDevice;
	CObj*					m_pCam;
	CLineBuffer*			m_pBuffer;
	CShader*				m_pShader;

private:
	DWORD					m_dwReserveSize;
	XMMATRIX				m_matWorld;

public:
	typedef vector<CNaviCell*>		VECCELL;
	VECCELL							m_vecCell;

private:
	DWORD			m_dwIdxCnt;

private:
	CNaviMgr(void);
public:
	~CNaviMgr(void);
};