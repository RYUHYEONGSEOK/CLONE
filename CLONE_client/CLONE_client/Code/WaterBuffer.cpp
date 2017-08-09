#include "stdafx.h"
#include "WaterBuffer.h"

CWaterBuffer::CWaterBuffer(CDevice * pDevice)
	: CBuffer(pDevice)
	, m_model(NULL)
{

}

CWaterBuffer::CWaterBuffer(const CWaterBuffer & rhs)
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
	m_model = rhs.m_model;
}

CWaterBuffer::~CWaterBuffer(void)
{
	Release();
}


HRESULT CWaterBuffer::CreateBuffer(const WORD& wCntX, const WORD& wCntZ)
{
	FAILED_CHECK_RETURN(LoadModel(L"../bin/Resources/Texture/Water/Water.txt"), E_FAIL);

	m_nVtxSize = sizeof(VertexTexture);
	m_nVtxOffset = 0;
	
	vertices2 = new VertexTexture[m_dwVtxCnt];
	unsigned long* Index = new unsigned long[m_dwIdxCnt];

	for (DWORD iIndex = 0; iIndex < m_dwVtxCnt; iIndex++)
	{
		vertices2[iIndex].vPos = D3DXVECTOR3(m_model[iIndex].x, m_model[iIndex].y, m_model[iIndex].z);
		vertices2[iIndex].vTextureUV = D3DXVECTOR2(m_model[iIndex].tu, m_model[iIndex].tv);
		vertices2[iIndex].vNormal = D3DXVECTOR3(m_model[iIndex].nx, m_model[iIndex].ny, m_model[iIndex].nz);

		Index[iIndex] = iIndex;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = m_nVtxSize * m_dwVtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices2;
	FAILED_CHECK_RETURN(m_pDevice->GetDevice()->CreateBuffer(&bd, &InitData, &m_pVtxBuffer), E_FAIL);


	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = sizeof(unsigned long) * m_dwIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = Index;
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

	CreateRasterizerState();

	//¼öÁ¤
	Safe_Delete(vertices2);
	Safe_Delete(Index);

	return S_OK;
}

CWaterBuffer * CWaterBuffer::Create(CDevice * pDevice, const WORD& wSizeX, const WORD& wSizeZ)
{
	CWaterBuffer*		pBuffer = new CWaterBuffer(pDevice);

	if (FAILED(pBuffer->CreateBuffer(wSizeX, wSizeZ)))
		Safe_Delete(pBuffer);

	return pBuffer;
}

CResource * CWaterBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CWaterBuffer(*this);
}

void CWaterBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	CBuffer::Render(eFormatType);
}

void CWaterBuffer::Update()
{

}

void CWaterBuffer::Release()
{
	
}

void CWaterBuffer::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;
	tRasterizerDesc.AntialiasedLineEnable = TRUE;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

HRESULT CWaterBuffer::LoadModel(const TCHAR* pFileName)
{
	ifstream fin;
	char input;
	int i;

	//Open the model file
	fin.open(pFileName);

	//if it could not open the file then exit
	if (fin.fail())
	{
		return false;
	}

	//Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//Read in the vertex count
	fin >> m_dwVtxCnt;

	//set the numder of indices to be the same as the vertex count
	m_dwIdxCnt = m_dwVtxCnt;

	//Create the model using the vertex count that was read in
	m_model = new ModelType[m_dwVtxCnt];
	if (!m_model)
	{
		return false;
	}

	//Read up to the beginning of the data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//Read in the vertex data
	for (i = 0; i < (int)m_dwVtxCnt; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	//Close the file
	fin.close();

	return S_OK;
}

