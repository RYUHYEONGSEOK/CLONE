#include "stdafx.h"
#include "Bead.h"

CBead::CBead()
	: m_iOwnerID(UNKNOWN_VALUE)
{
}
CBead::CBead(D3DXVECTOR3 _pos, int _iID)
	: m_iOwnerID(UNKNOWN_VALUE)
{
	m_vPos = _pos;
	m_iID = _iID;
}
CBead::~CBead()
{
}

int CBead::Progress(float _fFrameTime)
{
	return 0;
}