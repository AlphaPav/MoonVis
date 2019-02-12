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
// DlgWizzardLoad.cpp : implementation file
//

#include "stdafx.h"
#include "MoonVis.h"
#include "DlgWizzardLoad.h"
#include "DlgWizzard.h"

// CDlgWizzardLoad dialog

IMPLEMENT_DYNAMIC(CDlgWizzardLoad, CDialog)

CDlgWizzardLoad::CDlgWizzardLoad(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizzardLoad::IDD, pParent)
{

}

CDlgWizzardLoad::~CDlgWizzardLoad()
{
}

void CDlgWizzardLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editPath);
}


BEGIN_MESSAGE_MAP(CDlgWizzardLoad, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgWizzardLoad::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgWizzardLoad::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgWizzardLoad::OnBnClickedButtonOpenFile)
END_MESSAGE_MAP()


// CDlgWizzardLoad message handlers

void CDlgWizzardLoad::OnBnClickedButtonPrev()
{
	// TODO: Add your control notification handler code here
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();
	pDlgParent->hideWindow();
	pDlgParent->m_dlgWelcome.ShowWindow(SW_SHOW);
}

void CDlgWizzardLoad::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();
	m_editPath.GetWindowText(pDlgParent->m_strLoadPath);
	pDlgParent->m_lunchState = CDlgWizzard::LUNCH_STATE_LOAD;
	pDlgParent->doOk();
}

void CDlgWizzardLoad::OnBnClickedButtonOpenFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("MVSÎÄ¼þ(*.mvs)|*.mvs||"));
	if (IDOK == dlg.DoModal())
	{
		m_editPath.SetWindowText(dlg.GetPathName());
	}
}
