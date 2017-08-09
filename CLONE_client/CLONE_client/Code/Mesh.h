#pragma once

#include "VIBuffer.h"

/*
//애니메이션 관련 변수들 입니다.
float g_fAnimationPlayTime = 0.0f;				//애니메이션 재생 시간
XMMATRIX **g_pAnimationMatrix = nullptr;		//애니메이션 Matrix[시간별][인덱스별]
XMMATRIX * g_pBaseBoneMatrix = nullptr;			//애니메이션 BaseMatrix[인덱스별]
XMMATRIX ** g_pResultMatrix = nullptr;			//애니메이션 최종 변환 Matrix[시간별][인덱스별]
long long g_llAnimationMaxTime = 0;				//애니메이션 최대길이
unsigned int g_uiAnimationNodeIndexCount = 0;	//애니메이션 노드의 갯수
std::map<std::string, unsigned int> g_mIndexByName;

//애니메이션와 관련된 상수 버퍼
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
	bool					Animend;	//애니메이션이 끝났는지 체크하는 변수

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

