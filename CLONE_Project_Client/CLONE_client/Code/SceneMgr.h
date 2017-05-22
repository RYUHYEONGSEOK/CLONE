#pragma once

class CDevice;
class CRenderer;
class CScene;
class CObj;
class CSceneMgr
{
	DECLARE_SINGLETON(CSceneMgr)
public:
	enum SceneID
	{
		SCENE_ID_LOGO,
		SCENE_ID_CUSTOM,
		SCENE_ID_STAGE,
		SCENE_ID_END
	};
	enum AddGroupType
	{
		ADD_GROUP_BACK,
		ADD_GROUP_FRONT,
		ADD_GROUP_END
	};

private:
	CDevice*		m_pDevice;

	SceneID			m_eCurScene;
	CScene*			m_pScene;
	bool			m_bSceneLoadingEnd;

	CRenderer*		m_pRenderer;

public:
	HRESULT		SetScene(SceneID eSceneID);
	HRESULT		SetSceneAftherLoading(SceneID eSceneID);

	CScene*		GetScene() { return m_pScene; }
	CRenderer*	GetRenderer(void) { return m_pRenderer; }

	bool		GetSceneLoadingEnd() { return m_bSceneLoadingEnd; }
	void		SetSceneLoadingEnd(bool _bEnd) { m_bSceneLoadingEnd = _bEnd; }
	
public:
	void		Add_RenderGroup(RENDERGROUP eType, CObj* pObj, AddGroupType eGroupType = ADD_GROUP_BACK);
	void		Clear_RenderGroup(void);

public:
	HRESULT		Init(CDevice* pDevice);
	void		Update();
	void		Render(const float& fTime);
	void		Release();

private:
	CSceneMgr();
public:
	~CSceneMgr();
};