#include "stdafx.h"
#include "TerrainCol.h"

CTerrainCol::CTerrainCol(CDevice* pDevice,
	const WORD& wCntX, const WORD& wCntZ, const WORD& wItv)
	: CBuffer(pDevice)
{

}

CTerrainCol::CTerrainCol(const CTerrainCol & rhs)
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

	vertices = rhs.vertices;
	indices = rhs.indices;

	m_pRefCnt = rhs.m_pRefCnt;
}

CTerrainCol::~CTerrainCol(void)
{
	Release();
}

CTerrainCol* CTerrainCol::Create(CDevice* pDevice,
	const WORD& wCntX, const WORD& wCntZ, const WORD& wItv)
{
	CTerrainCol*		pBuffer = new CTerrainCol(pDevice, wCntX, wCntZ, wItv);

	if (FAILED(pBuffer->CreateBuffer(wCntX, wCntZ, wItv)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

HRESULT CTerrainCol::CreateBuffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItv)
{
	//우선 로드
	HANDLE		hFile;
	DWORD		dwByte;

	hFile = CreateFile(L"../bin/Resources/Texture/Stage/Terrain/Height.bmp", GENERIC_READ
		, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, NULL);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, NULL);

	DWORD*		pdwPixel = new DWORD[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pdwPixel, sizeof(DWORD) * ih.biWidth * ih.biHeight, &dwByte, NULL);
	CloseHandle(hFile);
	// 로드끝

	HRESULT hr = NULL;

	m_nVtxSize = sizeof(NormalVertex);
	m_nVtxOffset = 0;
	m_dwVtxCnt = wCntX * wCntZ;
	m_dwIdxCnt = (wCntX - 1) * (wCntZ - 1) * 6;;
	m_dwTriCnt = (wCntX - 1) * (wCntZ - 1) * 2;

	vertices = new NormalVertex[m_dwVtxCnt];

	int		iIndex = 0;

	for (int z = 0; z < wCntZ; ++z)
	{
		for (int x = 0; x < wCntX; ++x)
		{
			iIndex = z * wCntX + x;

			vertices[iIndex].Pos = XMFLOAT3(float(x) * wItv, (pdwPixel[iIndex] & 0x000000ff) / 5.f, float(z) * wItv);
			vertices[iIndex].Normal = XMFLOAT3(0.f, 0.f, 0.f);
			vertices[iIndex].texture = XMFLOAT2(x / (wCntX - 1.f), z / (wCntZ - 1.f));
		}
	}

	// 버퍼 디스크 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(NormalVertex) * m_dwVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer);
	if (FAILED(hr))
		return hr;

	//그리는 순서에 따른 인덱스 정보 입력
	INDEX32* Index = new INDEX32[m_dwIdxCnt];

	int	iTriCnt = 0;

	for (int z = 0; z < wCntZ - 1; ++z)
	{
		for (int x = 0; x < wCntX - 1; ++x)
		{
			iIndex = z * wCntX + x;

			// 오른쪽 위
			Index[iTriCnt]._1 = iIndex + wCntX;
			Index[iTriCnt]._2 = iIndex + wCntX + 1;
			Index[iTriCnt]._3 = iIndex + 1;

			XMFLOAT3		vDest, vSour;
			XMVECTOR		vNormalTemp1, vNormalTemp2, vNormalTemp3;
			XMVECTOR		vNormal;
			XMVECTOR		vTemp1, vTemp2, vTemp3;

			vTemp1 = XMLoadFloat3(&vertices[Index[iTriCnt]._2].Pos);
			vTemp2 = XMLoadFloat3(&vertices[Index[iTriCnt]._1].Pos);
			vTemp3 = XMLoadFloat3(&vertices[Index[iTriCnt]._3].Pos);

			XMStoreFloat3(&vDest, vTemp1 - vTemp2);
			XMStoreFloat3(&vSour, vTemp3 - vTemp1);

			vNormal = XMVector3Cross(XMLoadFloat3(&vDest), XMLoadFloat3(&vSour));

			vNormalTemp1 = XMLoadFloat3(&vertices[Index[iTriCnt]._1].Normal);
			vNormalTemp2 = XMLoadFloat3(&vertices[Index[iTriCnt]._2].Normal);
			vNormalTemp3 = XMLoadFloat3(&vertices[Index[iTriCnt]._3].Normal);

			vNormalTemp1 += vNormal;
			vNormalTemp2 += vNormal;
			vNormalTemp3 += vNormal;

			XMStoreFloat3(&vertices[Index[iTriCnt]._1].Normal, vNormalTemp1);
			XMStoreFloat3(&vertices[Index[iTriCnt]._2].Normal, vNormalTemp2);
			XMStoreFloat3(&vertices[Index[iTriCnt]._3].Normal, vNormalTemp3);

			++iTriCnt;

			// 왼쪽 아래
			Index[iTriCnt]._1 = iIndex + wCntX;
			Index[iTriCnt]._2 = iIndex + 1;
			Index[iTriCnt]._3 = iIndex;

			vTemp1 = XMLoadFloat3(&vertices[Index[iTriCnt]._2].Pos);
			vTemp2 = XMLoadFloat3(&vertices[Index[iTriCnt]._1].Pos);
			vTemp3 = XMLoadFloat3(&vertices[Index[iTriCnt]._3].Pos);

			XMStoreFloat3(&vDest, vTemp1 - vTemp2);
			XMStoreFloat3(&vSour, vTemp3 - vTemp1);

			vNormal = XMVector3Cross(XMLoadFloat3(&vDest), XMLoadFloat3(&vSour));

			vNormalTemp1 = XMLoadFloat3(&vertices[Index[iTriCnt]._1].Normal);
			vNormalTemp2 = XMLoadFloat3(&vertices[Index[iTriCnt]._2].Normal);
			vNormalTemp3 = XMLoadFloat3(&vertices[Index[iTriCnt]._3].Normal);

			vNormalTemp1 += vNormal;
			vNormalTemp2 += vNormal;
			vNormalTemp3 += vNormal;

			XMStoreFloat3(&vertices[Index[iTriCnt]._1].Normal, vNormalTemp1);
			XMStoreFloat3(&vertices[Index[iTriCnt]._2].Normal, vNormalTemp2);
			XMStoreFloat3(&vertices[Index[iTriCnt]._3].Normal, vNormalTemp3);

			++iTriCnt;
		}
	}

	for (DWORD i = 0; i < m_dwVtxCnt; ++i)
	{
		XMVECTOR vTemp;
		vTemp = XMLoadFloat3(&vertices[i].Normal);
		vTemp = XMVector3Normalize(vTemp);

		XMStoreFloat3(&vertices[i].Normal, vTemp);
	}

	// 인덱스 버퍼 생성
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * m_dwIdxCnt;        // 12개 삼각형 == 36개 버텍스
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	InitData.pSysMem = Index;
	hr = m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pIdxBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

CResource * CTerrainCol::Clone(void)
{
	++(*m_pRefCnt);
	return new CTerrainCol(*this);
}

void CTerrainCol::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CTerrainCol::Update()
{
}

void CTerrainCol::Release()
{
	
}

void CTerrainCol::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}
