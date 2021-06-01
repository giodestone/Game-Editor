#include "CFloatEdit.h"

#include <string>

#include "ToolMain.h"

BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
END_MESSAGE_MAP()


void CFloatEdit::UpdateContents()
{
	CString contents;
	GetWindowTextW(contents);

	std::wstring c = (LPCTSTR)contents;
	
	try
	{
		auto finalValue = std::stof(c);
		SetWindowTextW(CString(std::to_wstring(finalValue).c_str()));
		DispatchEvent(); // Value has been changed, update.
	}
	catch (...)
	{
		SetWindowTextW(contents);
	}
}


void CFloatEdit::OnKillFocus(CWnd* pNewWnd)
{
	UpdateContents();

	CEdit::OnKillFocus(pNewWnd);
}

void CFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Stop sound when return or tab are pressed.
	if (nChar == VK_RETURN || nChar == VK_TAB)
	{
		UpdateContents();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
