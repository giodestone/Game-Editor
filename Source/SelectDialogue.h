#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

// SelectDialogue dialog

class SelectDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(SelectDialogue)

public:
	SelectDialogue(CWnd* pParent, void* defineToMakeModal);   // modal // takes in out scenegraph in the constructor
	SelectDialogue(CWnd* pParent = NULL);
	virtual ~SelectDialogue();
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void Select();	//Item has been selected

	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	CListBox m_listBox;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeList1();
};


INT_PTR CALLBACK SelectProc( HWND   hwndDlg,UINT   uMsg,WPARAM wParam,LPARAM lParam);