#include "stdafx.h"
#include "PointLightShader.h"
//Object
#include "Camera.h"
#include "VIBuffer.h"

CPointLightShader::CPointLightShader(CDevice * pDevice)
	: CShader(pDevice)
	, m_pPixelMatrixBuffer(NULL)
{

}

CPointLightShader::CPointLightShader(const CPointLightShader & rhs)
	: CShader(rhs.m_pDevice)
{
	m_pMatrixBuffer = rhs.m_pMatrixBuffer;
	m_pVS = rhs.m_pVS;
	m_pLayout = rhs.m_pLayout;
	m_pPS = rhs.m_pPS;
	m_pSampleState = rhs.m_pSampleState;
	m_pLightBuffer = rhs.m_pLightBuffer;
	m_pPixelMatrixBuffer = rhs.m_pPixelMatrixBuffer;
}

CPointLightShader::~CPointLightShader(void)
{
	Safe_Release(m_pPixelMatrixBuffer);
}

HRESULT CPointLightShader::Ready_ShaderFile(const TCHAR * pFilePath)
{
	HRESULT hr = NULL;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC pixelMatrixBufferDesc;
	ID3DBlob* pVSBlob = NULL;
	ID3D11Device* pDevice = m_pDevice->GetDevice();

	//layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

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

	// Texture Sampler DESC 생성
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pDevice->GetDevice()->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
		return hr;

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

	// LightBuffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&lightBufferDesc, NULL, &m_pLightBuffer);
	if (FAILED(hr))
		return hr;

	// PixelMatrixBuffer
	pixelMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelMatrixBufferDesc.ByteWidth = sizeof(PixelMatrixBufferType);
	pixelMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelMatrixBufferDesc.MiscFlags = 0;
	pixelMatrixBufferDesc.StructureByteStride = 0;

	// 매트릭스 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&pixelMatrixBufferDesc, NULL, &m_pPixelMatrixBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CPointLightShader::BeginShader(const int & iPassIdx)
{
}

void CPointLightShader::EndShader(void)
{
}

HRESULT CPointLightShader::SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
	XMFLOAT4X4 matViewInvMatrix, XMFLOAT4X4 matProjInvMatrix,
	LightInfo* pLightInfo,
	ID3D11ShaderResourceView** ppResource,
	const WORD& wCntResource,
	CObj* pCam)
{
	HRESULT hr = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	ConstantBuffer* dataPtr;
	LightBufferType* dataPtr2;
	PixelMatrixBufferType* dataPtr3;
	XMMATRIX matWorld, matView, matProj, XMmatViewInv, XMmatProjInv, XMmatVPInv;
	D3DXMATRIX matViewInv, matProjInv;

	ID3D11DeviceContext* pDeviceContext = CDevice::GetInstance()->GetDeviceContext();

	//-------------------------------------------------------------------------------------------------
	// 셰이더로 보낼 변환 행렬 준비
	matWorld = XMLoadFloat4x4(&worldMatrix);
	matView = XMLoadFloat4x4(&viewMatrix);
	matProj = XMLoadFloat4x4(&projectionMatrix);

	matWorld = XMMatrixTranspose(matWorld);
	matView = XMMatrixTranspose(matView);
	matProj = XMMatrixTranspose(matProj);

	// 상수버퍼 락
	hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// 상수버퍼 포인터 가져온다...
	dataPtr = (ConstantBuffer*)mappedResource.pData;

	// 상수 버퍼에 값 복사
	dataPtr->mWorld = matWorld;
	dataPtr->mView = matView;
	dataPtr->mProjection = matProj;

	// 상수버퍼 언락
	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	// 버텍스 셰이더 안의 상수버퍼의 포지션 셋팅 // 초기값 == 0
	bufferNumber = 0;

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	//-------------------------------------------------------------------------------------------------

	// LightBuffer 락
	hr = pDeviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// 상수버퍼 포인터 얻어온다...
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// 라이트값 복사
	dataPtr2->ambientColor = pLightInfo->ambientColor;
	dataPtr2->diffuseColor = pLightInfo->diffuseColor;
	dataPtr2->lightDirection = pLightInfo->direction;
	dataPtr2->specularColor = pLightInfo->specular;
	dataPtr2->lightPos = pLightInfo->pos;

	// 라이트 버퍼 언락
	pDeviceContext->Unmap(m_pLightBuffer, 0);

	// 픽셀 쉐이더안의 버퍼 포지션 셋팅
	bufferNumber = 1;

	// 픽셀 쉐이더 업뎃
	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pLightBuffer);

	//-------------------------------------------------------------------------------------------------

	XMmatViewInv = XMLoadFloat4x4(&matViewInvMatrix);
	XMmatProjInv = XMLoadFloat4x4(&matProjInvMatrix);

	XMmatViewInv = XMMatrixTranspose(XMmatViewInv);
	XMmatProjInv = XMMatrixTranspose(XMmatProjInv);

	// Pixel에서 쓸 Matrix 락
	hr = pDeviceContext->Map(m_pPixelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// 상수버퍼 포인터 얻어온다...
	dataPtr3 = (PixelMatrixBufferType*)mappedResource.pData;

	// 값 복사
	D3DXVECTOR3 vCamPos = *((CCamera*)pCam)->GetEye();
	dataPtr3->CamPos	= D3DXVECTOR4(vCamPos.x, vCamPos.y, vCamPos.z, 1.f);
	dataPtr3->ProjInv	= XMmatProjInv;
	dataPtr3->ViewInv	= XMmatViewInv;
	dataPtr3->VPInv		= XMMatrixIdentity();
	dataPtr3->Range		= pLightInfo->range;

	// 라이트 버퍼 언락
	pDeviceContext->Unmap(m_pPixelMatrixBuffer, 0);

	// 픽셀 쉐이더안의 버퍼 포지션 셋팅
	bufferNumber = 2;

	// 픽셀 쉐이더 업뎃
	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pPixelMatrixBuffer);

	//-------------------------------------------------------------------------------------------------

	pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	pDeviceContext->IASetInputLayout(m_pLayout);
	//////////////////
	pDeviceContext->PSSetShader(m_pPS, NULL, 0);
	pDeviceContext->PSSetShaderResources(0, (UINT)wCntResource, &(*ppResource));
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	return S_OK;
}

CPointLightShader * CPointLightShader::Create(CDevice * pDevice, const TCHAR * pFilePath)
{
	CPointLightShader*		pShader = new CPointLightShader(pDevice);

	if (FAILED(pShader->Ready_ShaderFile(pFilePath)))
		Safe_Delete(pShader);

	return pShader;
}

void CPointLightShader::Release(void)
{
}

CResource * CPointLightShader::Clone(void)
{
	++(*m_pRefCnt);
	return new CPointLightShader(*this);
}

void CPointLightShader::Update(void)
{
}
