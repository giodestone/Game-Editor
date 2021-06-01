#include "MFCFrame.h"

#include <sstream>

#include "resource.h"
#include "ToolMain.h"

#include "COleVariantHelpers.h"


BEGIN_MESSAGE_MAP(CMyFrame, CFrameWnd)
	
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
	ON_WM_ACTIVATE(&OnActivate)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_TOOL
};
//FRAME CLASS


//frame initialiser
CMyFrame::CMyFrame()
	: m_currentSelection(nullptr)
{
}

void CMyFrame::SetCurrentSelection(SceneObject* currentSelection)
{
	this->m_currentSelection = currentSelection;
}

void CMyFrame::OnUpdatePage(CCmdUI * pCmdUI)
{
	//pCmdUI->Enable();
	//pCmdUI->SetText(strPage);
}

void CMyFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (!ToolMain::IsInitialised())
		return;

	ToolMain::GetInstance()->SetMainWindowReference(this);
	
	switch (nState)
	{
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		ToolMain::GetInstance()->OnMainWindowRegainFocus();
		break;
	case WA_INACTIVE:
		ToolMain::GetInstance()->OnMainWindowLostFocus();
		break;
	}

	// on active or clickalive mark as focused on main pane; on inactive mark as inactive.
}

//oncretae, called after init but before window is shown. 
int CMyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// create a view to occupy the client area of the frame //This is where DirectX is rendered
	if (!m_DirXView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_menu1.LoadMenuW(IDR_MENU1);
	SetMenu(&m_menu1);
	
	if (!m_toolBar.CreateEx(this, TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	CRect rect;
	GetClientRect(&rect);
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_TOOL, SBPS_NORMAL, rect.Width() - 500);//set width of status bar panel

	ToolMain::SetMainWindowReference(this);
	
	return 0;
}

LRESULT CMyFrame::OnShowWindow(BOOL shouldShow, UINT status)
{
	return 0;
}
