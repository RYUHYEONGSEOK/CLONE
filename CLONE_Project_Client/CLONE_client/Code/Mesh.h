#pragma once

#include "VIBuffer.h"

/*
//�ִϸ��̼� ���� ������ �Դϴ�.
float g_fAnimationPlayTime = 0.0f;				//�ִϸ��̼� ��� �ð�
XMMATRIX **g_pAnimationMatrix = nullptr;		//�ִϸ��̼� Matrix[�ð���][�ε�����]
XMMATRIX * g_pBaseBoneMatrix = nullptr;			//�ִϸ��̼� BaseMatrix[�ε�����]
XMMATRIX ** g_pResultMatrix = nullptr;			//�ִϸ��̼� ���� ��ȯ Matrix[�ð���][�ε�����]
long long g_llAnimationMaxTime = 0;				//�ִϸ��̼� �ִ����
unsigned int g_uiAnimationNodeIndexCount = 0;	//�ִϸ��̼� ����� ����
std::map<std::string, unsigned int> g_mIndexByName;

//�ִϸ��̼ǿ� ���õ� ��� ����
ID3D11Buffer *g_pd3dcbBoneMatrix = nullptr;
D3D11_MAPPED_SUBRESOURCE g_d3dMappedResource;
VS_CB_BONE_MATRIX *g_pcbBoneMatrix = nullptr;
*/

class CMesh
	: public CBuffer
{
protected:
	D3DXVECTOR3		m_vMin;
	D3DXVECTOR3		m_vMax;

	CBuffer*		m_pBoundingBox;

protected:
	WORD					m_wCurrenAniIdx;
	vector<Animation*>		m_vecAni;
	int						m_iRepeatTime;

	float					m_fAniPlayTimer;
	bool					Yamae;

	FbxScene*				pFBXScene;

public:
	bool					Animend;	//�ִϸ��̼��� �������� üũ�ϴ� ����

public:
	void MakeVertexNormal(BYTE* _pVertices, WORD* _pIndices);
	void SetNormalVectorByBasic(BYTE* _pVertices);
	void SetNormalVectorByAverage(BYTE* _pVertices, WORD* _pIndices, int _iPrimitives, int _iOffset, bool _bStrip);
	D3DXVECTOR3 GetTriAngleNormal(BYTE* _pVertices, USHORT _nIndex_0, USHORT _nIndex_1, USHORT _nIndex_2);

	CBuffer*		GetBoundingBoxBuffer(void) { return m_pBoundingBox; }
	void			GetMinMax(D3DXVECTOR3* const pMin, D3DXVECTOR3* const pMax);

	float			GetAniPlayTime(void)		{ return m_fAniPlayTimer; }
	int				GetCurrentAniIdx(void)		{ return (int)m_wCurrenAniIdx; }

public:
	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update(void);
	virtual void Release(void);

protected:
	explicit CMesh(CDevice* pDevice);
	explicit CMesh(const CMesh& rhs);
public:
	virtual ~CMesh(void);
};

