#include "stdafx.h"
#include "CubeColor.h"

CCubeColor::CCubeColor(CDevice * pDevice)
	: CBuffer(pDevice)
{

}

CCubeColor::CCubeColor(const CCubeColor & rhs)
	: CBuffer(rhs.m_pDevice)
{
	m_pRasterizerState = rhs.m_pRasterizerState;

	m_pVtxBuffer = rhs.m_pVtxBuffer;
	m_dwVtxCnt = rhs.m_dwVtxCnt;
	m_nVtxSize = rhs.m_nVtxSize;
	m_nVtxOffset = rhs.m_nVtxOffset;
	m_nVtxStart = rhs.m_nVtxStart;

	m_pIdxBuffer = rhs.m_pIdxBuffer;
	m_dwIdxCnt = rhs.m_dwIdxCnt;
	m_nIdxStart = rhs.m_nIdxStart;
	m_nIdxPlus = rhs.m_nIdxPlus;

	m_dwTriCnt = rhs.m_dwTriCnt;

	m_ConstantBuffer = rhs.m_ConstantBuffer;
	m_Float4Buffer = rhs.m_Float4Buffer;

	vertices = rhs.vertices;
	indices = rhs.indices;

	m_pRefCnt = rhs.m_pRefCnt;
}

CCubeColor::~CCubeColor(void)
{
	Release();
}

HRESULT CCubeColor::CreateBuffer(const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXVECTOR4& vColor)
{
	m_dwVtxCnt = 8;
	m_nVtxSize = sizeof(VertexColor);
	m_nVtxOffset = 0;

	m_dwIdxCnt = 36;
	m_nIdxStart = 0;
	m_nIdxPlus = 0;

	VertexColor pVertex[] =
	{
		{ D3DXVECTOR3(vMin.x, vMax.y, vMin.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMax.x, vMax.y, vMin.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMax.x, vMin.y, vMin.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMin.x, vMin.y, vMin.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },


		{ D3DXVECTOR3(vMin.x, vMax.y, vMax.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMax.x, vMax.y, vMax.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMax.x, vMin.y, vMax.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },

		{ D3DXVECTOR3(vMin.x, vMin.y, vMax.z)
		, D3DXVECTOR4(vColor.x, vColor.y, vColor.z, vColor.w) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = m_nVtxSize * m_dwVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVertex;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer), E_FAIL);

	INDEX16 pIdx[] =
	{
		// +x
		{ INDEX16(1, 5, 6) },
		{ INDEX16(1, 6, 2) },
		// -x
		{ INDEX16(4, 0, 3) },
		{ INDEX16(4, 3, 7) },
		// +y
		{ INDEX16(4, 5, 1) },
		{ INDEX16(4, 1, 0) },
		// -y
		{ INDEX16(3, 2, 6) },
		{ INDEX16(3, 6, 7) },
		// +z
		{ INDEX16(7, 6, 5) },
		{ INDEX16(7, 5, 4) },
		// -z
		{ INDEX16(0, 1, 2) },
		{ INDEX16(0, 2, 3) },
	};

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(DWORD) * m_dwIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = pIdx;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&tBufferDesc, &tData, &m_pIdxBuffer), E_FAIL);

	//ConstantBuffer
	D3D11_BUFFER_DESC cb;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(ConstantBuffer);
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&cb, NULL, &m_ConstantBuffer), E_FAIL);

	cb.ByteWidth = sizeof(ConstantBufferFloat4);
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&cb, NULL, &m_Float4Buffer), E_FAIL);

	CreateRasterizerState();

	return S_OK;
}

CCubeColor * CCubeColor::Create(CDevice * pDevice, const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXVECTOR4& vColor)
{
	CCubeColor*		pBuffer = new CCubeColor(pDevice);

	if (FAILED(pBuffer->CreateBuffer(vMin, vMax, vColor)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CCubeColor::Clone(void)
{
	++(*m_pRefCnt);
	return new CCubeColor(*this);
}

void CCubeColor::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CCubeColor::Update()
{

}

void CCubeColor::Release()
{

}

void CCubeColor::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

