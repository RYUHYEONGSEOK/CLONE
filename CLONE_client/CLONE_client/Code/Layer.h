#pragma once

class CDevice;
class CObj;
class CComponent;
class CLayer
{
public:
	enum LayerType
	{
		LAYERTYPE_ENVIRONMENT,
		LAYERTYPE_GAMELOGIC,
		LAYERTYPE_UI,
		LAYERTYPE_END
	};

public:
	typedef list<CObj*>	ListObj;

private:
	CDevice*				m_pDevice;
	unordered_map<wstring, ListObj>	m_mapObjList;

public:
	static CLayer*		Create(CDevice* pDevice);
	HRESULT				AddObj(const wstring& wstrObjKey, CObj* pObj);

	const CObj*				GetObj(const wstring& wstrObjKey, const unsigned int& iIndex = 0);
	const CComponent*		GetComponent(const wstring& wstrObjKey, const wstring& wstrComponentKey);

	unordered_map<wstring, ListObj>*	GetMapObjList() { return &m_mapObjList; }

public:
	void	Update_Obj();
	void	Render_Obj();
	void	ChangeState_Obj(STAGESTATETYPE eType);
	void	Release_Obj();

private:
	explicit CLayer(CDevice* pDevice);
public:
	~CLayer();
};
