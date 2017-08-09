#include "stdafx.h"
#include "RefactionShader.h"
//Object
#include "Camera.h"
#include "VIBuffer.h"

CRefactionShader::CRefactionShader(CDevice * pDevice)
	: CShader(pDevice)
	, m_pClipPlaneBuffer(NULL)
{


}
CRefactionShader::CRefactionShader(const CRefactionShader & rhs)
	: CShader(rhs.m_pDevice)
{
	m_pMatrixBuffer = rhs.m_pMatrixBuffer;
	m_pVS = rhs.m_pVS;
	m_pLayout = rhs.m_pLayout;
	m_pPS = rhs.m_pPS;
	m_pSampleState = rhs.m_pSampleState;
	m_pLightBuffer = rhs.m_pLightBuffer;
	m_pClipPlaneBuffer = rhs.m_pClipPlaneBuffer;
}

CRefactionShader::~CRefactionShader(void)
{
	Safe_Release(m_pClipPlaneBuffer);
}

HRESULT CRefactionShader::Ready_ShaderFile(const TCHAR * pFilePath)
{
	HRESULT hr = NULL;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC clipPlaneBufferDesc;
	ID3DBlob* pVSBlob = NULL;
	ID3D11Device* pDevice = m_pDevice->GetDevice();

	//layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	// clipPlaneBuffer
	clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
	clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	clipPlaneBufferDesc.MiscFlags = 0;
	clipPlaneBufferDesc.StructureByteStride = 0;

	// 매트릭스 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_pClipPlaneBuffer);
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


	return S_OK;
}

void CRefactionShader::BeginShader(const int & iPassIdx)
{
}

void CRefactionShader::EndShader(void)
{
}

HRESULT CRefactionShader::SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
	CBuffer*	pBuffer,
	LightInfo * pLightInfo,
	ID3D11ShaderResourceView * pResource,
	D3DXVECTOR4 vClipPlane)
{
	HRESULT hr = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer* dataPtr;
	ClipPlaneBufferType* dataPtr2;
	LightBufferType* dataPtr3;
	XMMATRIX matWorld, matView, matProj;

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

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 상수버퍼 락
	hr = pDeviceContext->Map(m_pClipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// 상수버퍼 포인터 가져온다...
	dataPtr2 = (ClipPlaneBufferType*)mappedResource.pData;

	dataPtr2->clipPlane = vClipPlane;

	// 상수버퍼 언락
	pDeviceContext->Unmap(m_pClipPlaneBuffer, 0);

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pClipPlaneBuffer);
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 상수버퍼 락
	hr = pDeviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// 상수버퍼 포인터 가져온다...
	dataPtr3 = (LightBufferType*)mappedResource.pData;

	dataPtr3->ambientColor = pLightInfo->ambientColor;
	dataPtr3->diffuseColor = pLightInfo->diffuseColor;
	dataPtr3->lightDirection = pLightInfo->direction;
	dataPtr3->specularColor = pLightInfo->specular;
	dataPtr3->lightPos = pLightInfo->pos;

	// 상수버퍼 언락
	pDeviceContext->Unmap(m_pLightBuffer, 0);

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	pDeviceContext->IASetInputLayout(m_pLayout);
	//////////////////
	pDeviceContext->PSSetShader(m_pPS, NULL, 0);
	pDeviceContext->PSSetShaderResources(0, 1, &pResource);
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	return S_OK;
}

CRefactionShader * CRefactionShader::Create(CDevice * pDevice, const TCHAR * pFilePath)
{
	CRefactionShader*		pShader = new CRefactionShader(pDevice);

	if (FAILED(pShader->Ready_ShaderFile(pFilePath)))
		Safe_Delete(pShader);

	return pShader;
}

void CRefactionShader::Release(void)
{

}

CResource * CRefactionShader::Clone(void)
{
	++(*m_pRefCnt);
	return new CRefactionShader(*this);
}

void CRefactionShader::Update(void)
{
}
