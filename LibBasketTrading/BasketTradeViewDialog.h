#pragma once

#include "resource.h"
#include "afxcmn.h"

// CBasketTradeViewDialog dialog

class CBasketTradeViewDialog : public CDialog {
	DECLARE_DYNAMIC(CBasketTradeViewDialog)

public:
	CBasketTradeViewDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasketTradeViewDialog();

// Dialog Data
	enum { IDD = IDD_DLGBASKETSYMBOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CListCtrl m_lcBasketSymbols;

protected:
  virtual BOOL OnInitDialog( void );
};
