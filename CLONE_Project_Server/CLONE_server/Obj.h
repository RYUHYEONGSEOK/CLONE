#pragma once

class CObj
{
protected:
	int			m_iID;
public:
	inline int GetID(void) { return m_iID; }

protected:
	eObjState	m_eObjState;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;
	float		m_fAngle[ANGLE_END];
	float		m_fRadious;
	

public:
	virtual int Progress(float _fFrameTime)PURE;

public:
	virtual inline D3DXVECTOR3 GetPos(void) { return m_vPos; }
	virtual inline void SetPos(D3DXVECTOR3 _vPos) { m_vPos = _vPos; }
	virtual inline D3DXVECTOR3 GetDir(void) { return m_vDir; }
	virtual inline void SetDir(D3DXVECTOR3 _vDir) { m_vDir = _vDir; }
	virtual inline void SetObjState(eObjState _eObjState) { m_eObjState = _eObjState; }
	virtual inline eObjState GetObjState(void) { return m_eObjState; }
	virtual inline float GetRad(void) { return m_fRadious; }
	virtual inline void SetRad(float _fRad) { m_fRadious = _fRad; }
	virtual inline float GetAngleY(void) { return m_fAngle[ANGLE_Y]; }
	virtual inline void SetAngleY(float _fAng) { m_fAngle[ANGLE_Y] = _fAng; }

public:
	CObj();
	virtual ~CObj();
};