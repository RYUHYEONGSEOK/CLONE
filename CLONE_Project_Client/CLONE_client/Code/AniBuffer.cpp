#include "stdafx.h"
#include "AniBuffer.h"

CAniBuffer::CAniBuffer(CDevice* pDevice)
	: CBuffer(pDevice)
	, m_nOffset(0)
	, m_nStride(sizeof(VertexAni))
	, m_nStartIndex(0)
	, m_nBaseVertex(0)
	, m_nVertices(0)
{
	this->m_pVertexBuffer = nullptr;
}

CAniBuffer::CAniBuffer(const CAniBuffer & rhs)
	: CBuffer(rhs.m_pDevice)
{
	m_pDevice = rhs.m_pDevice;

	m_llAniTime = rhs.m_llAniTime;
	m_strName = rhs.m_strName;

	m_pVertex = rhs.m_pVertex;
	m_nStartIndex = rhs.m_nStartIndex;
	m_nBaseVertex = rhs. m_nBaseVertex;

	m_nVertices = rhs.m_nVertices;
	m_nStride = rhs.m_nStride;
	m_nOffset = rhs.m_nOffset;

	for (size_t i = 0; i < m_vecChildBuffer.size(); ++i)
	{
		CAniBuffer* pBuffer = CAniBuffer::Create(rhs.m_pDevice);
		pBuffer = rhs.m_vecChildBuffer[i];
		m_vecChildBuffer.push_back(pBuffer);
	}

	m_pVertexBuffer = rhs.m_pVertexBuffer;
}

CAniBuffer::~CAniBuffer()
{
	Release();
}

CAniBuffer * CAniBuffer::Clone(void)
{
	++(*m_pRefCnt);
	return new CAniBuffer(*this);
}

VertexAni* CAniBuffer::GetVertex(unsigned int _nIndex)
{
	if (this->m_nVertices <= _nIndex)
	{
		MSG_BOX(L"WARNNING! Out Of Range");
		return nullptr;
	}
	else
		return &m_pVertex[_nIndex];
}

void CAniBuffer::SetVertexSize(unsigned int _iSize)
{
	this->m_nVertices = _iSize;
	this->m_pVertex = new VertexAni[_iSize];
}

CAniBuffer* CAniBuffer::GetChild(unsigned int _nindex)
{
	return m_vecChildBuffer[_nindex];
}

void CAniBuffer::AddChild(CAniBuffer* _pChildBuffer)
{
	this->m_vecChildBuffer.push_back(_pChildBuffer);
}

HRESULT CAniBuffer::CreateBuffer()
{
	if (this->m_nVertices != 0)
	{
		unsigned int ByteWidth
			= ((unsigned int)((this->m_nStride * this->m_nVertices) / 16) * 16) + 16;

		D3D11_BUFFER_DESC BD;
		ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
		BD.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		BD.ByteWidth = ByteWidth;
		BD.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		BD.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA SD;
		ZeroMemory(&SD, sizeof(D3D11_SUBRESOURCE_DATA));
		SD.pSysMem = this->m_pVertex;

		if (FAILED(m_pDevice->GetDevice()->CreateBuffer(&BD, &SD, &this->m_pVertexBuffer)))
		{
			MSG_BOX(L"Vertex Buffer Create Failed!");
			getchar();
			exit(0);
		}
	}

	return S_OK;
}

void CAniBuffer::SetBuffer()
{
	if (this->m_pVertexBuffer)
	{
		m_pDevice->GetDeviceContext()->IASetVertexBuffers(
			0,
			1,
			&this->m_pVertexBuffer,
			&this->m_nStride,
			&this->m_nOffset);
	}
}

void CAniBuffer::SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName,
	FbxNode* _pNode)
{
	(*_pIndexByName)[_pNode->GetName()] = _pIndexByName->size();

	for (int i = 0; i < _pNode->GetChildCount(); ++i)
		this->SetFbxBoneIndex(_pIndexByName, _pNode->GetChild(i));
}

void CAniBuffer::Update()
{

}

void CAniBuffer::Render(DXGIFORMATTYPE eFormatType)
{
	if (this->m_nVertices != 0)
	{
		this->SetBuffer();
		m_pDevice->GetDeviceContext()->Draw(this->m_nVertices, this->m_nStartIndex);
	}

	for (auto iter : this->m_vecChildBuffer)
		iter->Render(DXGI_32);
}

void CAniBuffer::Release()
{
	for (auto iter : this->m_vecChildBuffer)
	{
		Safe_Release(iter->m_pVertexBuffer);
		Safe_Delete_Array(iter->m_pVertex);
		Safe_Delete(iter);
	}
}

CAniBuffer * CAniBuffer::Create(CDevice * pDevice)
{
	CAniBuffer*		pBuffer = new CAniBuffer(pDevice);

	if (FAILED(pBuffer->CreateBuffer()))
		Safe_Delete(pBuffer);

	return pBuffer;
}
