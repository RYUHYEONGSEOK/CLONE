#include "stdafx.h"
#include "DefaultShader.h"

#include "VIBuffer.h"

CDefaultShader::CDefaultShader(CDevice * pDevice)
	: CShader(pDevice)
{
}

CDefaultShader::CDefaultShader(const CDefaultShader & rhs)
	: CShader(rhs.m_pDevice)
{
	m_pMatrixBuffer = rhs.m_pMatrixBuffer;
	m_pVS = rhs.m_pVS;
	m_pLayout = rhs.m_pLayout;
	m_pPS = rhs.m_pPS;
	m_pSampleState = rhs.m_pSampleState;
	m_pLightBuffer = rhs.m_pLightBuffer;
	m_pFloat4Buffer = rhs.m_pFloat4Buffer;
}

CDefaultShader::~CDefaultShader(void)
{

}

HRESULT CDefaultShader::Ready_ShaderFile(const TCHAR * pFilePath)
{
	HRESULT hr = NULL;

	D3D11_BUFFER_DESC matrixBufferDesc;

	//layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	ID3DBlob* pVSBlob = NULL;
	ID3D11Device* pDevice = m_pDevice->GetDevice();

	hr = CompileShaderFromFile(pFilePath, "VS", "vs_5_0", &pVSBlob);
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"VertexShader Init Failed");

	// 버텍스 쉐이더 생성
	hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVS);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	pDevice->CreateInputLayout(
		layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pLayout);

	ID3DBlob* pPSBlob = NULL;

	hr = CompileShaderFromFile(pFilePath, "PS", "ps_5_0", &pPSBlob);
	FAILED_CHECK_RETURN_MSG(hr, E_FAIL, L"Pixel Shader Init Failed");

	// 픽셀 쉐이더 만들기
	hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPS);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	Safe_Release(pPSBlob);
	Safe_Release(pVSBlob);

	// MatrixSet을 위한 DESC 생성
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 매트릭스 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (FAILED(hr))
		return hr;

	matrixBufferDesc.ByteWidth = sizeof(ConstantBufferFloat4);
	hr = m_pDevice->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_pFloat4Buffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CDefaultShader::BeginShader(const int & iPassIdx)
{
}

void CDefaultShader::EndShader(void)
{
	
}

HRESULT CDefaultShader::SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
	CBuffer* pBuffer, bool bCollision)
{
	ConstantBuffer			dataPtr;
	ConstantBufferFloat4	dataPtr2;

	XMMATRIX matWorld, matView, matProj;

	ID3D11DeviceContext* pDeviceContext = CDevice::GetInstance()->GetDeviceContext();

	// 셰이더로 보낼 변환 행렬 준비
	matWorld = XMLoadFloat4x4(&worldMatrix);
	matView = XMLoadFloat4x4(&viewMatrix);
	matProj = XMLoadFloat4x4(&projectionMatrix);

	matWorld = XMMatrixTranspose(matWorld);
	matView = XMMatrixTranspose(matView);
	matProj = XMMatrixTranspose(matProj);

	// 상수 버퍼에 값 복사
	dataPtr.mWorld = matWorld;
	dataPtr.mView = matView;
	dataPtr.mProjection = matProj;

	pDeviceContext->UpdateSubresource(pBuffer->m_ConstantBuffer, 0, NULL, &dataPtr, 0, 0);

	dataPtr2.mfloat4 = XMFLOAT4(float(bCollision), 0.f, 0.f, 0.f);

	pDeviceContext->UpdateSubresource(pBuffer->m_Float4Buffer, 0, NULL, &dataPtr2, 0, 0);

	pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer->m_ConstantBuffer);
	pDeviceContext->IASetInputLayout(m_pLayout);
	//////////////////
	pDeviceContext->PSSetConstantBuffers(1, 1, &pBuffer->m_Float4Buffer);		//PS에서 사용하므로 PS에 셋팅
	pDeviceContext->PSSetShader(m_pPS, NULL, 0);
	//pDeviceContext->PSSetShaderResources(0, 1, &pResource);
	//pDeviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	return S_OK;
}

CDefaultShader * CDefaultShader::Create(CDevice * pDevice, const TCHAR * pFilePath)
{
	CDefaultShader*		pShader = new CDefaultShader(pDevice);

	if (FAILED(pShader->Ready_ShaderFile(pFilePath)))
		Safe_Delete(pShader);

	return pShader;
}

void CDefaultShader::Release(void)
{
}

CResource * CDefaultShader::Clone(void)
{
	++(*m_pRefCnt);
	return new CDefaultShader(*this);
}

void CDefaultShader::Update(void)
{
}

