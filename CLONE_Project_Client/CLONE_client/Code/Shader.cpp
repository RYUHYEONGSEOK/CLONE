#include "stdafx.h"
#include "Shader.h"

CShader::CShader(CDevice* pDevice)
	: CResource(pDevice)
	, m_pVS(NULL)
	, m_pLayout(NULL)
	, m_pPS(NULL)
	, m_pMatrixBuffer(NULL)
	, m_pSampleState(NULL)
	, m_pLightBuffer(NULL)
	, m_pFloat4Buffer(NULL)
{

}

CShader::CShader(const CShader & rhs)
	: CResource(rhs.m_pDevice)
{

}

CShader::~CShader()
{
	Release();
}

HRESULT CShader::CompileShaderFromFile(const TCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob)
			pErrorBlob->Release();

		return hr;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	return S_OK;
}

void CShader::Render()
{
	ID3D11DeviceContext* pDeviceContext = m_pDevice->GetDeviceContext();

	if (m_pLayout)
		pDeviceContext->IASetInputLayout(m_pLayout);
	if (m_pVS)
		pDeviceContext->VSSetShader(m_pVS, NULL, 0);
	if (m_pPS)
		pDeviceContext->PSSetShader(m_pPS, NULL, 0);
}

void CShader::Release()
{
	if ((*m_pRefCnt) == 0)
	{
		Safe_Release(m_pVS);
		Safe_Release(m_pLayout);
		Safe_Release(m_pPS);
		Safe_Release(m_pSampleState);
		Safe_Release(m_pLightBuffer);
		Safe_Release(m_pLightBuffer);
		Safe_Release(m_pFloat4Buffer);
		CResource::Release_RefCnt();
	}
	else 
		--(*m_pRefCnt);
}
