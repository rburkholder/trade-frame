#pragma once

#include "resource.h"
#include "afxcmn.h"

#include "GeneratePeriodicRefresh.h"
#include "BasketTradeModel.h"
#include "TimeSource.h"

#include <sstream>
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

  void HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolBase *pInfo ); // when object instatiated in basket
  void HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolBase *p );  // when object has new data to display

  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh* );

  struct structDialogEntry {
    int ix; // index in dialog box
    bool bChanged;
    CBasketTradeSymbolBase *pInfo;  // try not to use this
    CBasketTradeSymbolBase::structFieldsForDialog *pFields;
    structDialogEntry( int ix_, CBasketTradeSymbolBase *pInfo_,  CBasketTradeSymbolBase::structFieldsForDialog *pFields_ ) 
      : ix( ix_ ), bChanged( false ), pInfo( pInfo_ ), pFields( pFields_ ) {};
    structDialogEntry( int ix_, CBasketTradeSymbolBase::structFieldsForDialog *pFields_ ) 
      : ix( ix_ ), bChanged( false ), pInfo( NULL ), pFields( pFields_ ) {};
  };
  typedef std::pair<string, structDialogEntry> mapDialogEntry_t;
  std::map<std::string, structDialogEntry> m_mapDialogEntry;
  bool m_bSourceChanged;

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
  virtual BOOL OnInitDialog( void );

  afx_msg void OnSize( UINT, int, int );
  bool bDialogReady;

  CBasketTradeSymbolBase::structFieldsForDialog m_Totals;

  //CTimeSource m_ts;

  std::stringstream m_ssDateTime;


private:
};
