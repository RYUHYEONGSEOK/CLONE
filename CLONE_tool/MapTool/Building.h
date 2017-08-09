#pragma once
#include "Obj.h"

class CShader;
class CVIBuffer;
class CTexture;
class CBuilding :
	public CObj
{
public:
	CBuilding();
	virtual ~CBuilding();

public:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);

public:
	static CBuilding*	Create(void);
	void Release(void);

public:
	HRESULT AddComponent(void);
};

