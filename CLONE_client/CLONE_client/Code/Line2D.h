#pragma once

class CLine2D
{
public:
	HRESULT InitLine(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint);
	bool Check_LinePass(const D3DXVECTOR2* pMoveEndPoint);
	
public:
	D3DXVECTOR2		GetLineNormal(void) { return m_vNormal; }
	D3DXVECTOR2		GetStartPos(void)	{ return m_vStartPoint; }
	D3DXVECTOR2		GetEndPos(void)		{ return m_vEndPoint; }

public:
	static CLine2D* Create(const D3DXVECTOR3* pStartPoint, const D3DXVECTOR3* pEndPoint);

private:
	D3DXVECTOR2		m_vStartPoint;
	D3DXVECTOR2		m_vEndPoint;
	D3DXVECTOR2		m_vNormal;

private:
	CLine2D(void);
public:
	~CLine2D(void);
};