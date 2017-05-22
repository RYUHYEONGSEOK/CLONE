#include "stdafx.h"
#include "GraphicDevice.h"

IMPLEMENT_SINGLETON(CDevice)

const float CDevice::COLOR_BACKBUFFER[4] = { 0.2f, 0.2f, 0.6f, 1.f };

CDevice::CDevice(void)
	: m_pDevice(NULL)
	, m_pDeviceContext(NULL)
	, m_pDeferredContext(NULL)

	, m_pSwapChain(NULL)
	, m_pRenderTargetView(NULL)
	, m_pDepthStencilBuffer(NULL)
	, m_pDepthStencilView(NULL)

	, m_pDepthDisabledStencilState(NULL)
	, m_pDepthStencilState(NULL)

	, m_pDXGIFactory(NULL)
{
	ZeroMemory(m_pszGraphicBrandName, sizeof(WCHAR) * MINPATH);
}

CDevice::~CDevice(void)
{
	Release();
}

HRESULT CDevice::Init(WINMODE _eWinMode)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// 스왑체인 디스크 생성
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
#ifdef _WINDOWED
	sd.Windowed = TRUE;
#else
	sd.Windowed = FALSE;
#endif

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, &m_featureLevel, &m_pDeviceContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	//Deferred Context 생성
	hr = m_pDevice->CreateDeferredContext(0, &m_pDeferredContext);
	if (FAILED(hr))
		return hr;

	// 스왑체인 백버퍼 셋팅
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	// 랜더타겟뷰 생성
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Depth-Stencil버퍼 생성
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width					= width;
	descDepth.Height				= height;
	descDepth.MipLevels				= 1;
	descDepth.ArraySize				= 1;
	descDepth.Format				= DXGI_FORMAT_R32_TYPELESS /*DXGI_FORMAT_D24_UNORM_S8_UINT*/;
	descDepth.SampleDesc.Count		= 1;
	descDepth.SampleDesc.Quality	= 0;
	descDepth.Usage					= D3D11_USAGE_DEFAULT;
	descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags		= 0;
	descDepth.MiscFlags				= 0;
	hr = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencilBuffer);
	if (FAILED(hr))
		return hr;

	// Depth-Stencil 디스크 생성
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format					= DXGI_FORMAT_D32_FLOAT /*descDepth.Format*/;
	descDSV.ViewDimension			= D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice		= 0;
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	// 디바이스 콘텍스트에 적재
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// Depth-Stencil 상태디스크
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Depth-Stencil 상태 생성
	hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(hr))
		return false;

	// Depth-Stencil 상태 세팅
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	// Depth-Stencil Disable 상태디스크
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 상태 생성
	hr = m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_pDepthDisabledStencilState);
	if (FAILED(hr))
		return false;

	// 뷰포트 셋팅
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &vp);

	FAILED_CHECK(Create_DXGI());
	FAILED_CHECK(Init_Blend());

	return S_OK;
}

void CDevice::Render_Begin()
{
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, COLOR_BACKBUFFER);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void CDevice::Render_End()
{
	FAILED(m_pSwapChain->Present(0, 0));
}

void CDevice::Blend_Begin()
{
	m_pDeviceContext->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);
}

void CDevice::Blend_End()
{
	m_pDeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
}

void CDevice::SetBackBufferRenderTarget(void)
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

void CDevice::TurnZBufferOn(void)
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
}

void CDevice::TurnZBufferOff(void)
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthDisabledStencilState, 1);
}

HRESULT CDevice::Init_Blend()
{
	D3D11_BLEND_DESC tBlend;
	ZeroMemory(&tBlend, sizeof(D3D11_BLEND_DESC));

	tBlend.AlphaToCoverageEnable = TRUE;
	tBlend.IndependentBlendEnable = FALSE;
	tBlend.RenderTarget[0].BlendEnable = TRUE;
	tBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	tBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	FAILED_CHECK_RETURN(m_pDevice->CreateBlendState(&tBlend, &m_pBlendState), E_FAIL);

	return S_OK;
}

HRESULT CDevice::Create_DXGI(void)
{
	HRESULT hr = NULL;

	IDXGIAdapter		*pAdapter		= NULL;

	DXGI_ADAPTER_DESC   adapterDesc;
	ZeroMemory(&adapterDesc, sizeof(DXGI_ADAPTER_DESC));

	// Create DXGI Factory
	if (FAILED(hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&m_pDXGIFactory))) 
		return(false);

	// Select Default Graphic Drive
	if (FAILED(m_pDXGIFactory->EnumAdapters(0, (IDXGIAdapter**)&pAdapter)))
		return false;

	pAdapter->GetDesc(&adapterDesc);

	m_ui64VideoMemory = (unsigned __int64)(adapterDesc.DedicatedVideoMemory + adapterDesc.SharedSystemMemory) / 1048576;
	memcpy(m_pszGraphicBrandName, adapterDesc.Description, sizeof(WCHAR) * MINPATH);

	return S_OK;
}

void CDevice::Release()
{
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pDepthStencilBuffer);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pBlendState);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDeferredContext);

	Safe_Release(m_pDXGIFactory);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
