#pragma once

class CDevice
{
	DECLARE_SINGLETON(CDevice)
public:
	enum WINMODE { WIN_MODE_FULL, WIN_MODE_WIN, WIN_MODE_END };

public:
	static const float COLOR_BACKBUFFER[4];

private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11DeviceContext*	m_pDeferredContext;

	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11Texture2D*			m_pDepthStencilBuffer;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilState*	m_pDepthDisabledStencilState;
	ID3D11BlendState*			m_pBlendState;

	//DXGI Factory
	IDXGIFactory*				m_pDXGIFactory;

	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0;

private:
	unsigned int	m_ui64VideoMemory;
	WCHAR			m_pszGraphicBrandName[MINPATH];
	
public:
	HRESULT		Init(WINMODE _eWinMode = WIN_MODE_WIN);
	void		Release();

	void		Render_Begin();
	void		Render_End();

	void		Blend_Begin();
	void		Blend_End();

private:
	HRESULT		Init_Blend(void);
	HRESULT		Create_DXGI(void);

public:
	ID3D11Device*			GetDevice() { return m_pDevice; }
	ID3D11DeviceContext*	GetDeviceContext() { return m_pDeviceContext; }
	ID3D11DeviceContext*	GetDeferredDeviceContext() { return m_pDeferredContext; }

	IDXGISwapChain*			GetSwapChain() { return m_pSwapChain; }
	ID3D11RenderTargetView*	GetRenderTargetView() { return m_pRenderTargetView; }
	ID3D11Texture2D*		GetSDepthStencilBuffer() { return m_pDepthStencilBuffer; }
	ID3D11DepthStencilView*	GetDepthStencilView() { return m_pDepthStencilView; }
	ID3D11BlendState*		GetBlendState() { return m_pBlendState; }

	TCHAR*					GetGraphicBrandName(void) { return m_pszGraphicBrandName; }
	unsigned int			GetGraphicMemory(void) { return m_ui64VideoMemory; }

public:
	void					SetBackBufferRenderTarget(void);

	void					TurnZBufferOn(void);
	void					TurnZBufferOff(void);
		
private:
	explicit CDevice(void);
public:
	~CDevice(void);
};