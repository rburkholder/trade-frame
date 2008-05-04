#include "stdafx.h"
#include "HScrollListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHScrollListBox, CListBox)
	//{{AFX_MSG_MAP(CHScrollListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_ADDSTRING, OnAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
	ON_MESSAGE(LB_DELETESTRING, OnDeleteString)
	ON_MESSAGE(LB_RESETCONTENT, OnResetContent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox
/////////////////////////////////////////////////////////////////////////////
CHScrollListBox::CHScrollListBox()
{
}

CHScrollListBox::~CHScrollListBox()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox message handlers
///////////////////////////////////////////////////////////////////////////////
int CHScrollListBox::GetTextLen(LPCTSTR lpszText)
{
	ASSERT(AfxIsValidString(lpszText));

	CDC *pDC = GetDC();
    ASSERT(pDC);
	CSize size;

    if ( pDC )
	{
		CFont* pOldFont = pDC->SelectObject(GetFont());
		if ( pOldFont ) 
		{
			size = pDC->GetTextExtent(lpszText, (int) _tcslen(lpszText));
			pDC->SelectObject(pOldFont);
		}
		ReleaseDC(pDC);
	}

	return size.cx +5; // add some whitespace
}

void CHScrollListBox::ResetHExtent()
{
	if (GetCount() == 0)
	{
		SetHorizontalExtent(0);
		return;
	}

	CWaitCursor     WaitCursor;
	int             iMaxHExtent = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		CString csText;
		GetText(i, csText);
		if ( int iExt = GetTextLen(csText) > iMaxHExtent )
			iMaxHExtent = iExt;
	}

	SetHorizontalExtent(iMaxHExtent);
}

void CHScrollListBox::SetNewHExtent(LPCTSTR lpszNewString)
{
    int iExt = GetTextLen(lpszNewString);
	if ( iExt > GetHorizontalExtent())
		SetHorizontalExtent(iExt);
}

// Event handlers
LRESULT CHScrollListBox::OnAddString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if ( !((lResult == LB_ERR) || (lResult == LB_ERRSPACE)) )
		SetNewHExtent((LPCTSTR) lParam);

	return lResult;
}

LRESULT CHScrollListBox::OnInsertString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if ( !((lResult == LB_ERR) || (lResult == LB_ERRSPACE)) )
		SetNewHExtent((LPCTSTR) lParam);

	return lResult;
}

LRESULT CHScrollListBox::OnDeleteString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
    if ( !((lResult == LB_ERR) || (lResult == LB_ERRSPACE)) ) 
		ResetHExtent();

	return lResult;
}

LRESULT CHScrollListBox::OnResetContent(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	SetHorizontalExtent(0);

	return lResult;
}

