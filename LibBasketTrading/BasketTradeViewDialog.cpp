// BasketTradeViewDialog.cpp : implementation file
//

// http://msdn.microsoft.com/en-us/library/6t3612sk(VS.80).aspx

#include "stdafx.h"
#include "BasketTradeViewDialog.h"
//#include "BasketTradeSymbolInfo.h"

// CBasketTradeViewDialog dialog

IMPLEMENT_DYNAMIC(CBasketTradeViewDialog, CDialog)

CBasketTradeViewDialog::CBasketTradeViewDialog(CBasketTradeModel *pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CBasketTradeViewDialog::IDD, pParent),
  m_pModel( pModel ), m_bSourceChanged( false )
{
  UINT id = IDD_DLGBASKETSYMBOLS;
  BOOL b = Create(id, pParent );
}

CBasketTradeViewDialog::~CBasketTradeViewDialog() {
}

BOOL CBasketTradeViewDialog::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  int ix = 0;
  m_lcBasketSymbols.InsertColumn( ix++, "Sym", LVCFMT_LEFT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Current", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "High", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "OpnHi", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Open", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "OpnLo", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Low", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Filled", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Stop", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "UnrelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "RelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Hit", LVCFMT_CENTER, 50 );

  m_refresh.OnRefresh.Add( MakeDelegate( this, &CBasketTradeViewDialog::HandlePeriodicRefresh ) );
  m_pModel->OnBasketTradeSymbolInfoAddedToBasket.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded ) );

  return TRUE;
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolInfo *pInfo ) {
  int ix = m_mapDialogEntry.size();
  structDialogEntry entry( ix, pInfo );
  m_mapDialogEntry.insert( mapDialogEntry_t( pInfo->GetDialogFields().sSymbolName, entry ) );
  m_lcBasketSymbols.InsertItem( ix, pInfo->GetDialogFields().sSymbolName.c_str() );
  pInfo->OnBasketTradeSymbolInfoChanged.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged ) );
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolInfo *pInfo ) {
  std::map<std::string, structDialogEntry>::iterator iter
    = m_mapDialogEntry.find( pInfo->GetSymbolName() );
  if ( m_mapDialogEntry.end() != iter ) {
    iter->second.bChanged = true;
    m_bSourceChanged = true;
  }
}

void CBasketTradeViewDialog::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pRefresh ) {
  if ( m_bSourceChanged ) {
    for ( std::map<std::string, structDialogEntry>::iterator iter = m_mapDialogEntry.begin();
      iter != m_mapDialogEntry.end(); ++iter ) {
        if ( iter->second.bChanged ) {
          iter->second.bChanged = false;
          const CBasketTradeSymbolInfo::structFieldsForDialog &flds = iter->second.pInfo->GetDialogFields();
          int ix = iter->second.ix;
          //m_lcBasketSymbols.SetItemText( ix, 1, 
        }
    }
    m_bSourceChanged = false;
  }
}

void CBasketTradeViewDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LISTBASKETSYMBOLS, m_lcBasketSymbols);
}


BEGIN_MESSAGE_MAP(CBasketTradeViewDialog, CDialog)
END_MESSAGE_MAP()


// CBasketTradeViewDialog message handlers
