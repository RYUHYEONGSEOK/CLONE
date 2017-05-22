#include "stdafx.h"
#include "CollisionMgr.h"

#include "Obj.h"

#include "MathHelper.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}
CCollisionMgr::~CCollisionMgr()
{
}

bool CCollisionMgr::CheckDistance(CObj* _pMyObj, CObj* _pTargetObj, float _fGoalDistance)
{
	D3DXVECTOR3 vMyPos = _pMyObj->GetPos();
	D3DXVECTOR3 vTargetPos = _pTargetObj->GetPos();

	float fObjDistance = (vMyPos.x - vTargetPos.x) * (vMyPos.x - vTargetPos.x)
		+ (vMyPos.x - vTargetPos.y) * (vMyPos.x - vTargetPos.y);
	float fGoalDistance = _fGoalDistance * _fGoalDistance;

	if (fObjDistance < fGoalDistance)
	{
		return true;
	}
	return false;
}