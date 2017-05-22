
// MapToolView.h : CMapToolView Ŭ������ �������̽�
//

#pragma once

class CMapToolDoc;
class CCamera;
class CMouseCol;
class CStaticMesh;
class CStaticObject;
class CNaviMgr;
class CMapToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CMapToolView();
	DECLARE_DYNCREATE(CMapToolView)

// Ư���Դϴ�.
public:
	CMapToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	CCamera*		m_pCamera;
	//CButton		m_ModeRadio[3];
	CMouseCol*		m_pMouseCol;
	CStaticMesh*	m_pMesh;
//	CMouseCol*		m_pMouseCol;
//	CStaticMesh*	m_pMesh;
	D3DXVECTOR3		NaviPos1;
	D3DXVECTOR3		NaviPos2;
	D3DXVECTOR3		NaviPos3;
	int				m_iNaviPickCount;
	CStaticObject*	m_pStatic;
	CNaviMgr*		pNaviCell;

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	bool RaySphereIntersect(D3DXVECTOR3, D3DXVECTOR3, float);


// �����Դϴ�.
public:
	virtual ~CMapToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PostNcDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MapToolView.cpp�� ����� ����
inline CMapToolDoc* CMapToolView::GetDocument() const
   { return reinterpret_cast<CMapToolDoc*>(m_pDocument); }
#endif

