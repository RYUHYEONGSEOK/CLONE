#pragma once

const WORD	BONE_NUM = 8;
const WORD	BONE_MATRIX_NUM = 128;

struct Buffer_View
{
	D3DXMATRIX			m_matView;
};

struct Buffer_Proj
{
	D3DXMATRIX			m_matProj;
};

struct Buffer_World
{
	D3DXMATRIX			m_matWorld;
};


struct VertexColor
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR4		vColor;

	VertexColor() {}
	VertexColor(D3DXVECTOR3 _vPos, D3DXVECTOR4 _vColor)
		: vPos(_vPos), vColor(_vColor) {}
};

struct VertexFont
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR2		vTextureUV;

	VertexFont() {}
	VertexFont(D3DXVECTOR3 _vPos, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vTextureUV(_vTextureUV) {}
};

struct VertexWater
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR2		vTextureUV;

	VertexWater() {}
	VertexWater(D3DXVECTOR3 _vPos, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vTextureUV(_vTextureUV) {}
};

struct VertexScreen
{
	D3DXVECTOR4		vPos;
	D3DXVECTOR2		vTextureUV;

	VertexScreen() {}
	VertexScreen(D3DXVECTOR4 _vPos, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vTextureUV(_vTextureUV) {}
};

struct VertexTexture
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTextureUV;

	VertexTexture() {}
	VertexTexture(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vNormal, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vNormal(_vNormal), vTextureUV(_vTextureUV) {}
};

struct VertexAni
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTextureUV;

	int				iBoneIdx[BONE_NUM];
	float			fBoneWeight[BONE_NUM];

	VertexAni() {}
	VertexAni(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vNormal)
		: vPos(_vPos), vNormal(_vNormal) {}
	VertexAni(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vNormal, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vNormal(_vNormal), vTextureUV(_vTextureUV) {}

	void SetPos(float _x, float _y, float _z) { vPos.x = _x; vPos.y = _y; vPos.z = _z; }
	void SetNormal(float _x, float _y, float _z) { vNormal.x = _x; vNormal.y = _y; vNormal.z = _z; }
	void AddBone(int _iIdx, float _fWeight);
};

struct VertexSkyBox
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR2		vTextureUV;

	VertexSkyBox() {}
	VertexSkyBox(D3DXVECTOR3 _vPos, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vTextureUV(_vTextureUV) {}
};

struct INDEX16
{
	WORD	_1, _2, _3;

	INDEX16() {}
	INDEX16(WORD _1, WORD _2, WORD _3) : _1(_1), _2(_2), _3(_3) {}
};

struct INDEX32
{
	DWORD	_1, _2, _3;

	INDEX32() {}
	INDEX32(DWORD _1, DWORD _2, DWORD _3) : _1(_1), _2(_2), _3(_3) {}
};

typedef struct tagVertexTexture
{
	D3DXVECTOR3				vPos;
	D3DXVECTOR3				vNormal;
	D3DXVECTOR2				vTexUV;
}VTXTEX;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct ConstantBufferPlusFloat4
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 mFloat4;
};

struct ConstantBufferFloat4
{
	XMFLOAT4 mfloat4;
};

struct LightBufferType
{
	D3DXVECTOR4 ambientColor;
	D3DXVECTOR4 diffuseColor;
	D3DXVECTOR4	lightDirection;
	D3DXVECTOR4	specularColor;
	D3DXVECTOR4	lightPos;
};

struct WaterBufferType
{
	float waterTranslation;
	float reflectRefractScale;
	D3DXVECTOR2 padding;
};

struct ReflectionBufferType
{
	D3DXMATRIX reflectionMatrix;
};

struct ClipPlaneBufferType
{
	D3DXVECTOR4 clipPlane;
};

struct FogBufferType
{
	D3DXVECTOR4 fogInfo;
};

typedef struct LightStruct
{
	D3DXVECTOR4		direction;
	D3DXVECTOR4		pos;
	D3DXVECTOR4		ambientColor;
	D3DXVECTOR4		diffuseColor;
	D3DXVECTOR4		specular;
	D3DXVECTOR4		range;
	D3DXVECTOR3		LookAt;
	D3DXMATRIX		matView;
	D3DXMATRIX		matProj;
	LIGHTINGTYPE	eLightType;
}LightInfo;

typedef struct LightMatStruct
{
	D3DXMATRIX		LightView;
	D3DXMATRIX		LightProj;
}LightMatrixStruct;

struct NormalVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 texture;
	XMFLOAT3 Normal;
};

struct PixelMatrixBufferType
{
	D3DXVECTOR4 CamPos;
	XMMATRIX	ProjInv;
	XMMATRIX	ViewInv;
	XMMATRIX	VPInv;
	D3DXVECTOR4 Range;
};
	

//Dynamic Mesh
struct VS_CB_BONE_MATRIX
{
	XMMATRIX m_XMmtxBone[BONE_MATRIX_NUM];
};

class CAniBuffer;
struct Animation
{
	string			strAniName;
	CAniBuffer*		pAniBuffer;

	long long		llAniMaxTime;
	//float			fAniPlayTimer;
	float			fAniPlaySpeed;

	XMMATRIX**		ppAniMatrix;
	XMMATRIX*		pBaseBoneMatrix;
	XMMATRIX**		ppResultMatrix;

	unsigned int	nAniNodeIdxCnt;
	map<string, unsigned int>	mapIndexByName;


	ID3D11Buffer*				pBoneMatrixBuffer;
	D3D11_MAPPED_SUBRESOURCE	tMappedResource;
	VS_CB_BONE_MATRIX*			pBoneMatrix;

	Animation()
		: pAniBuffer(NULL)
		, llAniMaxTime(0)
		//, fAniPlayTimer(0.f)
		, fAniPlaySpeed(100.f)

		, ppAniMatrix(NULL)
		, pBaseBoneMatrix(NULL)
		, ppResultMatrix(NULL)

		, nAniNodeIdxCnt(0)
		, pBoneMatrixBuffer(NULL)
		, pBoneMatrix(NULL)
	{}

	Animation(const Animation & rhs)
	{
		strAniName = rhs.strAniName;
		pAniBuffer = rhs.pAniBuffer;

		llAniMaxTime = rhs.llAniMaxTime;
		fAniPlaySpeed = rhs.fAniPlaySpeed;

		ppAniMatrix = rhs.ppAniMatrix;
		pBaseBoneMatrix = rhs.pBaseBoneMatrix;
		ppResultMatrix = rhs.ppResultMatrix;

		nAniNodeIdxCnt = rhs.nAniNodeIdxCnt;
		mapIndexByName = rhs.mapIndexByName;

		pBoneMatrixBuffer = rhs.pBoneMatrixBuffer;
		tMappedResource = rhs.tMappedResource;
		pBoneMatrix = rhs.pBoneMatrix;
	}
};


// -------------------------------------------------------------
// Include 꼬임때문에 어쩔수 없이 여기에 배치...
// 게임 Info
struct tGameInfo
{
	bool bMode;		// FALSE면 SOLO, TRUE면 TEAM
	bool bMapType;
	int iBotCnt;

	tGameInfo()
	{
		bMode = false;	// FALSE면 SOLO, TRUE면 TEAM
		bMapType = false;
		iBotCnt = 0;
	}
};


// 클라이언트 Struct
typedef struct tagClient
{
	//클라이언트 정보
	bool   m_bIsHost;
	bool   m_bIsReady;
	int    m_iRoomIndex;
	char   m_szName[MAX_PATH];

	//생성자 및 소멸자
	tagClient(void)
	{
		//information
		m_bIsHost = false;
		m_bIsReady = false;
		m_iRoomIndex = -1;
		ZeroMemory(m_szName, sizeof(char) * MAX_PATH);
	}
	tagClient(const tagClient& rhs)
	{
		m_bIsHost = rhs.m_bIsHost;
		m_bIsReady = rhs.m_bIsReady;
		m_iRoomIndex = rhs.m_iRoomIndex;
		memcpy(m_szName, rhs.m_szName, sizeof(m_szName));
	}
	~tagClient(void)
	{
	}
}CLIENT;