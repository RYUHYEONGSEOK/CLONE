#pragma once
#include "afxwin.h"


// CMeshTool 대화 상자입니다.
class CInfo;
class CDevice;
class CMouseCol;
class CBack;
class CMeshTool : public CDialog
{
	DECLARE_DYNAMIC(CMeshTool)

public:
	CMeshTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMeshTool();
public:
	bool m_bStart;
	bool m_bCheck[5];
	float m_PosX;
	float m_PosY;
	float m_PosZ;
	float m_RotationX;
	float m_RotationY;
	float m_RotationZ;
	float m_ScaleX;
	float m_ScaleY;
	float m_ScaleZ;
	CInfo*	m_pInfo;
	CString m_strSelectName;
	CMouseCol*	m_pMouseCol;
	DWORD	m_dwTime;
	float m_fPosValue;
	float m_fRotValue;
	float m_fScaleValue;
	

	CButton		m_ModeRadio[3];

public:
	void SetPickPos(D3DXVECTOR3 vPos);
	void SetPickScale(D3DXVECTOR3 vScale);
	//void SetPickRotation(D3DXVECTOR3 vRotation);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESHTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadmesh();
	CListBox m_ListBox;


	afx_msg void OnLbnSelchangeList1();

	//메쉬 위치관련
	afx_msg void OnEnChangePositionx();
	afx_msg void OnEnChangePositiony();
	afx_msg void OnEnChangePositionz();

	afx_msg void OnMapMode();
	afx_msg void OnBnClickedCreatemesh();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpin3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeRotationx();
	afx_msg void OnEnChangeRotationy();
	afx_msg void OnEnChangeRotationz();

	afx_msg void OnEnChangeScalex();
	afx_msg void OnEnChangeScaley();
	afx_msg void OnEnChangeScalez();
	afx_msg void OnBnClickedUpdate();

	afx_msg void OnBnClickedLastdelete();
};
