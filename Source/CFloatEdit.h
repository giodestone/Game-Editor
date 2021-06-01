#pragma once
#include <afxwin.h>

#include "IEventDispatcher.h"

class CFloatEdit :
    public CEdit, public IEventDispatcher
{
protected:
	void UpdateContents();

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
