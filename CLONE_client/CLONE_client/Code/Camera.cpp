#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera(CDevice* pDevice)
	: CObj(pDevice)
	, m_vEye(0.f, 0.f, 0.f)
	, m_vAt(0.f, 0.f, 0.f)
	, m_vUp(0.f, 1.f, 0.f)
	, m_fFovY(0.f)
	, m_fAspect(0.f)
	, m_fNear(0.f)
	, m_fFar(0.f)
	, m_dwNaviIdx(0)

	, m_pViewBuffer(NULL)
	, m_pProjBuffer(NULL)

	, m_pDeviceContext(pDevice->GetDeviceContext())
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matOrtho);

	Init_ViewBuffer();
	Init_ProjBuffer();
	Init_Viewprot();
}

CCamera::~CCamera(void)
{
	Release();
}

HRESULT CCamera::Init()
{
	return S_OK;
}

int CCamera::Update(void)
{
	Invalidate_View();

	return 0;
}

void CCamera::Render()
{
}

void CCamera::Release()
{
	Safe_Release(m_pViewBuffer);
	Safe_Release(m_pProjBuffer);
}

void CCamera::RenderForShadow(LightMatrixStruct tLight)
{
}

void CCamera::RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType)
{
}

void CCamera::Init_ViewBuffer(void)
{
	D3D11_BUFFER_DESC tBuffer;
	ZeroMemory(&tBuffer, sizeof(D3D11_BUFFER_DESC));

	tBuffer.Usage = D3D11_USAGE_DYNAMIC;
	tBuffer.ByteWidth = sizeof(Buffer_View);
	tBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	FAILED_CHECK_RETURN(
		m_pDevice->GetDevice()->CreateBuffer(&tBuffer, NULL, &m_pViewBuffer), );
}

void CCamera::Init_ProjBuffer(void)
{
	D3D11_BUFFER_DESC tBuffer;
	ZeroMemory(&tBuffer, sizeof(D3D11_BUFFER_DESC));

	tBuffer.Usage = D3D11_USAGE_DYNAMIC;
	tBuffer.ByteWidth = sizeof(Buffer_Proj);
	tBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	FAILED_CHECK_RETURN(
		m_pDevice->GetDevice()->CreateBuffer(&tBuffer, NULL, &m_pProjBuffer), );
}

void CCamera::Init_Viewprot(void)
{
	m_tViewport.TopLeftX = m_tViewport.TopLeftY = 0.f;
	m_tViewport.Width = (float)WINCX;
	m_tViewport.Height = (float)WINCY;
	m_tViewport.MinDepth = 0.f;
	m_tViewport.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &m_tViewport);

	D3DXMatrixOrthoLH(&m_matOrtho, (float)WINCX, (float)WINCY, 0.f, 1.f);
}

void CCamera::Invalidate_View(void)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

	D3D11_MAPPED_SUBRESOURCE tSubreResource;
	m_pDeviceContext->Map(m_pViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubreResource);

	Buffer_View* pMatView = (Buffer_View*)tSubreResource.pData;
	D3DXMatrixTranspose(&pMatView->m_matView, &m_matView);

	m_pDeviceContext->Unmap(m_pViewBuffer, 0);
	m_pDeviceContext->VSSetConstantBuffers(VS_SLOT_VIEW_MATRIX, 1, &m_pViewBuffer);
}

void CCamera::Invalidate_Proj(void)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);

	D3D11_MAPPED_SUBRESOURCE tSubreResource;
	m_pDeviceContext->Map(m_pProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubreResource);

	Buffer_Proj* pMatProj = (Buffer_Proj*)tSubreResource.pData;
	D3DXMatrixTranspose(&pMatProj->m_matProj, &m_matProj);

	m_pDeviceContext->Unmap(m_pProjBuffer, 0);
	m_pDeviceContext->VSSetConstantBuffers(VS_SLOT_PROJECTION_MATRIX, 1, &m_pProjBuffer);
}

void CCamera::Invalidate_Ortho(void)
{
	D3D11_MAPPED_SUBRESOURCE tSubreResource;
	m_pDeviceContext->Map(m_pProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubreResource);

	Buffer_Proj* pMatProj = (Buffer_Proj*)tSubreResource.pData;
	D3DXMatrixTranspose(&pMatProj->m_matProj, &m_matOrtho);

	m_pDeviceContext->Unmap(m_pProjBuffer, 0);
	m_pDeviceContext->VSSetConstantBuffers(VS_SLOT_PROJECTION_MATRIX, 1, &m_pProjBuffer);
}

D3DXMATRIX * CCamera::GetReflectViewMatrix(float fHeight)
{
	D3DXVECTOR3 position;

	// up벡터
	D3DXVECTOR3 up = D3DXVECTOR3(0.f, 1.f, 0.f);

	// Y가 거꾸로 뒤집어진 카메라 포지션
	position.x = m_vEye.x;
	position.y = -m_vEye.y + (fHeight * 2.0f);
	position.z = m_vEye.z;

	// 뷰매트릭스 생성
	D3DXMatrixLookAtLH(&m_reflectionView, &position, &m_vAt, &up);

	return &m_reflectionView;
}

float CCamera::GetDistEyeToAt(void)
{
	return D3DXVec3Length(&(m_vAt - m_vEye));
}


