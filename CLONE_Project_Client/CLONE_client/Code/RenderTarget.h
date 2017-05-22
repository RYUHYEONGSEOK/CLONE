#pragma once

class CBuffer;
class CDevice;
class CShader;
class CObj;
class CShader;
class CRenderTarget
{
public:
	HRESULT Init_RenderTarget(CObj* pCam, const UINT& iSizeX, const UINT& iSizeY
		, DXGI_FORMAT Format, D3DXCOLOR Color);
	HRESULT Create_DebugBuffer(const float& fSizeX, const float& fSizeY);
	void Clear_Target(void);
	void Set_Target_Deferred(void);
	void Set_Target_Light(void);
	void Set_Target_Shadow(void);
	void Set_Target_Refraction(void);
	void Relese_Target(void);
	void Draw_RenderTarget(void);
	void Draw_LightTarget(void);

	void Set_ConstantTable(CBuffer*	pBuffer);
	void Set_Cam(CObj* pCam) { m_pCam = pCam; }

public:
	static CRenderTarget* Create(CDevice* pDevice, CObj* pCam
		, const UINT& iSizeX, const UINT& iSizeY
		, DXGI_FORMAT Format, D3DXCOLOR Color);
	void Release(void);

private:
	CDevice*					m_pDevice;

	CShader*					m_pBlendShader;

	//Albedo
	ID3D11Texture2D*			m_pAlbedoTexture;
	ID3D11ShaderResourceView*	m_pAlbedoResourceView;
	ID3D11RenderTargetView*		m_pAlbedoTargetView;
	//Normal
	ID3D11Texture2D*			m_pNormalTexture;
	ID3D11ShaderResourceView*	m_pNormalResourceView;
	ID3D11RenderTargetView*		m_pNormalTargetView;
	//Depth
	ID3D11Texture2D*			m_pDepthTexture;
	ID3D11ShaderResourceView*	m_pDepthResourceView;
	ID3D11RenderTargetView*		m_pDepthTargetView;
	//Light
	ID3D11Texture2D*			m_pLightTexture;
	ID3D11ShaderResourceView*	m_pLightResourceView;
	ID3D11RenderTargetView*		m_pLightTargetView;
	//Specular
	ID3D11Texture2D*			m_pSpecTexture;
	ID3D11ShaderResourceView*	m_pSpecResourceView;
	ID3D11RenderTargetView*		m_pSpecTargetView;

	//Depth - Shadow
	ID3D11Texture2D*			m_pShadowTexture;
	ID3D11ShaderResourceView*	m_pShadowResourceView;
	ID3D11RenderTargetView*		m_pShadowTargetView;
	ID3D11Texture2D*			m_pShadowStencilBuffer;
	ID3D11DepthStencilView*		m_pShadowStencilView;

	//Refraction
	ID3D11Texture2D*			m_pRefractionTexture;
	ID3D11ShaderResourceView*	m_pRefractionResourceView;
	ID3D11RenderTargetView*		m_pRefractionTargetView;
	ID3D11Texture2D*			m_pRefractionDepthStencilBuffer;
	ID3D11DepthStencilView*		m_pRefractionDepthStencilView;
	//Reflection
	ID3D11Texture2D*			m_pReflectionTexture;
	ID3D11ShaderResourceView*	m_pReflectionResourceView;
	ID3D11RenderTargetView*		m_pReflectionTargetView;
	ID3D11Texture2D*			m_pReflectionDepthStencilBuffer;
	ID3D11DepthStencilView*		m_pReflectionDepthStencilView;

	ID3D11Resource* pResource;

private:
	CShader*		m_pShader;
	CObj*			m_pCam;

private:
	CBuffer*		m_pTargetBuffer[5];
	float			m_fX, m_fY, m_fSizeX, m_fSizeY;
	D3DXMATRIX		m_matRTProj, m_matRTView;
	
	// 물을 위한 클리핑 플레인
	D3DXVECTOR4		m_vClipPlane; 

private:
	D3DXCOLOR		m_Color;

private:
	explicit CRenderTarget(CDevice* pDevice);
public:
	~CRenderTarget(void);
};