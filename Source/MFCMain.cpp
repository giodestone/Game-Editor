#include "MFCMain.h"

#include <afxshellmanager.h>
#include <afxvisualmanagerwindows.h>

#include "resource.h"


BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT, &MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::MenuFileSaveTerrain)
	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_BUTTON40001, &MFCMain::ToolBarButton1)
	ON_COMMAND(ID_WINDOW_SHOWOBJECTPROPERTIES, &MFCMain::ShowObjectProperties)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CShellManager* pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}
	
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(	NULL,
					_T("World Of Flim-Flam Craft Editor"),
					WS_OVERLAPPEDWINDOW,
					CRect(100, 100, 1024, 768),
					NULL,
					NULL,
					0,
					NULL
				);

	//show and set the window to run and update. 
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();


	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView.GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView.GetClientRect(&WindowRECT);
	m_width		= WindowRECT.Width();
	m_height	= WindowRECT.Height();

	m_ToolSystem.OnActionInitialise(m_toolHandle, m_width, m_height);
	
	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ToolSystem.UpdateInput(&msg);
		}
		else
		{	
			int ID = m_ToolSystem.GetClosestSelectedSceneObjectID();
			std::wstring statusString;
			if (ID == Selection::NothingSelectedID)
				statusString += L"No Object Selected.";
			else
				statusString += L"Selected Object ID: " + std::to_wstring(ID);
			
			m_ToolSystem.Tick(&msg);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);	
		}
	}

	return (int)msg.wParam;
}

void MFCMain::OnActivate(WPARAM wParam, LPARAM lParam)
{                      
	

}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
	m_ToolSystem.OnActionSaveTerrain();
}

void MFCMain::MenuEditSelect()
{
	//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
	//m_ToolSelectDialogue.DoModal();	// start it up modal

	//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
	if (m_ToolSelectDialogue != nullptr)
	{
		delete m_ToolSelectDialogue;
		m_ToolSelectDialogue = nullptr;
	}

	m_ToolSelectDialogue = new SelectDialogue(GetMainWnd());
	m_ToolSelectDialogue->Create(IDD_DIALOG1);	//Start up modeless
	m_ToolSelectDialogue->ShowWindow(SW_SHOW);	//show modeless
}

void MFCMain::ToolBarButton1()
{
	m_ToolSystem.OnActionSave();
}

void MFCMain::ShowObjectProperties()
{
	if (m_ObjectPropertiesDialog != nullptr)
	{
		delete m_ObjectPropertiesDialog;
		m_ObjectPropertiesDialog = nullptr;
	}

	m_ObjectPropertiesDialog = new ObjectPropertiesDialog(GetMainWnd());
	m_ObjectPropertiesDialog->SetReferenceToToolMain(&m_ToolSystem);
	m_ObjectPropertiesDialog->Create(IDD_OBJECTPROPERTIESDIALOG);
	m_ObjectPropertiesDialog->ShowWindow(SW_SHOW);
	m_ObjectPropertiesDialog->Initialize();
}


MFCMain::MFCMain()
{
}


MFCMain::~MFCMain()
{
}
