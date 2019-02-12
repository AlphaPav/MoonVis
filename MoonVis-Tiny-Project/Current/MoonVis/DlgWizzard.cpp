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
// DlgWizzard.cpp : implementation file
//

#include "stdafx.h"
#include "MoonVis.h"
#include "DlgWizzard.h"

// CWizzardDlg dialog

IMPLEMENT_DYNAMIC(CDlgWizzard, CDialog)

CDlgWizzard::CDlgWizzard(CString strLoadPath, CWnd* pParent /*=NULL*/)
	:m_strLoadPath(strLoadPath)
	, CDialog(CDlgWizzard::IDD, pParent)
	, m_lunchState(LUNCH_STATE_NONE)
{

}

CDlgWizzard::~CDlgWizzard()
{
}

void CDlgWizzard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWizzard, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgWizzard::OnBnClickedButtonCreate)
END_MESSAGE_MAP()

BOOL CDlgWizzard::OnInitDialog()
{
	if (!CDialog::OnInitDialog())
	{
		return FALSE;
	}

	m_dlgWelcome.Create(IDD_DLG_WIZZARD_WELCOM, this);
	m_dlgCreate.Create(IDD_DLG_WIZZARD_CREATE, this);
	m_dlgLoad.Create(IDD_DLG_WIZZARD_LOAD, this);
	hideWindow();
	m_dlgWelcome.ShowWindow(TRUE);

	m_dlgLoad.m_editPath.SetWindowText(m_strLoadPath);

	return TRUE;
}


// CWizzardDlg message handlers

void CDlgWizzard::OnBnClickedButtonCreate()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("create"));
}

void CDlgWizzard::hideWindow(void)
{
	m_dlgWelcome.ShowWindow(SW_HIDE);
	m_dlgCreate.ShowWindow(SW_HIDE);
	m_dlgLoad.ShowWindow(SW_HIDE);
}