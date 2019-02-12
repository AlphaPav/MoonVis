/*************************************************************************\

Copyright 2018 Zhejiang University.
All Rights Reserved.

Permission to use, copy, modify and distribute this software and its
documentation for educational, research and non-profit purposes, without
fee, and without a written agreement is hereby granted, provided that the
above copyright notice and the following three paragraphs appear in all
copies.

IN NO EVENT SHALL ZHEJIANG UNIVERSITY BE
LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF ZHEJIANG
UNIVERSITY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

ZHEJIANG UNIVERSITY SPECIFICALLY DISCLAIM ANY
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND ZHEJIANG UNIVERSITY
HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

The authors may be contacted via:

Mail:             College of Computer Science and Technology
Zhejiang Univeristy
Hangzhou, Zhejiang, China 310027

EMail:            tang_m@zju.edu.cn

\**************************************************************************/
// MoonVisView.h : CMoonVisView 类的接口
//


#pragma once

#include "../avi/AVIRecorder.h"

class CMoonVisView : public CView
{
protected: // 仅从序列化创建
	CMoonVisView();
	DECLARE_DYNCREATE(CMoonVisView)

// 属性
public:
	CMoonVisDoc* GetDocument() const;

	CPalette m_cPalette;
	CRect m_oldRect;
	CDC* m_pDC;
	UINT m_nTimerID;

protected:
	BOOL m_bIsRecording;
	CAVIRecorder m_cAviRec;
	CBmpCapturer m_cBmpCap;

	long m_anmTimeTag;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMoonVisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL Init();
	void EnableRecording(BOOL flag, const wchar_t *fname = NULL);

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetSun();
	afx_msg void OnAnmEdit();
	afx_msg void OnSetGrid();
	afx_msg void OnLoadModel();
	afx_msg void OnLoadTerrain();
	afx_msg void OnCameraAdd();
	afx_msg void OnCameraList();
	afx_msg void OnPointList();
	afx_msg void OnViewGrid();
	afx_msg void OnViewFPS();
	afx_msg void OnViewWings();
	afx_msg void OnViewCamera();
	afx_msg void OnViewFog();
	afx_msg void OnViewWireframe();
	afx_msg void OnViewDepthMap();
	afx_msg void OnReloadAnimation();
	afx_msg void OnUpdateReloadAnimation(CCmdUI* pCmdUI);
	afx_msg void OnViewAnimation();
	afx_msg void OnUpdateViewAnimation(CCmdUI* pCmdUI);
	afx_msg void OnPauseAnimation();
	afx_msg void OnUpdatePauseAnimation(CCmdUI* pCmdUI);
	afx_msg void OnRestoreAnimation();
	afx_msg void OnAviBegin();
	afx_msg void OnAviEnd();
	afx_msg void OnUpdateAviBegin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAviEnd(CCmdUI* pCmdUI);
	afx_msg void OnBmpCapture();
	afx_msg void OnShadowRatio();

	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewWings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewCamera(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDepthMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFPS(CCmdUI* pCmdUI);

	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // MoonVisView.cpp 中的调试版本
inline CMoonVisDoc* CMoonVisView::GetDocument() const
   { return reinterpret_cast<CMoonVisDoc*>(m_pDocument); }
#endif

