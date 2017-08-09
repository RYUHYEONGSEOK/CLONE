#include "stdafx.h"
#include "WaterShader.h"
//Object
#include "Camera.h"
#include "VIBuffer.h"

CWaterShader::CWaterShader(CDevice * pDevice)
	: CShader(pDevice)
	, m_pReflectionBuffer(NULL)
	, m_pWaterBuffer(NULL)
{


}
CWaterShader::CWaterShader(const CWaterShader & rhs)
	: CShader(rhs.m_pDevice)
{
	m_pMatrixBuffer = rhs.m_pMatrixBuffer;
	m_pVS = rhs.m_pVS;
	m_pLayout = rhs.m_pLayout;
	m_pPS = rhs.m_pPS;
	m_pSampleState = rhs.m_pSampleState;
	m_pLightBuffer = rhs.m_pLightBuffer;
	m_pReflectionBuffer = rhs.m_pReflectionBuffer;
	m_pWaterBuffer = rhs.m_pWaterBuffer;
}

CWaterShader::~CWaterShader(void)
{
	Safe_Release(m_pReflectionBuffer);
	Safe_Release(m_pWaterBuffer);
}

HRESULT CWaterShader::Ready_ShaderFile(const TCHAR * pFilePath)
{
	HRESULT hr = NULL;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC waterBufferDesc;
	D3D11_BUFFER_DESC ReflectionBufferDesc;
	ID3DBlob* pVSBlob = NULL;
	ID3D11Device* pDevice = m_pDevice->GetDevice();

	//layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;

	// 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&waterBufferDesc, NULL, &m_pWaterBuffer);
	if (FAILED(hr))
		return hr;

	// PixelMatrixBuffer
	ReflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ReflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	ReflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ReflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ReflectionBufferDesc.MiscFlags = 0;
	ReflectionBufferDesc.StructureByteStride = 0;

	// 매트릭스 버퍼 생성
	hr = m_pDevice->GetDevice()->CreateBuffer(&ReflectionBufferDesc, NULL, &m_pReflectionBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void CWaterShader::BeginShader(const int & iPassIdx)
{
}

void CWaterShader::EndShader(void)
{
}

HRESULT CWaterShader::SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
	D3DXMATRIX reflectionMatrix,
	CBuffer*	pBuffer,
	LightInfo * pLightInfo,
	ID3D11ShaderResourceView * pNormalTexture,
	ID3D11ShaderResourceView * reflectionTexture,
	ID3D11ShaderResourceView * refractionTexture,
	float waterTranslation, float reflectRefractScale)
{
	HRESULT hr = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer* dataPtr;
	ReflectionBufferType* dataPtr2;
	WaterBufferType* dataPtr3;
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
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 상수버퍼 락
	hr = pDeviceContext->Map(m_pReflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	dataPtr2 = (ReflectionBufferType*)mappedResource.pData;

	dataPtr2->reflectionMatrix = reflectionMatrix;

	// 상수버퍼 언락
	pDeviceContext->Unmap(m_pReflectionBuffer, 0);

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pReflectionBuffer);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 상수버퍼 락
	hr = pDeviceContext->Map(m_pWaterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;
	
	dataPtr3 = (WaterBufferType*)mappedResource.pData;

	//물 관련 변수 추가
	dataPtr3->waterTranslation = waterTranslation;
	dataPtr3->reflectRefractScale = reflectRefractScale;
	dataPtr3->padding = D3DXVECTOR2(float(g_GameInfo.bMapType), 0.0f);

	// 상수버퍼 언락
	pDeviceContext->Unmap(m_pWaterBuffer, 0);

	// 여기서 버텍스 셰이더를 업뎃한다.
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pWaterBuffer);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	pDeviceContext->IASetInputLayout(m_pLayout);
	//////////////////
	pDeviceContext->PSSetShader(m_pPS, NULL, 0);
	pDeviceContext->PSSetShaderResources(0, 1, &pNormalTexture);
	pDeviceContext->PSSetShaderResources(1, 1, &reflectionTexture);
	pDeviceContext->PSSetShaderResources(2, 1, &refractionTexture);
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	return S_OK;
}

CWaterShader * CWaterShader::Create(CDevice * pDevice, const TCHAR * pFilePath)
{
	CWaterShader*		pShader = new CWaterShader(pDevice);

	if (FAILED(pShader->Ready_ShaderFile(pFilePath)))
		Safe_Delete(pShader);

	return pShader;
}

void CWaterShader::Release(void)
{
	
}

CResource * CWaterShader::Clone(void)
{
	++(*m_pRefCnt);
	return new CWaterShader(*this);
}

void CWaterShader::Update(void)
{
}
