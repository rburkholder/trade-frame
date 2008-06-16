#pragma once

#include "resource.h"
#include "afxcmn.h"

#include "BasketTradeModel.h"

// CBasketTradeViewDialog dialog

class CBasketTradeViewDialog : public CDialog {
	DECLARE_DYNAMIC(CBasketTradeViewDialog)

public:
	CBasketTradeViewDialog(CBasketTradeModel *pModel, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasketTradeViewDialog();

// Dialog Data
	enum { IDD = IDD_DLGBASKETSYMBOLS };

protected:
	CBasketTradeModel *m_pModel;
  CListCtrl m_lcBasketSymbols;

  void HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolInfo *pInfo ); // when object instatiated in basket
  void HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolInfo *pInfo );  // when object has new data to display

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
  virtual BOOL OnInitDialog( void );

private:
};
