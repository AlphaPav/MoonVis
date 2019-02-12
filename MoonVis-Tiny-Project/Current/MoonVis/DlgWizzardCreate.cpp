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
// DlgWizzardCreate.cpp : implementation file
//

#include "stdafx.h"
#include "MoonVis.h"
#include "DlgWizzardCreate.h"
#include "DlgWizzard.h"

extern char* depth_tex_filename;
extern char* inf_filename;
extern wchar_t* explorer_filename;
extern wchar_t* explorer_animation_filename;
extern wchar_t* material_filename;
extern wchar_t* point_filename;


// CDlgWizzardCreate dialog

IMPLEMENT_DYNAMIC(CDlgWizzardCreate, CDialog)

CDlgWizzardCreate::CDlgWizzardCreate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizzardCreate::IDD, pParent)
{

}

CDlgWizzardCreate::~CDlgWizzardCreate()
{
}

void CDlgWizzardCreate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editPath1);
	DDX_Control(pDX, IDC_EDIT2, m_editPath2);
}


BEGIN_MESSAGE_MAP(CDlgWizzardCreate, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgWizzardCreate::OnBnClickedButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgWizzardCreate::OnBnClickedButtonOpenFile1)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgWizzardCreate::OnBnClickedButtonOpenFile2)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgWizzardCreate::OnBnClickedButtonNext)
END_MESSAGE_MAP()


// CDlgWizzardCreate message handlers

void CDlgWizzardCreate::OnBnClickedButtonPrev()
{
	// TODO: Add your control notification handler code here
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();
	pDlgParent->hideWindow();
	pDlgParent->m_dlgWelcome.ShowWindow(SW_SHOW);
}

void CDlgWizzardCreate::OnBnClickedButtonOpenFile1()
{
	// TODO: Add your control notification handler code here

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("模型文件(*.wrl;*.bin)|*.wrl;*.bin||"));
	if (IDOK == dlg.DoModal())
	{
		m_editPath1.SetWindowText(dlg.GetPathName());
	}
}

void CDlgWizzardCreate::OnBnClickedButtonOpenFile2()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("PNG文件(*.png)|*.png||"));
	if (IDOK == dlg.DoModal())
	{
		m_editPath2.SetWindowText(dlg.GetPathName());
	}
}

void CDlgWizzardCreate::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	CDlgWizzard* pDlgParent = (CDlgWizzard*)GetParent();
	pDlgParent->m_lunchState = CDlgWizzard::LUNCH_STATE_CREATE;

	CString buff;
	
	m_editPath2.GetWindowText(buff);
	if (NULL != depth_tex_filename)
		delete[] depth_tex_filename;
	WideCharToMultiByte_M(buff, &depth_tex_filename);
	buff = buff.Left(buff.GetLength() - 3);
	buff += _T("inf");
	if (NULL != inf_filename)
		delete[] inf_filename;
	WideCharToMultiByte_M(buff, &inf_filename);

	m_editPath1.GetWindowText(buff);
	if (NULL != explorer_filename)
		delete[] explorer_filename;
	explorer_filename = new wchar_t[buff.GetLength()+1];
	memcpy(explorer_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
	explorer_filename[buff.GetLength()] = _T('\0');

	buff = buff.Left(buff.GetLength() - 3);
	buff += _T("anm");
	if (NULL != explorer_animation_filename)
		delete[] explorer_animation_filename;
	explorer_animation_filename = new wchar_t[buff.GetLength()+1];
	memcpy(explorer_animation_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
	explorer_animation_filename[buff.GetLength()] = _T('\0');

	buff = buff.Left(buff.GetLength() - 3);
	buff += _T("mat");
	if (NULL != material_filename)
		delete[] material_filename;
	material_filename = new wchar_t[buff.GetLength()+1];
	memcpy(material_filename, buff.GetBuffer(), buff.GetLength()*sizeof(wchar_t));
	material_filename[buff.GetLength()] = _T('\0');

	buff = buff.Left(buff.GetLength() - 3);
	buff += _T("vtx");
	if (NULL != point_filename)
		delete[] point_filename;
	point_filename = new wchar_t[buff.GetLength() + 1];
	memcpy(point_filename, buff.GetBuffer(), buff.GetLength() * sizeof(wchar_t));
	point_filename[buff.GetLength()] = _T('\0');

	pDlgParent->doOk();
}
