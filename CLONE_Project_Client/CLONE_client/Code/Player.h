#pragma once

#include "Obj.h"

class CAllianceTag;
class CPlayer
	: public CObj
{
private:
	CAllianceTag*		m_pAllianceTag;	//동맹일시 확인하는 Tag
	




private:
	float	m_fInitTime;
	bool	m_bCreate;
public:
	void	CollisionAABB(void);





private:
	//bool				m_bIsMyTeam;//나중에 팀전이 구현된 경우 적용
	bool				m_bIsMe;
	int					m_iID;



private:
	DWORD				m_dwIdx;		//Move On Navi 체크용 변수
	bool				m_bRun;			//Run Check
	bool				m_bSit;			//Sit Check
	bool				m_bAtt;			//Att Check
	bool				m_bJump;		//Jump Check
	bool				m_bKeyDown;		//Key Down Check
	float				m_fStamina;		//스테미너
	float				m_fMaxStamina;	//맥스 스테미너

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render(); 
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	void KeyCheck(void);
	void StateCheck(void);
	void PlayAnimation(void);
public:
	void				SetPlayerID(int _iID)		{ m_iID = _iID; }
	void				SetPlayerIsMe(bool _bIsMe)	{ m_bIsMe = _bIsMe; }
	bool				GetPlayerIsMe(void)			{ return m_bIsMe; }
	int					GetPlayerID(void)			{ return m_iID; }
	



private:
	bool	m_bIsFirstDie;
	DWORD	m_dwKey;
private:
	void	SendAnimationEnd(CObj::ObjState _objstate);
public:
	void	InputKeyCheck(void);
	void	SetPlayerKeyValue(DWORD _dwKey) { m_dwKey = _dwKey; }
	DWORD	GetPlayerKeyValue(void) { return m_dwKey; }




public:
	HRESULT	AddComponent(void);
	HRESULT Update_AllianceTag(void);

public:
	static CPlayer* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CPlayer(CDevice* pDevice);
public:
	virtual ~CPlayer();
};