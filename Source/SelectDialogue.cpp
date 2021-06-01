// SelectDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "SelectDialogue.h"

#include "ToolMain.h"

// SelectDialogue dialog

IMPLEMENT_DYNAMIC(SelectDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(SelectDialogue, CDialogEx)
	ON_BN_CLICKED(IDOK, &SelectDialogue::OnBnClickedOk)		
	ON_LBN_SELCHANGE(IDC_LIST1, &SelectDialogue::Select)	//listbox
END_MESSAGE_MAP()


SelectDialogue::SelectDialogue(CWnd* pParent, void* defineToMakeModal)		//constructor used in modal
	: CDialogEx(IDD_DIALOG1, pParent)
{
}

SelectDialogue::SelectDialogue(CWnd * pParent)			//constructor used in modeless
	: CDialogEx(IDD_DIALOG1, pParent)
{
}

SelectDialogue::~SelectDialogue()
{
}


void SelectDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
}

void SelectDialogue::Select()
{
	int index = m_listBox.GetCurSel();
	CString currentSelectionValue;
	
	m_listBox.GetText(index, currentSelectionValue);

	auto currentSelection = _ttoi(currentSelectionValue);
	ToolMain::GetInstance()->GetSelection().SetCurrentlySelected(currentSelection);

	auto objPropertiesDialog = ToolMain::GetInstance()->GetObjectPropertiesDialog();
	if (objPropertiesDialog != nullptr)
		objPropertiesDialog->SetCurrentSceneObject(ToolMain::GetInstance()->GetSelection().GetNearestSelectedSceneObject());
}

BOOL SelectDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	auto toolMain = ToolMain::GetInstance();

	// Add IDs to the select box.
	for (auto& p : ToolMain::GetInstance()->GetSceneGraph())
	{
		std::wstring listBoxEntry = std::to_wstring(p.first);
		m_listBox.AddString(listBoxEntry.c_str());
	}
	
	return TRUE;
}

void SelectDialogue::PostNcDestroy()
{
}




// SelectDialogue message handlers callback   - We only need this if the dailogue is being setup-with createDialogue().  We are doing
//it manually so its better to use the messagemap
/*INT_PTR CALLBACK SelectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		//	EndDialog(hwndDlg, wParam);
			DestroyWindow(hwndDlg);
			return TRUE;
			

		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;
		}
	}
	
	return INT_PTR();
}*/


void SelectDialogue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

