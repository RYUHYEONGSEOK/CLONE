#pragma once

#include "Layer.h"

class CDevice;
class CCamera;
class CLoading;
class CScene
{
protected:
	CDevice*		m_pDevice;
	CLayer*			m_pLayer[CLayer::LAYERTYPE_END];

	CObj*			m_pMainCamera;
	CLoading*		m_pLoading;

public:
	virtual HRESULT		InitScene()		PURE;
	virtual void		Update()		PURE;
	virtual void		Render()		PURE;
	virtual void		Release()		PURE;

public:
	void Update_Layer();
	void Render_Layer();
	void ChangeState_Layer(STAGESTATETYPE eType);
public:
	const CComponent*							GetComponent(CLayer::LayerType eType, const wstring& wstrObjKey, const wstring& wstrComponentKey);
	const CObj*									GetObj(CLayer::LayerType eType, const wstring& wstrObjKey, const unsigned int& iIndex = 0);
	const unordered_map<wstring, list<CObj*>>*	GetObjList(CLayer::LayerType eType);
private:
	void Release_Layer();

public:
	CLoading*	GetLoading(void) { return m_pLoading; }
	
	CObj*	GetMainCam(void) { return m_pMainCamera; }

public:
	CScene(CDevice* pDevice);
	~CScene();
};