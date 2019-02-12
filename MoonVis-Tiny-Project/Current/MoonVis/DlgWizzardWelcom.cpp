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
// DlgWizzardWelcom.cpp : implementation file
//

#include "stdafx.h"
#include "MoonVis.h"
#include "DlgWizzardWelcom.h"
#include "DlgWizzard.h"


// DlgWizzardWelcom dialog

IMPLEMENT_DYNAMIC(CDlgWizzardWelcom, CDialog)

CDlgWizzardWelcom::CDlgWizzardWelcom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizzardWelcom::IDD, pParent)
{

}

CDlgWizzardWelcom::~CDlgWizzardWelcom()
{
}

void CDlgWizzardWelcom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWizzardWelcom, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgWizzardWelcom::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgWizzardWelcom::OnBnClickedButtonLoad)
END_MESSAGE_MAP()


// DlgWizzardWelcom message handlers

void CDlgWizzardWelcom::OnBnClickedButtonCreate()
{
	// TODO: Add your control notification handler code here
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();

	pDlgParent->hideWindow();
	pDlgParent->m_dlgCreate.ShowWindow(SW_SHOW);
}

void CDlgWizzardWelcom::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();

	pDlgParent->hideWindow();
	pDlgParent->m_dlgLoad.ShowWindow(SW_SHOW);
}
