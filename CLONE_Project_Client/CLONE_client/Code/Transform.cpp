#include "stdafx.h"
#include "Transform.h"

#include "GraphicDevice.h"

CTransform::CTransform(const D3DXVECTOR3& vLook)
	: m_vScale(1.f, 1.f, 1.f)
	, m_vPos(0.f, 0.f, 0.f)
	, m_vDir(vLook)
	, m_pWorldBuffer(NULL)
{
	ZeroMemory(m_fAngle, sizeof(float) * ANGLE_END);
	D3DXMatrixIdentity(&m_matWorld);

	Init_WorldBuffer();
}

CTransform::~CTransform(void)
{
	Release();
}

void CTransform::Update(void)
{
	D3DXMATRIX				matScale, matRotX, matRotY, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationX(&matRotX, m_fAngle[ANGLE_X]);
	D3DXMatrixRotationY(&matRotY, m_fAngle[ANGLE_Y]);
	D3DXMatrixRotationZ(&matRotZ, m_fAngle[ANGLE_Z]);
	D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
}

CTransform* CTransform::Create(const D3DXVECTOR3& vLook)
{
	return new CTransform(vLook);
}

void CTransform::Release(void)
{
}

void CTransform::Init_WorldBuffer(void)
{
	D3D11_BUFFER_DESC tBuffer;
	ZeroMemory(&tBuffer, sizeof(D3D11_BUFFER_DESC));
	tBuffer.Usage = D3D11_USAGE_DYNAMIC;
	tBuffer.ByteWidth = sizeof(Buffer_World);
	tBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	FAILED_CHECK_RETURN(CDevice::GetInstance()->GetDevice()->CreateBuffer(&tBuffer, NULL, &m_pWorldBuffer), );
}

void CTransform::Render_Reday(void)
{
	Update();

	D3D11_MAPPED_SUBRESOURCE tSubreResource;
	ID3D11DeviceContext* pDeviceContext = CDevice::GetInstance()->GetDeviceContext();
	pDeviceContext->Map(m_pWorldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSubreResource);

	Buffer_World* pMatWorld = (Buffer_World*)tSubreResource.pData;
	D3DXMatrixTranspose(&pMatWorld->m_matWorld, &m_matWorld);

	pDeviceContext->Unmap(m_pWorldBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pWorldBuffer);
}
