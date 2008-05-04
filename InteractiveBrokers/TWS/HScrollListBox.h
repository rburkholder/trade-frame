#if !defined(CHSCROLLLISTBOX_H)
#define CHSCROLLLISTBOX_H

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox window
/////////////////////////////////////////////////////////////////////////////
class CHScrollListBox : public CListBox
{
// Construction
public:
	CHScrollListBox();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHScrollListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHScrollListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHScrollListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetContent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void    ResetHExtent();
	void    SetNewHExtent(LPCTSTR lpszNewString);
	int     GetTextLen(LPCTSTR lpszText);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(CHSCROLLLISTBOX_H)
