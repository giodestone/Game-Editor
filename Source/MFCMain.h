#pragma once

#include <afxwin.h> 
#include <afxext.h>
#include <afx.h>
#include "pch.h"
#include "Game.h"
#include "ToolMain.h"
#include "resource.h"
#include "MFCFrame.h"
#include "SelectDialogue.h"

#include "ObjectPropertiesDialog.h"


class MFCMain : public CWinApp 
{
public:
	MFCMain();
	~MFCMain();
	BOOL InitInstance() override;
	int  Run() override;

private:

	CMyFrame * m_frame;	//handle to the frame where all our UI is
	HWND m_toolHandle;	//Handle to the MFC window
	ToolMain m_ToolSystem;	//Instance of Tool System that we interface to. 
	CRect WindowRECT;	//Window area rectangle. 
	SelectDialogue* m_ToolSelectDialogue;			//for modeless dialogue, declare it here
	ObjectPropertiesDialog* m_ObjectPropertiesDialog;

	int m_width;		
	int m_height;

	afx_msg void OnActivate(WPARAM wParam, LPARAM lParam);
	
	//Interface funtions for menu and toolbar etc requires
	afx_msg void MenuFileQuit();
	afx_msg void MenuFileSaveTerrain();
	afx_msg void MenuEditSelect();
	afx_msg	void ToolBarButton1();

	/// <summary>
	/// Callback for showing an objects' properties dialog.
	/// </summary>
	afx_msg void ShowObjectProperties();

	DECLARE_MESSAGE_MAP()	// required macro for message map functionality  One per class
};
