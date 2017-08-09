#pragma once

#include "VIBuffer.h"

class CAniBuffer
	: public CBuffer
{
protected:
	ID3D11Buffer*			m_pVertexBuffer;

public:
	long long				m_llAniTime;
	string					m_strName;
	vector<CAniBuffer*>		m_vecChildBuffer;

public:
	VertexAni*				m_pVertex;
	unsigned int			m_nStartIndex;
	unsigned int			m_nBaseVertex;

	unsigned int			m_nVertices;
	unsigned int			m_nStride;
	unsigned int			m_nOffset;

public:
	CAniBuffer* Clone(void);

	VertexAni*	GetVertex(unsigned int _nIndex);
	void		SetVertexSize(unsigned int _iSize);

	CAniBuffer* GetChild(unsigned int _nindex);
	void		AddChild(CAniBuffer* _pChildBuffer);

	HRESULT		CreateBuffer();
	void		SetBuffer();

	void		SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName, FbxNode* _pNode);

	virtual void Update();
	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Release();

public:
	static CAniBuffer* Create(CDevice* pDevice);

private:
	explicit CAniBuffer(CDevice* pDevice);
	explicit CAniBuffer(const CAniBuffer& rhs);
public:
	virtual ~CAniBuffer(void);
};