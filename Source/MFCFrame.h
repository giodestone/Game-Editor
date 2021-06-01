#pragma once
#include <afxwin.h> 
#include <afxext.h>
#include "MFCRenderFrame.h"
#include "SceneObject.h"



class CMyFrame : public CFrameWnd
{
protected:
//	DECLARE_DYNAMIC(CMainFrame)
	class ToolMain* m_toolMain;

public:

	CMenu			m_menu1;	//menu bar
	CStatusBar		m_wndStatusBar;
	CToolBar		m_toolBar;
	CChildRender	m_DirXView;

public:
	CMyFrame();
	void SetCurrentSelection(SceneObject* currentSelection);
	afx_msg void OnUpdatePage(CCmdUI *pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

private:	//overrides
	SceneObject* m_currentSelection;

	const CString nothingSelectedText = "No Object Selected.";

	//note the afx_message keyword is linking this method to message map access.
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnShowWindow(BOOL shouldShow, UINT status);
	DECLARE_MESSAGE_MAP()	// required macro for message map functionality  One per class
};