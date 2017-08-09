#pragma once
#include "MeshTool.h"



// CMyForm �� ���Դϴ�.
class CMapToolView;
class CNaviMgr;
class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CFont			m_Font;
	CMapToolView*	m_pView;
	//CMeshTool*		m_Tab1;
	CNaviMgr*		pNavi;
	CMeshTool		m_MeshTool;
	CButton			m_iRadio[2];
	CButton			m_iNaviRadio[2];
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedMeshtool();
	afx_msg void OnBnClickedMeshsave();
	afx_msg void OnBnClickedMeshload();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedStatic4();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedMeshsave2();
	afx_msg void OnBnClickedNavisave();
	afx_msg void OnBnClickedNaviload();
};


