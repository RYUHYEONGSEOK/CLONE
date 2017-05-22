#include "stdafx.h"
#include "Texture.h"

CTexture::CTexture(CDevice* pDevice)
	: CResource(pDevice)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CResource(rhs.m_pDevice)
{
	//±Ì¿∫∫πªÁ
	m_vecTexture = rhs.m_vecTexture;

	m_pRefCnt = rhs.m_pRefCnt;
}

CTexture::~CTexture()
{
	Release();
}

CResource* CTexture::Create(CDevice* pDevice,
	wstring wstrPath,
	const TextureType& eTextureType,
	const WORD& wCnt /*=0*/)
{
	CResource* pResource = new CTexture(pDevice);
	if (FAILED(dynamic_cast<CTexture*>(pResource)->Load(wstrPath, eTextureType, wCnt)))
		Safe_Delete(pResource);

	return pResource;
}

CResource* CTexture::Clone()
{
	++(*m_pRefCnt);
	return new CTexture(*this);
}

HRESULT CTexture::Load(wstring wstrPath, const TextureType& eTextureType, const WORD& wCnt)
{
	switch (eTextureType)
	{
	case TEXTURE_TYPE_BASIC:	Load_Basic(wstrPath, wCnt);	break;
	case TEXTURE_TYPE_DDS:		Load_DDS(wstrPath, wCnt);	break;

	default:	return E_FAIL;
	}

	return S_OK;
}

void CTexture::Load_Basic(wstring wstrPath, const WORD& wCnt)
{
	TEXTUREINFO tInfo;

	TCHAR		szFullPath[MAX_PATH] = L"";

	for (size_t i = 0; i <= wCnt; ++i)
	{
		wsprintf(szFullPath, wstrPath.c_str(), i);

		D3DX11CreateShaderResourceViewFromFile(
			m_pDevice->GetDevice(), szFullPath, NULL, NULL, &tInfo.pTextures, NULL);

		ID3D11SamplerState* pSamplerState = NULL;
		D3D11_SAMPLER_DESC tData;

		ZeroMemory(&tData, sizeof(D3D11_SAMPLER_DESC));
		tData.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		tData.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		tData.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		tData.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		tData.ComparisonFunc = D3D11_COMPARISON_NEVER;
		tData.MinLOD = 0;
		tData.MaxLOD = 0;
		m_pDevice->GetDevice()->CreateSamplerState(&tData, &tInfo.pSamplerState);

		m_vecTexture.push_back(tInfo);
	}
}

void CTexture::Load_DDS(wstring wstrPath, const WORD& wCnt)
{
	TEXTUREINFO tInfo;

	TCHAR		szFullPath[MAX_PATH] = L"";

	for (size_t i = 0; i <= wCnt; ++i)
	{
		wsprintf(szFullPath, wstrPath.c_str(), i);

		D3DX11_IMAGE_LOAD_INFO tLoadInfo;
		tLoadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		ID3D11Texture2D* pTexture2D = NULL;

		D3DX11CreateTextureFromFile(
			m_pDevice->GetDevice(), szFullPath,
			&tLoadInfo, NULL, (ID3D11Resource**)&pTexture2D, NULL);

		D3D11_TEXTURE2D_DESC tTextureData;
		pTexture2D->GetDesc(&tTextureData);

		D3D11_SHADER_RESOURCE_VIEW_DESC tViewData;
		tViewData.Format = tTextureData.Format;
		tViewData.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		tViewData.TextureCube.MipLevels = tTextureData.MipLevels;
		tViewData.TextureCube.MostDetailedMip = 0;

		m_pDevice->GetDevice()->
			CreateShaderResourceView(pTexture2D, &tViewData, &tInfo.pTextures);

		ID3D11SamplerState* pSamplerState = NULL;
		D3D11_SAMPLER_DESC tSamplerData;

		ZeroMemory(&tSamplerData, sizeof(D3D11_SAMPLER_DESC));
		tSamplerData.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		tSamplerData.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerData.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerData.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		tSamplerData.ComparisonFunc = D3D11_COMPARISON_NEVER;
		tSamplerData.MinLOD = 0;
		tSamplerData.MaxLOD = D3D11_FLOAT32_MAX;
		m_pDevice->GetDevice()->CreateSamplerState(&tSamplerData, &tInfo.pSamplerState);

		m_vecTexture.push_back(tInfo);
	}
}

void CTexture::Update()
{

}

void CTexture::Render(const WORD& wCnt)
{
	m_pDevice->GetDeviceContext()->PSSetShaderResources(PS_SLOT_TEXTURE, 1, &m_vecTexture[wCnt].pTextures);
	m_pDevice->GetDeviceContext()->PSSetSamplers(PS_SLOT_SAMPLER_STATE, 1, &m_vecTexture[wCnt].pSamplerState);
}

void CTexture::Release()
{
	if ((*m_pRefCnt) == 0)
	{
		for (auto i = m_vecTexture.begin(); i != m_vecTexture.end(); ++i)
		{
			Safe_Release((*i).pTextures);
			Safe_Release((*i).pSamplerState);
		}
		CResource::Release_RefCnt();
	}
	else 
		--(*m_pRefCnt);
}
