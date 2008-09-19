#pragma once

#include "resource.h"
#include "afxcmn.h"

#include "GeneratePeriodicRefresh.h"
#include "BasketTradeModel.h"

#include <map>
#include <queue>
#include "afxwin.h"

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
  CStatic m_lblDateTime;

  void HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolInfo *pInfo ); // when object instatiated in basket
  void HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolInfo *pInfo );  // when object has new data to display

  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh* );

  struct structDialogEntry {
    int ix; // index in dialog box
    bool bChanged;
    CBasketTradeSymbolInfo *pInfo;
    structDialogEntry( int ix_, CBasketTradeSymbolInfo *pInfo_ ) 
      : ix( ix_ ), bChanged( false ), pInfo( pInfo_ ) {};
  };
  typedef std::pair<string, structDialogEntry> mapDialogEntry_t;
  std::map<std::string, structDialogEntry> m_mapDialogEntry;
  bool m_bSourceChanged;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
  virtual BOOL OnInitDialog( void );

  afx_msg void OnSize( UINT, int, int );
  bool bDialogReady;


private:
};
