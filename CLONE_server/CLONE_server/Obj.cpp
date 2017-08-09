#include "stdafx.h"
#include "Obj.h"

CObj::CObj()
	: m_eObjState(OBJ_STATE_IDEL)
	, m_eMapType(MAP_END)
	, m_vPos(D3DXVECTOR3(0.f, 0.f, 0.f))
	, m_vDir(D3DXVECTOR3(0.f, 0.f, 0.f))
	, m_fRadious(0.f)
{
	ZeroMemory(m_fAngle, sizeof(float) * ANGLE_END);
}
CObj::~CObj()
{
}