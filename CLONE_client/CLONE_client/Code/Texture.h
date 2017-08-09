#pragma once

#include "Resources.h"

class CTexture : public CResource
{
public:
	enum TextureType { TEXTURE_TYPE_BASIC, TEXTURE_TYPE_DDS };

	struct TEXTUREINFO
	{
		ID3D11ShaderResourceView* pTextures;
		ID3D11SamplerState*	pSamplerState;
	};

private:
	vector<TEXTUREINFO>	m_vecTexture;

public:
	static CResource*		Create(CDevice* pDevice,
		wstring wtrPath,
		const TextureType& eTextureType, 
		const WORD& wCnt = 0);
	virtual CResource*		Clone();

private:
	HRESULT		Load(wstring wtrPath, const TextureType& eTextureType, const WORD& wCnt);
	void		Load_Basic(wstring wstrPath, const WORD& wCnt);
	void		Load_DDS(wstring wstrPath, const WORD& wCnt);

public:
	ID3D11ShaderResourceView*	GetResource(const WORD& wCnt = 0) { return m_vecTexture[wCnt].pTextures; }
	ID3D11SamplerState*			GetSamplerState(const WORD& wCnt = 0) { return m_vecTexture[wCnt].pSamplerState; }

	void Set_ConstantTable(ID3DX11Effect* pEffect, const char* pConstantName, const DWORD& dwCnt = 0);

public:
	virtual void			Update();
	virtual void			Release();

public:
	void					Render(const WORD& wCnt);

protected:
	explicit CTexture(CDevice* pDevice);
	explicit CTexture(const CTexture& rhs);
public:
	virtual ~CTexture();
};