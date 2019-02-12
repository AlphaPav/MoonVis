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
#include "DlgSunSet.h"

#include <math.h>
#define M_PI 3.14159265f
#include "../CSM-src/main.h"
void normalize(float *v);

// CDlgSunSet dialog

IMPLEMENT_DYNAMIC(CDlgSunSet, CDialog)

CDlgSunSet::CDlgSunSet(CWnd* pParent, float x, float y, float z)
	: CDialog(CDlgSunSet::IDD, pParent)
{
	float dir[3];
	dir[0] = x, dir[1] = y, dir[2] = z;
	//normalize(dir);
	m_InitH = m_hAngle = atan2(dir[2], dir[0])*180/M_PI; // atan(x / y) * 180 / M_PI;
	m_InitV = m_vAngle = atan(dir[1])*180/M_PI; // atan(z / y) * 180 / M_PI;
}

CDlgSunSet::~CDlgSunSet()
{
}

void CDlgSunSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_hScroll);
	DDX_Control(pDX, IDC_SCROLLBAR2, m_vScroll);
	DDX_Control(pDX, IDC_EDIT2, m_hVal);
	DDX_Control(pDX, IDC_EDIT1, m_vVal);
}


BEGIN_MESSAGE_MAP(CDlgSunSet, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT2, &CDlgSunSet::OnEnChangeHVal)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgSunSet::OnEnChangeVVal)
	ON_BN_CLICKED(IDCANCEL, &CDlgSunSet::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSunSet message handlers
BOOL CDlgSunSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_hScroll.SetScrollRange(-180, 180);
	m_hScroll.SetScrollPos(m_hAngle);
	m_vScroll.SetScrollRange(0, 90);
	m_vScroll.SetScrollPos(m_vAngle);

	CString buf;
	buf.Format(L"%g", m_hAngle);
	m_hVal.SetWindowText(buf);

	buf.Format(L"%g", m_vAngle);
	m_vVal.SetWindowText(buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSunSet::Apply()
{
	// refresh the parent window ...
	float vAngle = m_vAngle;
	if (m_vAngle == 0)
		vAngle = 0.001;
	if (m_vAngle == 90)
		vAngle = 89.999;

	light_dir[0] = cos(m_hAngle / 180.f*M_PI); //tan(float(m_hAngle/180.f*M_PI));
	light_dir[2] = sin(m_hAngle / 180.f*M_PI); //tan(float(m_vAngle/180.f*M_PI));
	light_dir[1] = tan(vAngle / 180.f*M_PI); //1.f;
	normalize(light_dir);

	GetParent()->Invalidate();
}

void CDlgSunSet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
		m_hAngle = nCurpos;
		m_hVal.SetWindowText(buf);
	}
	else {
		m_vAngle = nCurpos;
		m_vVal.SetWindowText(buf);
	}


	Apply();

	//Default
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgSunSet::OnEnChangeHVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	TCHAR buffer[512];
	m_hVal.GetWindowText(buffer, 512);

	m_hAngle = _wtof(buffer);
	if (m_hAngle > 180.f)  m_hAngle = 180.f;
	if (m_hAngle < -180.f) m_hAngle = -180.f;
	
	m_hScroll.SetScrollPos(m_hAngle);  //更新信息

	Apply();
}

void CDlgSunSet::OnEnChangeVVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	TCHAR buffer[512];
	m_vVal.GetWindowText(buffer, 512);

	m_vAngle = _wtof(buffer);
	if (m_vAngle >= 90.f)  m_hAngle = 90.f;
	if (m_vAngle < 0.f) m_hAngle = 0.f;
	
	m_vScroll.SetScrollPos(m_vAngle);  //更新信息

	Apply();
}

void CDlgSunSet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_hAngle = m_InitH;
	m_vAngle = m_InitV;

	Apply();
	OnCancel();
}
