#pragma once

#include "Mesh.h"

class CStaticMesh : public CMesh
{
public:
	static CResource*		Create(CDevice* pDevice, const char* pPath);
	virtual CResource*		Clone();

private:
	virtual HRESULT		Load_Model(const char* pPath,
		FbxManager* pFBXManager, FbxIOSettings* pSetting, FbxScene* pFBXScene, FbxImporter* pImporter);

public:
	virtual HRESULT		Init(const char* pPath);

	void CreateRasterizerState(void);

public:
	virtual void		Update();

private:
	void Release();

protected:
	CStaticMesh(CDevice* pDevice);
	CStaticMesh(const CStaticMesh& rhs);
public:
	virtual ~CStaticMesh();
};
