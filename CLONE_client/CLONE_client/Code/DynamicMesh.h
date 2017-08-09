#pragma once

#include "Mesh.h"
class CDynamicMesh : public CMesh
{
private:
	//Bone Matrix¸¦ Ã£´Â ÇÃ·¡±×, Ani¹Ù²ð¶§ false·Î ¹Ù²ãÁà¾ß´ï...
	bool			m_bFindBone;	

public:
	static CResource*		Create(CDevice* pDevice,
		const char* pPath,
		vector<string> vecAniName);

	virtual CResource*		Clone();
	
protected:
	virtual HRESULT		Load_Model(const char*,
		vector<string> vecAniName,
		FbxManager*, FbxIOSettings*, FbxScene*, FbxImporter*);


public:
	virtual HRESULT		Init(const char* pPath, vector<string> vecAniName);

	XMMATRIX**	GetBoneMatrix(FbxNode* pNode, Animation* pAni, const string* pAniName, const char* pBoneName);
	
	void CreateRasterizerState(void);

public:
	virtual void		Update();

private:
	void Release();
	void Release_Animation();

public:
	void		PlayPlayerAnimation(int iIdx);
	void		PlayBotAnimation(int iIdx);
	void		RenderMesh(int iIdx);
	void		BWPlayAnim(int iIdx);

protected:
	CDynamicMesh(CDevice* pDevice);
	CDynamicMesh(const CDynamicMesh& rhs);

public:
	virtual ~CDynamicMesh();
};