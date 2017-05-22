#include "stdafx.h"
#include "Mesh.h"

#include "AniBuffer.h"

CMesh::CMesh(CDevice* pDevice)
	: CBuffer(pDevice)
	, m_pBoundingBox(NULL)
	, m_wCurrenAniIdx(-1)
	, m_fAniPlayTimer(0.f)
	, pFBXScene(NULL)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CBuffer(rhs.m_pDevice)
{
}

CMesh::~CMesh()
{
	Release();
}

void CMesh::Render(DXGIFORMATTYPE eFormatType)
{
	//m_pShader->Render();
	CBuffer::Render(eFormatType);
}

void CMesh::Update(void)
{
}

void CMesh::Release(void)
{
	if ((*m_pRefCnt) == 0)
	{
		Safe_Delete(m_pBoundingBox);
		
		for (auto i = m_vecAni.begin(); i != m_vecAni.end(); ++i)
		{
			Safe_Delete((*i)->pAniBuffer);
			Safe_Delete_Array((*i)->pBaseBoneMatrix);
			
			for (long long j = 0; j < (*i)->llAniMaxTime / 10; ++j)
			{
				Safe_Delete((*i)->ppAniMatrix[j]);
				Safe_Delete((*i)->ppResultMatrix[j]);
			}

			Safe_Release((*i)->pBoneMatrixBuffer);
			Safe_Delete((*i)->pBoneMatrix);
		}
		m_vecAni.clear();

		if (pFBXScene != NULL)
			pFBXScene->Destroy();
	}
	else
	{
		for (auto i = m_vecAni.begin(); i != m_vecAni.end(); ++i)
		{
			Safe_Delete((*i)->pAniBuffer);
			Safe_Delete_Array((*i)->pBaseBoneMatrix);
		
			for (long long j = 0; j < (*i)->llAniMaxTime / 10; ++j)
			{
				Safe_Delete((*i)->ppAniMatrix[j]);
				Safe_Delete((*i)->ppResultMatrix[j]);
			}

			Safe_Release((*i)->pBoneMatrixBuffer);
			//Safe_Delete((*i)->pBoneMatrix);		// 이거 지우는순간 뻑난다...
		}
		m_vecAni.clear();
	}
}

void CMesh::MakeVertexNormal(BYTE * _pVertices, WORD * _pIndices)
{
	if (!_pIndices)
		SetNormalVectorByBasic(_pVertices);
	else
		SetNormalVectorByAverage(_pVertices, _pIndices, (m_dwIdxCnt / 3), 3, false);
}

void CMesh::SetNormalVectorByBasic(BYTE * _pVertices)
{
	D3DXVECTOR3		vNormal;
	VTXTEX*	pVertex = NULL;

	int nPrimitives = m_dwVtxCnt / 3;
	for (int i = 0; i < nPrimitives; ++i)
	{
		vNormal = GetTriAngleNormal(_pVertices, (i * 3 + 0), (i * 3 + 1), (i * 3 + 2));

		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 0) * m_nVtxSize));
		pVertex->vNormal = vNormal;
		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 1) * m_nVtxSize));
		pVertex->vNormal = vNormal;
		pVertex = (VTXTEX*)(_pVertices + ((i * 3 + 2) * m_nVtxSize));
		pVertex->vNormal = vNormal;
	}
}

void CMesh::SetNormalVectorByAverage(BYTE * _pVertices, WORD * _pIndices, int _iPrimitives, int _iOffset, bool _bStrip)
{
	D3DXVECTOR3		vNormal(0.f, 0.f, 0.f);
	VTXTEX*	pVertex = NULL;
	USHORT nIndex_0, nIndex_1, nIndex_2;

	for (int i = 0; i < (int)m_dwVtxCnt; ++i)
	{
		vNormal = D3DXVECTOR3(0.f, 0.f, 0.f);

		for (int j = 0; j < _iPrimitives; j++)
		{
			nIndex_0 = (_bStrip) ? (((j % 2) == 0) ? (j * _iOffset + 0) : (j * _iOffset + 1)) : (j * _iOffset + 0);
			if (_pIndices) nIndex_0 = _pIndices[nIndex_0];

			nIndex_1 = (_bStrip) ? (((j % 2) == 0) ? (j * _iOffset + 1) : (j * _iOffset + 0)) : (j * _iOffset + 1);
			if (_pIndices) nIndex_1 = _pIndices[nIndex_1];

			nIndex_2 = (_pIndices) ? _pIndices[j * _iOffset + 2] : (j * _iOffset + 2);

			if ((nIndex_0 == i) || (nIndex_1 == i) || (nIndex_2 == i))
				vNormal += GetTriAngleNormal(_pVertices, nIndex_0, nIndex_1, nIndex_2);
		}
		D3DXVec3Normalize(&vNormal, &vNormal);
		pVertex = (VTXTEX *)(_pVertices + (i * m_nVtxSize));
		pVertex->vNormal = vNormal;
	}
}

D3DXVECTOR3 CMesh::GetTriAngleNormal(BYTE * _pVertices, USHORT _nIndex_0, USHORT _nIndex_1, USHORT _nIndex_2)
{
	D3DXVECTOR3 vNormal(0.f, 0.f, 0.f);
	D3DXVECTOR3 vP_0 = *((D3DXVECTOR3 *)(_pVertices + (m_nVtxSize * _nIndex_0)));
	D3DXVECTOR3 vP_1 = *((D3DXVECTOR3 *)(_pVertices + (m_nVtxSize * _nIndex_1)));
	D3DXVECTOR3 vP_2 = *((D3DXVECTOR3 *)(_pVertices + (m_nVtxSize * _nIndex_2)));

	D3DXVECTOR3 vEdge_0 = vP_1 - vP_0;
	D3DXVECTOR3 vEdge_1 = vP_2 - vP_0;

	D3DXVec3Cross(&vNormal, &vEdge_0, &vEdge_1);
	D3DXVec3Normalize(&vNormal, &vNormal);

	return vNormal;
}

void CMesh::GetMinMax(D3DXVECTOR3 * const pMin, D3DXVECTOR3 * const pMax)
{
	*pMin = m_vMin;
	*pMax = m_vMax;
}
