#pragma once
#include "Resources.h"

class CDevice;
class CBuffer abstract : public CResource
{
protected:
	ID3D11CommandList*			m_pCommandList;
	ID3D11RasterizerState*		m_pRasterizerState;

protected: // Vectex
	ID3D11Buffer*		m_pVtxBuffer;
	DWORD				m_dwVtxCnt;
	UINT				m_nVtxSize;
	UINT				m_nVtxOffset;
	UINT				m_nVtxStart;

protected: // Index
	ID3D11Buffer*		m_pIdxBuffer;
	DWORD				m_dwIdxCnt;
	UINT				m_nIdxStart;
	UINT				m_nIdxPlus;

protected:
	DWORD				m_dwTriCnt;
	NormalVertex*		vertices;
	VertexTexture*		vertices2;
	unsigned long*		indices;

public:
	ID3D11Buffer*		m_ConstantBuffer;
	ID3D11Buffer*		m_Float4Buffer;

public:
	ID3D11Buffer*		GetVertexBuffer(void)	{ return m_pVtxBuffer; }
	ID3D11Buffer*		GetIndexBuffer(void)	{ return m_pIdxBuffer; }

public:
	virtual void	Render(DXGIFORMATTYPE eFormatType = DXGI_END, DRAWTYPE eDrawType = DRAW_INDEX);
	virtual void	Release();

protected:
	virtual void	CreateRasterizerState();

protected:
	explicit CBuffer(CDevice* pDevice);
	explicit CBuffer(const CBuffer& rhs);
public:
	virtual ~CBuffer()		PURE;
};

