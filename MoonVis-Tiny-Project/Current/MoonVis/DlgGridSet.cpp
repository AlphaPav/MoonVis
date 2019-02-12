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
// DlgSunSet.cpp : implementation file
//

#include "stdafx.h"
#include "MoonVis.h"
#include "DlgGridSet.h"

#include <math.h>
#define M_PI 3.14159265f
#include "../CSM-src/main.h"
void normalize(float *v);

// CDlgGridSet dialog

IMPLEMENT_DYNAMIC(CDlgGridSet, CDialog)

CDlgGridSet::CDlgGridSet(CWnd* pParent)
	: CDialog(CDlgGridSet::IDD, pParent)
{
	m_InitH = xgrid;
	m_InitV = ygrid;
}

CDlgGridSet::~CDlgGridSet()
{
}

void CDlgGridSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_hScroll);
	DDX_Control(pDX, IDC_SCROLLBAR2, m_vScroll);
	DDX_Control(pDX, IDC_EDIT2, m_hVal);
	DDX_Control(pDX, IDC_EDIT1, m_vVal);
}


BEGIN_MESSAGE_MAP(CDlgGridSet, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT2, &CDlgGridSet::OnEnChangeHVal)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgGridSet::OnEnChangeVVal)
	ON_BN_CLICKED(IDCANCEL, &CDlgGridSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgGridSet message handlers
BOOL CDlgGridSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_hScroll.SetScrollRange(0, 100);
	m_hScroll.SetScrollPos(xgrid);
	m_vScroll.SetScrollRange(0, 100);
	m_vScroll.SetScrollPos(ygrid);

	CString buf;
	buf.Format(L"%g", xgrid);
	m_hVal.SetWindowText(buf);

	buf.Format(L"%g", ygrid);
	m_vVal.SetWindowText(buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGridSet::Apply()
{
	// refresh the parent window ...
	GetParent()->Invalidate();
}

void CDlgGridSet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	// TODO: Add your message handler code here and/or call default
	int nMin,nMax,nCurpos;
	pScrollBar->GetScrollRange(&nMin,&nMax);  //取得滚动条范围
	nCurpos=pScrollBar->GetScrollPos();          //取得当前值
	switch(nSBCode)                 //处理滚动信息
	{
		case SB_LINELEFT:                 //向左的箭头被按下
			nCurpos--;
			break;   
		case SB_LINERIGHT:
			nCurpos++;
			break;    //向右的箭头被按下
		
		case SB_THUMBTRACK:         //鼠标拖动
			nCurpos=nPos;
			break;
	}

	if(nCurpos>nMax)  nCurpos=nMax;
	if (nCurpos<nMin) nCurpos = nMin;
	pScrollBar->SetScrollPos(nCurpos);  //更新信息

	
	CString buf;
	buf.Format(L"%d", nCurpos);

	if (pScrollBar-> GetDlgCtrlID()   ==   IDC_SCROLLBAR1) {
		xgrid = nCurpos;
		m_hVal.SetWindowText(buf);
	}
	else {
		ygrid = nCurpos;
		m_vVal.SetWindowText(buf);
	}


	Apply();

	//Default
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgGridSet::OnEnChangeHVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	TCHAR buffer[512];
	m_hVal.GetWindowText(buffer, 512);

	xgrid = _wtof(buffer);
	m_hScroll.SetScrollPos(xgrid);  //更新信息

	Apply();
}

void CDlgGridSet::OnEnChangeVVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	TCHAR buffer[512];
	m_vVal.GetWindowText(buffer, 512);

	ygrid = _wtof(buffer);
	m_vScroll.SetScrollPos(ygrid);  //更新信息

	Apply();
}

void CDlgGridSet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	xgrid = m_InitH;
	ygrid = m_InitV;

	Apply();
	OnCancel();
}
