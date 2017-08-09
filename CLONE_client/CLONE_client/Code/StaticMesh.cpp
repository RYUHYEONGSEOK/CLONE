#include "stdafx.h"
#include "StaticMesh.h"
#include "CubeColor.h"

CStaticMesh::CStaticMesh(CDevice * pDevice)
	: CMesh(pDevice)
{

}

CStaticMesh::CStaticMesh(const CStaticMesh & rhs)
	: CMesh(rhs.m_pDevice)
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

	m_pBoundingBox = rhs.m_pBoundingBox;

	m_vMax = rhs.m_vMax;
	m_vMin = rhs.m_vMin;

	m_pRefCnt = rhs.m_pRefCnt;
}

CStaticMesh::~CStaticMesh()
{
	Release();
}

CResource * CStaticMesh::Create(CDevice * pDevice, const char * pPath)
{
	CStaticMesh* pMesh = new CStaticMesh(pDevice);

	if (FAILED(pMesh->Init(pPath)))
		Safe_Delete(pMesh);

	return pMesh;
}

CResource * CStaticMesh::Clone()
{
	++(*m_pRefCnt);
	return new CStaticMesh(*this);
}

HRESULT CStaticMesh::Load_Model(const char* pPath,
	FbxManager* pFBXManager, FbxIOSettings* pSetting, FbxScene* pFBXScene, FbxImporter* pImporter)
{
	if (!(pImporter->Initialize(pPath, -1, pFBXManager->GetIOSettings())))
		FAILED_CHECK_MSG(E_FAIL, L"Model Init Failed");

	if (!(pImporter->Import(pFBXScene)))
		FAILED_CHECK_MSG(E_FAIL, L"Model Import Failed");

	FbxGeometryConverter clsConverter(pFBXManager);
	clsConverter.Triangulate(pFBXScene, false);
	FbxNode* pRootNode = pFBXScene->GetRootNode();

	if (!pRootNode)
		return E_FAIL;


	vector<VertexTexture*> vecVertexTexture;

	for (int i = 0; i < pRootNode->GetChildCount(); ++i)
	{
		FbxNode* pChildNode = pRootNode->GetChild(i);

		if (pChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType
			= pChildNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType != FbxNodeAttribute::eMesh)
			continue;


		FbxMesh* pMesh = (FbxMesh*)pChildNode->GetNodeAttribute();
		D3DXVECTOR3 m_pos;
		D3DXVECTOR2 outUV;
		D3DXVECTOR3 outNormal;
		FbxVector4* mControlPoints = pMesh->GetControlPoints();
		int vertexCounter = 0;

		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			int iNumVertices = pMesh->GetPolygonSize(j);
			assert(iNumVertices == 3);
			FbxGeometryElementUV* vertexUV = pMesh->GetElementUV(0);
			FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal(0);

			for (int k = 0; k < iNumVertices; k++)
			{
				// 정점데이터 얻어오기
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
				int inTextureUVIndex = pMesh->GetTextureUVIndex(j, k);
				++vertexCounter;

				m_pos.x = (float)mControlPoints[iControlPointIndex].mData[0];
				m_pos.y = (float)mControlPoints[iControlPointIndex].mData[1];
				m_pos.z = (float)mControlPoints[iControlPointIndex].mData[2];


				// uv얻기
				switch (vertexUV->GetMappingMode())
				{

				case FbxGeometryElement::eByControlPoint:

					switch (vertexUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						outUV.x = static_cast<float>(vertexUV->GetDirectArray()
							.GetAt(iControlPointIndex).mData[0]);
						outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray()
							.GetAt(iControlPointIndex).mData[1]);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						int index = vertexUV->GetIndexArray().GetAt(iControlPointIndex);
						outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
						outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
					switch (vertexUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
						outUV.x = static_cast<float>(vertexUV->GetDirectArray()
							.GetAt(inTextureUVIndex).mData[0]);
						outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray()
							.GetAt(inTextureUVIndex).mData[1]);
						break;

					default:
						throw std::exception("Invalid Reference");
					}
					break;

				default:
					throw std::exception("Invalid Reference");
					break;
				}

				VertexTexture*  m_Ver = new VertexTexture(
					D3DXVECTOR3(m_pos.x, m_pos.z, -m_pos.y),
					D3DXVECTOR3(outNormal.x, outNormal.z, -outNormal.y),
					D3DXVECTOR2(outUV.x, outUV.y));
				vecVertexTexture.push_back(m_Ver);
			}
		}
	}

	unsigned int n = vecVertexTexture.size();
	VertexTexture* pVertexTexture = new VertexTexture[n];
	for (unsigned int i = 0; i < vecVertexTexture.size(); ++i)
	{
		// vecVertexTexture.at(i) 는 catch 문을 사용하지 않기 때문에 지웠습니다
		pVertexTexture[i].vPos = vecVertexTexture[i]->vPos;
		pVertexTexture[i].vNormal = vecVertexTexture[i]->vNormal;
		pVertexTexture[i].vTextureUV = vecVertexTexture[i]->vTextureUV;
	}

	m_dwVtxCnt = vecVertexTexture.size();
	m_nVtxSize = sizeof(VertexTexture);
	m_nVtxOffset = 0;

	MakeVertexNormal((BYTE*)pVertexTexture, NULL);

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = m_nVtxSize * m_dwVtxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = pVertexTexture;
	m_pDevice->GetDevice()->CreateBuffer(&tBufferDesc, &tData, &m_pVtxBuffer);

	//Bounding Box Calc
	fbxsdk::FbxVector4 vMin, vMax, vCenter;
	pFBXScene->ComputeBoundingBoxMinMaxCenter(vMin, vMax, vCenter);	//바운딩 박스 계산

	m_vMin.x = static_cast<float>(vMin.mData[0]);
	m_vMin.y = static_cast<float>(vMin.mData[1]);
	m_vMin.z = static_cast<float>(vMin.mData[2]);
	
	m_vMax.x = static_cast<float>(vMax.mData[0]);
	m_vMax.y = static_cast<float>(vMax.mData[1]);
	m_vMax.z = static_cast<float>(vMax.mData[2]);

	m_pBoundingBox = CCubeColor::Create(m_pDevice, m_vMin, m_vMax, D3DXVECTOR4(1.f, 1.f, 0.f, 1.f));
	NULL_CHECK_RETURN(m_pBoundingBox, NULL);

	return S_OK;
}

HRESULT CStaticMesh::Init(const char * pPath)
{
	FbxManager*	pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	FbxScene* pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");
	
	FAILED_CHECK_RETURN(Load_Model(pPath, pFBXManager, pIOsettings, pFBXScene, pImporter), E_FAIL);
	
	CreateRasterizerState();

	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();

	return S_OK;
}

void CStaticMesh::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

void CStaticMesh::Update()
{
}

void CStaticMesh::Release()
{

}

