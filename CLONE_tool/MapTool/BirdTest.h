#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CBirdTest :
	public CObj
{
public:
	CBirdTest();
	virtual ~CBirdTest();

public:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;
	

public:
	virtual HRESULT Initialize();
	virtual int		Update(void);
	virtual void	Render(void);

public:
	static CBirdTest* Create();
	void	Release(void);

public:
	HRESULT	AddComponent();
};

