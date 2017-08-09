#pragma once

#include "stdafx.h"
#include "Logo.h"
#include "Custom.h"
#include "Stage.h"

class CScene;
class CDevice;
class CSceneSelector
{
public:
	enum SCENE { SCENE_LOGO, SCENE_STAGE, SCENE_CUSTOM, SCENE_END };
public:
	explicit CSceneSelector(SCENE eSceneID) : m_eSceneID(eSceneID) {}
	~CSceneSelector(void) {}
public:
	HRESULT operator () (CScene** ppScene, CDevice*	pDevice)
	{
		switch (m_eSceneID)
		{
		case SCENE_LOGO:
			*ppScene = CLogo::Create(pDevice);
			break;
		case SCENE_CUSTOM:
			*ppScene = CCustom::Create(pDevice);
			break;
		case SCENE_STAGE:
			*ppScene = CStage::Create(pDevice);
			break;
		}

		NULL_CHECK_RETURN(*ppScene, E_FAIL);

		return S_OK;
	}
private:
	SCENE			m_eSceneID;
};