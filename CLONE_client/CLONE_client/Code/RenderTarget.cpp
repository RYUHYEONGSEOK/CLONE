#include "stdafx.h"
#include "RenderTarget.h"
//Manager
#include "ResourceMgr.h"
#include "InputMgr.h"
#include "GraphicDevice.h"
#include "LightMgr.h"
//Object
#include "RenderTex.h"
#include "TextureShader.h"
#include "BlendShader.h"
#include "Camera.h"
#include "Obj.h"

CRenderTarget::CRenderTarget(CDevice* pDevice)
	: m_pDevice(pDevice)
	, m_pShader(NULL)
	, m_pBlendShader(NULL)
	, m_pCam(NULL)
	, m_fX(0.f)
	, m_fY(0.f)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)

	, m_pAlbedoTexture(NULL)
	, m_pAlbedoResourceView(NULL)
	, m_pAlbedoTargetView(NULL)

	, m_pNormalTexture(NULL)
	, m_pNormalResourceView(NULL)
	, m_pNormalTargetView(NULL)

	, m_pDepthTexture(NULL)
	, m_pDepthResourceView(NULL)
	, m_pDepthTargetView(NULL)

	, m_pLightTexture(NULL)
	, m_pLightResourceView(NULL)
	, m_pLightTargetView(NULL)

	, m_pSpecTexture(NULL)
	, m_pSpecResourceView(NULL)
	, m_pSpecTargetView(NULL)

	, m_pShadowTexture(NULL)
	, m_pShadowResourceView(NULL)
	, m_pShadowTargetView(NULL)

	, m_pRefractionTexture(NULL)
	, m_pRefractionResourceView(NULL)
	, m_pRefractionTargetView(NULL)

	, m_pReflectionTexture(NULL)
	, m_pReflectionResourceView(NULL)
	, m_pReflectionTargetView(NULL)
{
	D3DXMatrixIdentity(&m_matRTView);
	D3DXMatrixIdentity(&m_matRTProj);

	m_pShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
		CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
		CResourceMgr::RESOURCE_TYPE_SHADER,
		L"Shader_Texture"));
}

CRenderTarget::~CRenderTarget(void)
{
	Release();
}

HRESULT CRenderTarget::Init_RenderTarget(CObj* pCam, const UINT& iSizeX, const UINT& iSizeY
	, DXGI_FORMAT Format, D3DXCOLOR Color)
{
	//SetCam
	m_pCam = pCam;

	//Texture To Render Target
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = iSizeX;
	desc.Height = iSizeY;
	desc.MipLevels = 1;							//밉맵 레벨
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;					//멀티 샘플 수
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pAlbedoTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pNormalTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pDepthTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pLightTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pSpecTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pShadowTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pReflectionTexture);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pRefractionTexture);
	FAILED_CHECK(hr);

	//TargetRenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pAlbedoTexture, &rtDesc, &m_pAlbedoTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pNormalTexture, &rtDesc, &m_pNormalTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pDepthTexture, &rtDesc, &m_pDepthTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pLightTexture, &rtDesc, &m_pLightTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pSpecTexture, &rtDesc, &m_pSpecTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pShadowTexture, &rtDesc, &m_pShadowTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pReflectionTexture, &rtDesc, &m_pReflectionTargetView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pRefractionTexture, &rtDesc, &m_pRefractionTargetView);
	FAILED_CHECK(hr);

	//Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = desc.Format;                                                  //텍스처 포맷 설정과 같게
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pAlbedoTexture, &SRVDesc, &m_pAlbedoResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pNormalTexture, &SRVDesc, &m_pNormalResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pDepthTexture, &SRVDesc, &m_pDepthResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pLightTexture, &SRVDesc, &m_pLightResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pSpecTexture, &SRVDesc, &m_pSpecResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pShadowTexture, &SRVDesc, &m_pShadowResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pReflectionTexture, &SRVDesc, &m_pReflectionResourceView);
	FAILED_CHECK(hr);
	hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pRefractionTexture, &SRVDesc, &m_pRefractionResourceView);
	FAILED_CHECK(hr);


	//// 이건 좀 더 연구해보장....
	////Shadow는 품질을 올려서 따로 만들어준다
	//ZeroMemory(&desc, sizeof(desc));
	//desc.Width =  WINCX * 4;
	//desc.Height = WINCY * 4;
	//desc.MipLevels = 1;							//밉맵 레벨
	//desc.ArraySize = 1;
	//desc.SampleDesc.Count = 1;					//멀티 샘플 수
	//desc.SampleDesc.Quality = 0;
	//desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;
	//hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, NULL, &m_pShadowTexture);
	//FAILED_CHECK(hr);

	//rtDesc.Format = desc.Format;
	//rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//rtDesc.Texture2D.MipSlice = 0;
	//hr = m_pDevice->GetDevice()->CreateRenderTargetView(m_pShadowTexture, &rtDesc, &m_pShadowTargetView);
	//FAILED_CHECK(hr);

	//SRVDesc.Format = desc.Format;                //텍스처 포맷 설정과 같게
	//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//SRVDesc.Texture2D.MostDetailedMip = 0;
	//SRVDesc.Texture2D.MipLevels = 1;
	//hr = m_pDevice->GetDevice()->CreateShaderResourceView(m_pShadowTexture, &SRVDesc, &m_pShadowResourceView);
	//FAILED_CHECK(hr);

	m_Color = Color;

	return S_OK;
}

HRESULT CRenderTarget::Create_DebugBuffer(const float& fSizeX, const float& fSizeY)
{
	for (int i = 0; i < RENDERTARGET_NUM; ++i)
	{
		m_pTargetBuffer[i] = CRenderTex::Create(m_pDevice);
		NULL_CHECK_RETURN(m_pTargetBuffer[i], E_FAIL);
	}

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(CDevice* pDevice, CObj* pCam
	, const UINT& iSizeX, const UINT& iSizeY
	, DXGI_FORMAT Format, D3DXCOLOR Color)
{
	CRenderTarget*	pRenderTarget = new CRenderTarget(pDevice);

	if (FAILED(pRenderTarget->Init_RenderTarget(pCam, iSizeX, iSizeY, Format, Color)))
	{
		Safe_Delete(pRenderTarget);
	}

	return pRenderTarget;
}

void CRenderTarget::Release(void)
{
	Safe_Release(m_pAlbedoTexture);
	Safe_Release(m_pAlbedoResourceView);
	Safe_Release(m_pAlbedoTargetView);

	Safe_Release(m_pNormalTexture);
	Safe_Release(m_pNormalResourceView);
	Safe_Release(m_pNormalTargetView);

	Safe_Release(m_pDepthTexture);
	Safe_Release(m_pDepthResourceView);
	Safe_Release(m_pDepthTargetView);

	Safe_Release(m_pLightTexture);
	Safe_Release(m_pLightResourceView);
	Safe_Release(m_pLightTargetView);

	Safe_Release(m_pSpecTexture);
	Safe_Release(m_pSpecResourceView);
	Safe_Release(m_pSpecTargetView);

	Safe_Release(m_pShadowTexture);
	Safe_Release(m_pShadowResourceView);
	Safe_Release(m_pShadowTargetView);

	Safe_Release(m_pRefractionTexture);
	Safe_Release(m_pRefractionResourceView);
	Safe_Release(m_pRefractionTargetView);

	Safe_Release(m_pReflectionTexture);
	Safe_Release(m_pReflectionResourceView);
	Safe_Release(m_pReflectionTargetView);

	for (int i = 0; i < RENDERTARGET_NUM; ++i)
		Safe_Delete(m_pTargetBuffer[i]);

	Safe_Release(m_pBlendShader);
}

void CRenderTarget::Clear_Target(void)
{
	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pAlbedoTargetView, m_Color);
	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pNormalTargetView, m_Color);
	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pDepthTargetView, m_Color);
	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pLightTargetView, m_Color);
	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pSpecTargetView, m_Color);
}

void CRenderTarget::Draw_RenderTarget(void)
{
	//이건 나중에 줄일 수 있다면 줄이자
	D3DXMatrixOrthoLH(&m_matRTProj, WINCX, WINCY, 0.f, 1.f);
	m_matRTView._11 = m_fSizeX;
	m_matRTView._22 = m_fSizeY;
	m_matRTView._33 = 1.f;

	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matView;
	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	memcpy(&matProj, m_matRTProj, sizeof(float) * 16);

	//위치를 바꾸며 5번을 그린다
	//Albedo
	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matRTView._41 = WINCX - 900.f - (WINCX >> 1);
	m_matRTView._42 = -(WINCY - 50.f) + (WINCY >> 1);
	memcpy(&matView, m_matRTView, sizeof(float) * 16);
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, m_pTargetBuffer[0], NULL, m_pAlbedoResourceView);
	m_pTargetBuffer[0]->Render(DXGI_16);

	//Normal
	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matRTView._41 = WINCX - 700.f - (WINCX >> 1);
	m_matRTView._42 = -(WINCY - 50.f) + (WINCY >> 1);
	memcpy(&matView, m_matRTView, sizeof(float) * 16);
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, m_pTargetBuffer[1], NULL, m_pNormalResourceView);
	m_pTargetBuffer[1]->Render(DXGI_16);

	//Depth
	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matRTView._41 = WINCX - 500.f - (WINCX >> 1);
	m_matRTView._42 = -(WINCY - 50.f) + (WINCY >> 1);
	memcpy(&matView, m_matRTView, sizeof(float) * 16);
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, m_pTargetBuffer[2], NULL, m_pDepthResourceView);
	m_pTargetBuffer[2]->Render(DXGI_16);

	//Light
	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matRTView._41 = WINCX - 300.f - (WINCX >> 1);
	m_matRTView._42 = -(WINCY - 50.f) + (WINCY >> 1);
	memcpy(&matView, m_matRTView, sizeof(float) * 16);
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, m_pTargetBuffer[3], NULL, m_pLightResourceView);
	m_pTargetBuffer[3]->Render(DXGI_16);

	//Spec
	// x : -0.5 -> -WINCX / 2.f	,	0.5 -> WINCX / 2.f
	m_matRTView._41 = WINCX - 100.f - (WINCX >> 1);
	m_matRTView._42 = -(WINCY - 50.f) + (WINCY >> 1);
	memcpy(&matView, m_matRTView, sizeof(float) * 16);
	((CTextureShader*)m_pShader)->SetShaderParameter(matWorld, matView, matProj, m_pTargetBuffer[4], NULL, m_pShadowResourceView/*m_pSpecResourceView*/);
	m_pTargetBuffer[4]->Render(DXGI_16);

	if (CInput::GetInstance()->GetDIKeyState(DIK_0) & 0x80)
	{
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pAlbedoTexture, D3DX11_IFF_JPG, L"g_AlbedoTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pNormalTexture, D3DX11_IFF_JPG, L"g_NormalTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pDepthTexture, D3DX11_IFF_JPG, L"g_DepthTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pLightTexture, D3DX11_IFF_JPG, L"g_LightTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pSpecTexture, D3DX11_IFF_JPG, L"g_SpecTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pReflectionTexture, D3DX11_IFF_JPG, L"g_ReflectionTexture.jpg");
		D3DX11SaveTextureToFile(m_pDevice->GetDeviceContext(), m_pRefractionTexture, D3DX11_IFF_JPG, L"g_RefactionTexture.jpg");
	}
}

void CRenderTarget::Draw_LightTarget(void)
{
	XMFLOAT4X4 matWorld, matView, matProj, matViewInv, matProjInv;
	D3DXMATRIX dxmatViewInv, dxmatProjInv;
	XMMATRIX XMmatProj, XMmatView;
	D3DXMatrixIdentity(&dxmatProjInv);
	D3DXMatrixIdentity(&dxmatViewInv);

	//World
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	//Proj
	XMmatProj = XMMatrixIdentity();
	XMmatProj = XMMatrixOrthographicLH(WINCX, WINCY, 0.f, 1.f);
	XMStoreFloat4x4(&matProj, XMmatProj);

	//View
	XMmatView = XMMatrixIdentity();
	XMmatView._11 = WINCX / 2.f;
	XMmatView._22 = WINCY / 2.f;
	XMmatView._33 = 1.f;

	XMmatView._41 = 0.f;
	XMmatView._42 = 0.f;
	XMStoreFloat4x4(&matView, XMmatView);

	//ViewInv
	memcpy(&matViewInv, D3DXMatrixInverse(&dxmatViewInv, NULL, ((CCamera*)m_pCam)->GetViewMatrix()), sizeof(float) * 16);
	//ProjInv
	memcpy(&matProjInv, D3DXMatrixInverse(&dxmatProjInv, NULL, ((CCamera*)m_pCam)->GetProjMatrix()), sizeof(float) * 16);

	CLightMgr::GetInstance()->Render_Light(matWorld, matView, matProj, matViewInv, matProjInv,
		m_pNormalResourceView, m_pDepthResourceView, m_pShadowResourceView);
}

void CRenderTarget::Set_Target_Deferred(void)
{
	ID3D11RenderTargetView* pTargetView[RENDERTARGET_DEFERRED_NUM] =
	{
		m_pAlbedoTargetView , m_pNormalTargetView , m_pDepthTargetView
	};

	m_pDevice->GetDeviceContext()->OMSetRenderTargets(RENDERTARGET_DEFERRED_NUM, &(*pTargetView), m_pDevice->GetDepthStencilView());
}

void CRenderTarget::Set_Target_Light(void)
{
	ID3D11RenderTargetView* pTargetView[RENDERTARGET_LIGHT_NUM] =
	{
		m_pLightTargetView , m_pSpecTargetView
	};

	m_pDevice->GetDeviceContext()->OMSetRenderTargets(RENDERTARGET_LIGHT_NUM, &(*pTargetView), m_pDevice->GetDepthStencilView());
}

void CRenderTarget::Set_Target_Shadow(void)
{
	m_pDevice->GetDeviceContext()->OMSetRenderTargets(1, &m_pShadowTargetView, m_pDevice->GetDepthStencilView());

	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pShadowTargetView, m_Color);
	m_pDevice->GetDeviceContext()->ClearDepthStencilView(m_pDevice->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CRenderTarget::Set_Target_Refraction(void)
{
	m_pDevice->GetDeviceContext()->OMSetRenderTargets(1, &m_pRefractionTargetView, m_pDevice->GetDepthStencilView());

	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pRefractionTargetView, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	m_pDevice->GetDeviceContext()->ClearDepthStencilView(m_pDevice->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CRenderTarget::Set_Target_Reflaction(void)
{
	m_pDevice->GetDeviceContext()->OMSetRenderTargets(1, &m_pReflectionTargetView, m_pDevice->GetDepthStencilView());

	m_pDevice->GetDeviceContext()->ClearRenderTargetView(m_pReflectionTargetView, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	m_pDevice->GetDeviceContext()->ClearDepthStencilView(m_pDevice->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CRenderTarget::Relese_Target(void)
{
	m_pDevice->GetDeviceContext()->ClearDepthStencilView(m_pDevice->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_pDevice->SetBackBufferRenderTarget();
}

void CRenderTarget::Set_ConstantTable(CBuffer*	pBuffer)
{
	if (m_pBlendShader == NULL)
	{
		m_pBlendShader = dynamic_cast<CShader*>(CResourceMgr::GetInstance()->CloneResource(
			CResourceMgr::RESOURCE_ATTRI_DYNAMIC,
			CResourceMgr::RESOURCE_TYPE_SHADER,
			L"Shader_Blend"));
		NULL_CHECK_MSG(m_pBlendShader, L"Get Shader Failed");
	}

	XMFLOAT4X4 matWorld, matView, matProj;
	XMMATRIX XMmatProj, XMmatView;

	//World
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());

	//Proj
	XMmatProj = XMMatrixIdentity();
	XMmatProj = XMMatrixOrthographicLH(WINCX, WINCY, 0.f, 1.f);
	XMStoreFloat4x4(&matProj, XMmatProj);

	//View
	XMmatView = XMMatrixIdentity();
	XMmatView._11 = WINCX / 2.f;
	XMmatView._22 = WINCY / 2.f;
	XMmatView._33 = 1.f;

	XMmatView._41 = 0.f;
	XMmatView._42 = 0.f;
	XMStoreFloat4x4(&matView, XMmatView);

	ID3D11ShaderResourceView* pResourceView[SHADERRESOURCEVIEW_NUM] =
	{
		m_pAlbedoResourceView, m_pLightResourceView, m_pSpecResourceView
	};

	((CBlendShader*)m_pBlendShader)->SetShaderParameter(matWorld, matView, matProj,
		pBuffer, NULL, &(*pResourceView), SHADERRESOURCEVIEW_NUM);
}

