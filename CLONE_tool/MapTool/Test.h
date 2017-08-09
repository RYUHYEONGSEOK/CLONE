#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CTest :
	public CObj
{
public:
	CTest();
	virtual ~CTest();

public:
	CVIBuffer*			m_pBuffer;
	CShader*			m_pVertexShader;
	CShader*			m_pPixelShader;
	CTexture*			m_pTexture;
	VTXTEX*				m_pVerTex;
	

public:
	virtual HRESULT		Initialize(void);
	virtual int			Update(void);
	virtual void		Render(void);
	
public:
	static CTest*		Create(void);
	void				Release(void);

public:
	HRESULT				AddComponent(void);
};

