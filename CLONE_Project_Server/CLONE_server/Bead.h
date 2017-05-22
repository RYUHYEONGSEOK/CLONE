#pragma once

#include "Obj.h"

class CBead :
	public CObj
{
private:
	int		m_iOwnerID;
public:
	inline int	GetOwnerID(void) { return m_iOwnerID; }
	inline void SetOwnerID(int _iOwnerID) { m_iOwnerID = _iOwnerID; }

public:
	virtual int Progress(float _fFrameTime);

public:
	CBead();
	CBead(D3DXVECTOR3 _pos, int _iID);
	virtual ~CBead();
};