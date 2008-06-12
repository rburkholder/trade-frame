// BasketTradeViewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BasketTradeViewDialog.h"


// CBasketTradeViewDialog dialog

IMPLEMENT_DYNAMIC(CBasketTradeViewDialog, CDialog)

CBasketTradeViewDialog::CBasketTradeViewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBasketTradeViewDialog::IDD, pParent) {

}

CBasketTradeViewDialog::~CBasketTradeViewDialog() {
}

BOOL CBasketTradeViewDialog::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  m_lcBasketSymbols.InsertColumn(  0, "Sym", LVCFMT_LEFT, 50 );
  m_lcBasketSymbols.InsertColumn(  1, "High", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  2, "OpnHi", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  3, "Open", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  4, "OpnLo", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  5, "Low", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  6, "Filled", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  7, "Current", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  8, "Stop", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn(  9, "UnrelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( 10, "RelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( 11, "Hit", LVCFMT_CENTER, 50 );

  return TRUE;
}

void CBasketTradeViewDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LISTBASKETSYMBOLS, m_lcBasketSymbols);
}


BEGIN_MESSAGE_MAP(CBasketTradeViewDialog, CDialog)
END_MESSAGE_MAP()


// CBasketTradeViewDialog message handlers
