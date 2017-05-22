#pragma once
#include "include.h"

class CTimeMgr;
class CDevice
{
public:
	CDevice();
	virtual ~CDevice();

public:
	DECLARE_SINGLETON(CDevice)

public:
	ID3D11Device*					m_pDevice;
	IDXGISwapChain*					m_pSwapChain;
	ID3D11RenderTargetView*			m_pRenderTargetView;
	ID3D11DeviceContext*			m_pDeviceContext;
	D3D11_VIEWPORT					viewport;
	ID3D11Texture2D*				m_pDepthStencilBuffer;
	ID3D11DepthStencilView*			m_pDepthStencilView;
	//D3D11_VIEWPORT					viewport;

public:
	HRESULT CreateDevice(void);
	bool	CreateSwapChain(void);
	bool	CreateRenderTargetStanciView(void);

public:
	void	BeginDevice(void);
	void	EndDevice(void);
	void Release(void);
};



