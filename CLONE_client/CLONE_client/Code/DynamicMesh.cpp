#include "stdafx.h"
#include "DynamicMesh.h"

#include "TimeMgr.h"
#include "FbxParser.h"

#include "Obj.h"
#include "CubeColor.h"

CDynamicMesh::CDynamicMesh(CDevice * pDevice)
	: CMesh(pDevice)
	, m_bFindBone(false)
{
}

CDynamicMesh::CDynamicMesh(const CDynamicMesh & rhs)
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

	//Ani
	m_wCurrenAniIdx = rhs.m_wCurrenAniIdx;
	
	//여기서 Ani Vector 복사해서 넣는다...
	for (size_t i = 0; i < rhs.m_vecAni.size(); ++i)
	{
		Animation* prhsAni = new Animation();
		prhsAni->fAniPlaySpeed = rhs.m_vecAni[i]->fAniPlaySpeed;
		prhsAni->llAniMaxTime = rhs.m_vecAni[i]->llAniMaxTime;
		prhsAni->mapIndexByName = rhs.m_vecAni[i]->mapIndexByName;
		prhsAni->nAniNodeIdxCnt = rhs.m_vecAni[i]->nAniNodeIdxCnt;
		prhsAni->strAniName = rhs.m_vecAni[i]->strAniName;
		prhsAni->tMappedResource = rhs.m_vecAni[i]->tMappedResource;
		
		// 여기부터 깊은복사 --------------------------------------------------------------
		prhsAni->pAniBuffer = rhs.m_vecAni[i]->pAniBuffer->Clone();

		prhsAni->pBaseBoneMatrix = new XMMATRIX[BONE_MATRIX_NUM];
		memcpy(prhsAni->pBaseBoneMatrix, rhs.m_vecAni[i]->pBaseBoneMatrix, sizeof(XMMATRIX) * BONE_MATRIX_NUM);
		
		//상수 버퍼 깊은복사
		D3D11_BUFFER_DESC BD;
		::ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
		BD.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		BD.ByteWidth = sizeof(VS_CB_BONE_MATRIX);
		BD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BD.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		m_pDevice->GetDevice()->CreateBuffer(&BD, NULL, &prhsAni->pBoneMatrixBuffer);

		m_pDevice->GetDeviceContext()->Map(
			prhsAni->pBoneMatrixBuffer,
			NULL,
			D3D11_MAP::D3D11_MAP_WRITE_DISCARD,
			NULL,
			&prhsAni->tMappedResource);

		prhsAni->pBoneMatrix = (VS_CB_BONE_MATRIX *)prhsAni->tMappedResource.pData;

		for (int i = 0; i < BONE_MATRIX_NUM; i++)
			prhsAni->pBoneMatrix->m_XMmtxBone[i] = XMMatrixIdentity();

		m_pDevice->GetDeviceContext()->Unmap(
			prhsAni->pBoneMatrixBuffer,
			NULL);

		// Matrix 깊은복사
		prhsAni->ppAniMatrix = new XMMATRIX*[unsigned int(prhsAni->llAniMaxTime / 10)];
		prhsAni->ppResultMatrix = new XMMATRIX*[unsigned int(prhsAni->llAniMaxTime / 10)];

		for (long long j = 0; j < prhsAni->llAniMaxTime / 10; ++j)
		{
			prhsAni->ppAniMatrix[j] = new XMMATRIX[prhsAni->nAniNodeIdxCnt];
			prhsAni->ppResultMatrix[j] = new XMMATRIX[prhsAni->nAniNodeIdxCnt];
		}

		for (long long j = 0; j < prhsAni->llAniMaxTime / 10; ++j)
		{
			for (unsigned int z = 0; z < prhsAni->nAniNodeIdxCnt; ++z)
			{
				memcpy(&prhsAni->ppAniMatrix[j][z], &rhs.m_vecAni[i]->ppAniMatrix[j][z], sizeof(XMMATRIX));
				memcpy(&prhsAni->ppResultMatrix[j][z], &rhs.m_vecAni[i]->ppResultMatrix[j][z], sizeof(XMMATRIX));
			}
		}

		m_vecAni.push_back(prhsAni);
	}
	
	m_iRepeatTime = rhs.m_iRepeatTime;

	m_fAniPlayTimer = rhs.m_fAniPlayTimer;
	Animend = rhs.Animend;
	Yamae = rhs.Yamae;

	pFBXScene = rhs.pFBXScene;
}

CDynamicMesh::~CDynamicMesh()
{
	Release();
}

CResource * CDynamicMesh::Create(CDevice * pDevice, const char * pPath, vector<string> vecAniName)
{
	CDynamicMesh*	pMesh = new CDynamicMesh(pDevice);

	//DeviceContext 간섭 방지용
	g_bDraw = FALSE;

	if (FAILED(pMesh->Init(pPath, vecAniName)))
		Safe_Delete(pMesh);

	//DeviceContext 간섭 방지용
	g_bDraw = TRUE;

	return pMesh;
}

CResource * CDynamicMesh::Clone()
{
	++(*m_pRefCnt);
	return new CDynamicMesh(*this);
}

HRESULT CDynamicMesh::Load_Model(const char* pPath,
								vector<string> vecAniName,
								FbxManager* pFBXManager,
								FbxIOSettings* pIOsettings,
								FbxScene* pFBXScene,
								FbxImporter* pImporter)
{
	int		iFileFormat = EOF;
	string	strFullPath;
	string	strExtenstiom = ".FBX";

	FbxAxisSystem CurrAxisSystem;
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;
	FbxGeometryConverter lGeomConverter(pFBXManager);

	FbxNode*		pFbxRootNode;
	Animation*		pAni;

	for (unsigned int i = 0; i < vecAniName.size(); ++i)
	{
		// http://www.slideshare.net/quxn6/c4316-alignment-error
		pAni = new Animation();
		pAni->strAniName = vecAniName[i];

		strFullPath.clear();
		strFullPath = pPath;
		strFullPath += vecAniName[i];
		strFullPath += strExtenstiom;

		pFBXManager->GetIOPluginRegistry()->DetectReaderFileFormat(strFullPath.c_str(), iFileFormat);

		FAILED_CHECK_MSG(pImporter->Initialize(strFullPath.c_str(), iFileFormat), L"No Find Fbx Path");

		pImporter->Import(pFBXScene);

		CurrAxisSystem = pFBXScene->GetGlobalSettings().GetAxisSystem();
		if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(pFBXScene);

		lGeomConverter.Triangulate(pFBXScene, true);

		pFbxRootNode = pFBXScene->GetRootNode();
		NULL_CHECK_RETURN(pFbxRootNode, E_FAIL);

		//pAni->pBaseBoneMatrix = new XMMATRIX[BONE_MATRIX_NUM];
		pAni->pBaseBoneMatrix = (XMMATRIX*)_aligned_malloc(
			sizeof(XMMATRIX) * BONE_MATRIX_NUM, 16);
		//new (pAni->pBaseBoneMatrix)XMMATRIX();

		for (int i = 0; i < BONE_MATRIX_NUM; ++i)
			pAni->pBaseBoneMatrix[i] = XMMatrixIdentity();

		pAni->pAniBuffer = CAniBuffer::Create(CDevice::GetInstance());
		pAni->pAniBuffer->SetFbxBoneIndex(&(pAni->mapIndexByName), pFbxRootNode);


		CFbxParser::ParsingVertex(pFbxRootNode, pAni);


		// Animation -----------------------
		auto AnimStack = pFBXScene->GetSrcObject<FbxAnimStack>();
		NULL_CHECK_RETURN(AnimStack, E_FAIL);

		pAni->llAniMaxTime = AnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
		pAni->nAniNodeIdxCnt = pAni->mapIndexByName.size();

		pAni->ppAniMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];
		pAni->ppResultMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];

		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			//pAni->ppAniMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];
			//pAni->ppResultMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];

			pAni->ppAniMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
			pAni->ppResultMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
		}

		//Animation 상수 버퍼 생성
		{
			//최초 상수버퍼를 생성한다.
			D3D11_BUFFER_DESC BD;
			::ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
			BD.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			BD.ByteWidth = sizeof(VS_CB_BONE_MATRIX);
			BD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			BD.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

			m_pDevice->GetDevice()->CreateBuffer(&BD, NULL, &pAni->pBoneMatrixBuffer);

			m_pDevice->GetDeviceContext()->Map(
				pAni->pBoneMatrixBuffer,
				NULL,
				D3D11_MAP::D3D11_MAP_WRITE_DISCARD,
				NULL,
				&pAni->tMappedResource);

			pAni->pBoneMatrix = (VS_CB_BONE_MATRIX *)pAni->tMappedResource.pData;

			for (int i = 0; i < BONE_MATRIX_NUM; i++)
				pAni->pBoneMatrix->m_XMmtxBone[i] = XMMatrixIdentity();

			m_pDevice->GetDeviceContext()->Unmap(
				pAni->pBoneMatrixBuffer,
				NULL);
		}

		CFbxParser::ParsingAnimation(pFbxRootNode, pAni, AnimStack);

		//XMMATRIX ResultMtx;
		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			for (unsigned int j = 0; j < pAni->nAniNodeIdxCnt; ++j)
			{
				XMMATRIX ResultMtx(pAni->pBaseBoneMatrix[j] * (XMMATRIX)(pAni->ppAniMatrix[i][j]));
				pAni->ppResultMatrix[i][j] = ResultMtx;
			}
		}

		m_vecAni.push_back(pAni);
	}

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

HRESULT CDynamicMesh::Init(const char * pPath, vector<string> vecAniName)
{
	FbxManager*	pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");

	FAILED_CHECK_RETURN(Load_Model(pPath, vecAniName, pFBXManager, pIOsettings, pFBXScene, pImporter),
		E_FAIL);

	CreateRasterizerState();

	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();

	Animend = false;
	Yamae = false;

	HRESULT hr;

	D3D11_BUFFER_DESC cb;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(ConstantBuffer);
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	hr = CDevice::GetInstance()->GetDevice()->CreateBuffer(&cb, NULL, &m_ConstantBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;
	}

	return S_OK;
}

XMMATRIX ** CDynamicMesh::GetBoneMatrix(FbxNode* pNode, Animation* pAni, const string* pAniName, const char * pBoneName)
{
	if (pAni == NULL)
	{
		//우선 애니메이션을 찾는다
		for (auto i = m_vecAni.begin(); i != m_vecAni.end(); ++i)
		{
			if ((*i)->strAniName == *pAniName)
			{
				pAni = (*i);
				break;
			}
		}
		NULL_CHECK_MSG(pAni, L"Find Animation Failed");
	}

	if (pNode != nullptr)
	{
		if (!strcmp(pNode->GetName(), pBoneName))
		{
			unsigned int BoneIdx = pAni->mapIndexByName[pNode->GetName()];

			//여기 수정해야됨!
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
		CDynamicMesh::GetBoneMatrix(pNode->GetChild(i), pAni, pAniName, pBoneName);

	return NULL;	//찾지 못했다.
}

void CDynamicMesh::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_pDevice->GetDevice()->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);
}

void CDynamicMesh::Update()
{
}

void CDynamicMesh::Release()
{
	
}

void CDynamicMesh::Release_Animation()
{
}

void CDynamicMesh::PlayPlayerAnimation(int iIdx)
{
	if (iIdx < 0 || (unsigned)iIdx > m_vecAni.size())
		return;

	//파라미터로 들어온 애니와 현재 애니가 다르면 false로 바꿔준다...
	if (m_wCurrenAniIdx != iIdx)
	{
		Animend = false;
		m_fAniPlayTimer = 0.f;
	}

	//m_pShader->Render();
	m_pDevice->GetDeviceContext()->RSSetState(m_pRasterizerState);

	//if (m_vecAni[_iIdx]->fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10)
	//	m_vecAni[_iIdx]->fAniPlayTimer = 0;

	//애니메이션 스테이트 체크해서 어디서부터 반복할지 넣어준다
	switch (iIdx)
	{
	case CObj::OBJ_STATE_ATT:
		//m_vecAni[_iIdx]->fAniPlayTimer
		m_fAniPlayTimer += m_vecAni[iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			//m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			m_fAniPlayTimer = 0.f;
			Animend = true;
		}
		break;
	case CObj::OBJ_STATE_SIT:
		//m_vecAni[_iIdx]->fAniPlayTimer
		m_fAniPlayTimer += m_vecAni[iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			//m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			m_fAniPlayTimer = 100.f;
			Animend = true;
		}
		break;
	case CObj::OBJ_STATE_DIE:
		m_fAniPlayTimer += m_vecAni[iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			// 계속 누워있기위한 임의 수정
			m_fAniPlayTimer = 100.f;
			Animend = true;
		}
		break;
	default:
		//m_vecAni[_iIdx]->fAniPlayTimer
		m_fAniPlayTimer += m_vecAni[iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10) 
		{
			m_fAniPlayTimer = 0;
			Animend = true;
		}
		break;
	}

	for (unsigned int i = 0; i < m_vecAni[iIdx]->nAniNodeIdxCnt; ++i)
	{
		if ((iIdx == 0 && m_fAniPlayTimer == 133.f) ||
			(iIdx == 2 && m_fAniPlayTimer == 200.f) ||
			(iIdx == 4 && m_fAniPlayTimer == 333.f))
			continue;	// 터짐방지

		//if(m_vecAni[iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i] )
		m_vecAni[iIdx]->pBoneMatrix->m_XMmtxBone[i]
			//= m_vecAni[_iIdx]->ppResultMatrix[int(m_vecAni[_iIdx]->fAniPlayTimer)][i];
			= m_vecAni[iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[iIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_vecAni[iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[iIdx]->pAniBuffer->Render(DXGI_32);
	m_wCurrenAniIdx = iIdx;
}

void CDynamicMesh::PlayBotAnimation(int iIdx)
{
	if (iIdx < 0 || (unsigned)iIdx > m_vecAni.size())
		return;

	//파라미터로 들어온 애니와 현재 애니가 다르면 false로 바꿔준다...
	if (m_wCurrenAniIdx != iIdx)
	{
		Animend = false;
		m_fAniPlayTimer = 0.f;
	}
		
	if (Animend)
		Animend = false;

	//m_pShader->Render();
	m_pDevice->GetDeviceContext()->RSSetState(m_pRasterizerState);

	//m_vecAni[_iIdx]->fAniPlayTimer
	m_fAniPlayTimer += m_vecAni[iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//if (m_vecAni[_iIdx]->fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10)
	//	m_vecAni[_iIdx]->fAniPlayTimer = 0;

	//애니메이션 스테이트 체크해서 어디서부터 반복할지 넣어준다
	switch (iIdx)
	{
	case CObj::OBJ_STATE_MOVE:
		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			//m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			m_fAniPlayTimer = 0.f;
			Animend = true;
		}
		break;
	case CObj::OBJ_STATE_IDEL:
		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			//m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			m_fAniPlayTimer = 0.f;
			Animend = true;
		}
		break;
	case CObj::OBJ_STATE_DIE:
		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			// 계속 누워있기위한 임의 수정
			m_fAniPlayTimer = 100.f;
			Animend = true;
		}
		break;
	default:
		if (m_fAniPlayTimer > m_vecAni[iIdx]->llAniMaxTime / 10)
		{
			m_fAniPlayTimer = 0;
			Animend = true;
		}
		break;
	}

	// 터지는 부분
	// iIdx = 0 , m_fAniPlayTime = 133.f , i = 0
	// iIdx = 2 , m_fAniPlayTime = 200.f , i = 0
	// iIdx = 4 , m_fAniPlayTime = 333.f , i = 0
	for (unsigned int i = 0; i < m_vecAni[iIdx]->nAniNodeIdxCnt; ++i)
	{
		if ((iIdx == 0 && m_fAniPlayTimer == 133.f) ||
			(iIdx == 2 && m_fAniPlayTimer == 200.f) ||
			(iIdx == 4 && m_fAniPlayTimer == 333.f))
			continue;	// 터짐방지

		m_vecAni[iIdx]->pBoneMatrix->m_XMmtxBone[i]
			//= m_vecAni[_iIdx]->ppResultMatrix[int(m_vecAni[_iIdx]->fAniPlayTimer)][i];
			= m_vecAni[iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[iIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_vecAni[iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[iIdx]->pAniBuffer->Render(DXGI_32);
	m_wCurrenAniIdx = iIdx;
}

void CDynamicMesh::RenderMesh(int iIdx)
{
	//버그 방지용
	if (m_wCurrenAniIdx > 10)
		return;

	for (unsigned int i = 0; i < m_vecAni[m_wCurrenAniIdx]->nAniNodeIdxCnt; ++i)
	{
		if ((iIdx == 0 && m_fAniPlayTimer == 133.f) ||
			(iIdx == 2 && m_fAniPlayTimer == 200.f) ||
			(iIdx == 4 && m_fAniPlayTimer == 333.f))
			continue;	// 터짐방지

		m_vecAni[m_wCurrenAniIdx]->pBoneMatrix->m_XMmtxBone[i]
			//= m_vecAni[_iIdx]->ppResultMatrix[int(m_vecAni[_iIdx]->fAniPlayTimer)][i];
			= m_vecAni[m_wCurrenAniIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}

	if (m_vecAni[m_wCurrenAniIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_vecAni[m_wCurrenAniIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[m_wCurrenAniIdx]->pAniBuffer->Render(DXGI_32);
}

void CDynamicMesh::BWPlayAnim(int _iIdx)
{
	//애니메이션 스테이트 체크해서 어디서부터 반복할지 넣어준다
	switch (_iIdx)
	{
	/*case CPlayingInfo::ANI_STATE_BOOSTER:
		m_iRepeatTime = 120;
		break;
	case CPlayingInfo::ANI_STATE_DEAD:
		m_iRepeatTime = 100;
		break;
	case CPlayingInfo::ANI_STATE_BREAK:
		m_iRepeatTime = 50;
		break;*/
	case CObj::OBJ_STATE_IDEL:
		m_iRepeatTime = 0;
		break;
	default:
		m_iRepeatTime = 0;
		break;
	}

	if (_iIdx < 0 || (unsigned)_iIdx > m_vecAni.size())
		return;


	//m_pShader->Render();
	m_pDevice->GetDeviceContext()->RSSetState(m_pRasterizerState);

	//반대로 돌린다아ㅏ
	m_fAniPlayTimer
		-= m_vecAni[_iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//AllocConsole();
	//_cprintf("%lf\n", m_fAniPlayTimer);

	if (m_fAniPlayTimer <= m_iRepeatTime) {
		Animend = false;
		//return true;
	}

	//-----------------------------------------------------

	for (unsigned int i = 0; i < m_vecAni[_iIdx]->nAniNodeIdxCnt; ++i)
	{
		if ((_iIdx == 0 && m_fAniPlayTimer == 133.f) ||
			(_iIdx == 2 && m_fAniPlayTimer == 200.f) ||
			(_iIdx == 4 && m_fAniPlayTimer == 333.f))
			continue;	// 터짐방지

		m_vecAni[_iIdx]->pBoneMatrix->m_XMmtxBone[i]
			= m_vecAni[_iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[_iIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pDevice->GetDeviceContext()->VSSetConstantBuffers(
			VS_SLOT_BONE_MATRIX, 1, &m_vecAni[_iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[_iIdx]->pAniBuffer->Render(DXGI_32);
	m_wCurrenAniIdx = _iIdx;
	//return false;
}