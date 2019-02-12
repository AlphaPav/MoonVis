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

#pragma once
#include "afxcmn.h"

#include "DlgWizzardWelcom.h"
#include "DlgWizzardCreate.h"
#include "DlgWizzardLoad.h"

// CWizzardDlg dialog

class CDlgWizzard : public CDialog
{
	DECLARE_DYNAMIC(CDlgWizzard)

public:
	enum LunchState
	{
		LUNCH_STATE_CREATE,
		LUNCH_STATE_LOAD,
		LUNCH_STATE_NONE
	};

public:
	CDlgWizzard(CString strLoadPath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWizzard();

	void doOk()
	{
		OnOK();
	}

// Dialog Data
	enum { IDD = IDD_DLG_WIZZARD };

public:
	// UI
	CDlgWizzardWelcom m_dlgWelcome;
	CDlgWizzardCreate m_dlgCreate;
	CDlgWizzardLoad m_dlgLoad;

	// data
	LunchState m_lunchState;
	CString m_strLoadPath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCreate();
	afx_msg BOOL OnInitDialog();
	
public:
	void hideWindow(void);
};
