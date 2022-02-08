// VuKeyValues.cpp : implementation file
//

#include "stdafx.h"
#include "GTScalp.h"
#include "VuKeyValues.h"

#include <iostream>
using namespace std;

// CKeyValues dialog

IMPLEMENT_DYNAMIC(CVuKeyValues, CDialog)

CVuKeyValues::CVuKeyValues(CWnd* pParent /*=NULL*/) 
	: CDialog(CVuKeyValues::IDD, pParent) {

    bDialogReady = false;

    BOOL b = Create(IDD, pParent );
  }

CVuKeyValues::~CVuKeyValues() {
  DestroyWindow();
}

BOOL CVuKeyValues::OnInitDialog() {
  
  BOOL b = CDialog::OnInitDialog();

  kv1.InitControl();
  
  CRect rect1, rect2;
  GetClientRect( &rect1 );
  rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
  kv1.MoveWindow( &rect2 );

  kv1.DrawColumns();

  bDialogReady = true;

  return b;
}

afx_msg void CVuKeyValues::OnSize( UINT i, int x, int y ) {
  if ( bDialogReady ) {
    CRect rect1, rect2;
    GetClientRect( &rect1 );
    rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
    kv1.MoveWindow( &rect2 );
  }
}

void CVuKeyValues::WatchSymbol( const string &sSymbol, CRowKeyValues *row ) {

  IQFeedSymbol *pSym;

  pSym = m_IQFeedProvider.GetIQFeedProvider()->GetSymbol( sSymbol );
  //pSym = theApp.m_pIQFeed->Attach( sSymbol );
  pSym->OnFundamentalMessage.Add( MakeDelegate( row, &CRowKeyValues::HandleSymbolFundamental ) );
  pSym->OnSummaryMessage.Add( MakeDelegate( row, &CRowKeyValues::HandleSymbolSummary ) );
  pSym->OnUpdateMessage.Add( MakeDelegate( row, &CRowKeyValues::HandleSymbolUpdate ) );
  //theApp.m_pIQFeed->Watch( sSymbol );
  m_IQFeedProvider.GetIQFeedProvider()->StartQuoteTradeWatch( pSym );
}

void CVuKeyValues::UnWatchSymbol( const string &sSymbol, CRowKeyValues *row ) {

  IQFeedSymbol *pSym;

  pSym = m_IQFeedProvider.GetIQFeedProvider()->GetSymbol( sSymbol );
  //pSym = theApp.m_pIQFeed->Attach( sSymbol );
  pSym->OnFundamentalMessage.Remove( MakeDelegate( row, &CRowKeyValues::HandleSymbolFundamental ) );
  pSym->OnSummaryMessage.Remove( MakeDelegate( row, &CRowKeyValues::HandleSymbolSummary ) );
  pSym->OnUpdateMessage.Remove( MakeDelegate( row, &CRowKeyValues::HandleSymbolUpdate ) );
  //theApp.m_pIQFeed->Watch( sSymbol );
  m_IQFeedProvider.GetIQFeedProvider()->StopQuoteTradeWatch( pSym );
}

CRowKeyValues *CVuKeyValues::AppendSymbol( const string &sName ) {
  return kv1.InsertSymbol( sName.c_str() );
}

afx_msg void CVuKeyValues::fn2( NMHDR * nm, LRESULT * pResult ) {
  if ( 0 == nm ) {
  }
}

afx_msg void CVuKeyValues::fn3( NMHDR * nm, LRESULT * pResult ) {
  LPNMLVGETINFOTIP tip1;
  tip1 = (LPNMLVGETINFOTIP) nm; 
  strcpy_s( tip1->pszText, tip1->cchTextMax, kv1.GetFullName( tip1->iItem ) );
}

afx_msg void CVuKeyValues::fn4( NMHDR * nm, LRESULT * pResult ) {
  LPNMITEMACTIVATE active = (LPNMITEMACTIVATE) nm;
}


void CVuKeyValues::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_KEYVALUES, kv1);
}

afx_msg void CVuKeyValues::OnLButtonUp( UINT, CPoint ) {
}

afx_msg void CVuKeyValues::OnLButtonDown( UINT, CPoint ) {
}

BEGIN_MESSAGE_MAP(CVuKeyValues, CDialog)
//  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, memberFxn )
//  ON_NOTIFY_EX( LVN_GETINFOTIP, IDD_VUKEYVALUES, fn2 )
//  ON_NOTIFY_EX( NM_HOVER, IDD_VUKEYVALUES, fn2 )
//  ON_NOTIFY_EX( LVN_KEYDOWN, IDD_VUKEYVALUES, fn2 )
//  ON_NOTIFY_EX( LVN_GETINFOTIP, IDC_KEYVALUES, fn2 )
//  ON_NOTIFY_EX( NM_HOVER, IDC_KEYVALUES, fn2 )
//  ON_NOTIFY_EX( LVN_KEYDOWN, IDC_KEYVALUES, fn2 )
  ON_NOTIFY( NM_HOVER, IDC_KEYVALUES, fn2 )
  ON_NOTIFY( LVN_GETINFOTIP, IDC_KEYVALUES, fn3 )
  ON_NOTIFY( LVN_ITEMACTIVATE, IDC_KEYVALUES, fn4 )
//  ON_NOTIFY( NM_HOVER, IDD_VUKEYVALUES, fn )
//  ON_NOTIFY( LVN_GETINFOTIP, IDD_VUKEYVALUES, fn )
  ON_WM_LBUTTONUP( )
  ON_WM_LBUTTONDOWN( )
  ON_WM_SIZE()
  //ON_NOTIFY(HDN_DIVIDERDBLCLICK, 0, &CVuKeyValues::OnHdnDividerdblclickKeyvalues)
  //ON_NOTIFY(HDN_ITEMCHANGED, 0, &CVuKeyValues::OnHdnItemchangedKeyvalues)
  ON_NOTIFY(HDN_ENDTRACK, 0, &CVuKeyValues::OnHdnEndtrackKeyvalues)
  ON_NOTIFY(NM_CUSTOMDRAW, IDC_KEYVALUES, &CVuKeyValues::OnNMCustomdrawKeyvalues)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYVALUES, &CVuKeyValues::OnLvnItemchangedKeyvalues)
END_MESSAGE_MAP()

// CKeyValues message handlers

void CVuKeyValues::OnHdnDividerdblclickKeyvalues(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
  CString s;
  s.Format( "col %d is %d", phdr->iItem, phdr->pitem->cxy );
  *pResult = 0;
}

void CVuKeyValues::OnHdnItemchangedKeyvalues(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
  
  *pResult = 0;
}

void CVuKeyValues::OnHdnEndtrackKeyvalues(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
  CString s;
  s.Format( "col %d is %d", phdr->iItem, phdr->pitem->cxy );
  //theApp.pConsoleMessages->WriteLine( s );
  cout << s << endl;
  *pResult = 0;
}

void CVuKeyValues::OnNMCustomdrawKeyvalues(NMHDR *pNMHDR, LRESULT *pResult)
{
  //LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  //LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
  kv1.OnNMCustomdrawKeyvalues( pNMHDR, pResult );

  //*pResult = 0;
}

void CVuKeyValues::OnLvnItemchangedKeyvalues(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  // TODO: Add your control notification handler code here
  *pResult = 0;
}
