#pragma once

class CObj;

class CCollisionMgr
{
	DECLARE_SINGLETON(CCollisionMgr)

public:
	bool CheckDistance(CObj* _pMyObj, CObj* _pTargetObj, float _fGoalDistance);

private:
	CCollisionMgr();
	~CCollisionMgr();
};