#include "StdAfx.h"

#include <stdexcept>

#include "ChartInstrumentTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartInstrumentTree, CTreeCtrl)

CChartInstrumentTree::CChartInstrumentTree(void)
: CTreeCtrl()
{
}

CChartInstrumentTree::~CChartInstrumentTree(void) {
}

void CChartInstrumentTree::Add(const std::string &sStrategy, const std::string &sName, CChartDataView *pDataView) {
  HTREEITEM itemStrategy;
  HTREEITEM itemName;

  // if this code chunk changes, copy into Remove method as well
  bool bStrategyFound = false;
  bool bNameFound = false;
  CString s;
  for ( HTREEITEM ix = GetNextItem( TVI_ROOT, TVGN_CHILD ); NULL != ix; ix = GetNextItem( ix, TVGN_NEXT ) ) {
    s = GetItemText( ix );
    if ( 0 == s.Compare( sStrategy.c_str() ) ) {
      bStrategyFound = true;
      itemStrategy = ix;
      for ( HTREEITEM iy = GetNextItem( itemStrategy, TVGN_CHILD ); NULL != iy; iy = GetNextItem( iy, TVGN_NEXT ) ) {
        s = GetItemText( iy );
        if ( 0 == s.Compare( sName.c_str() ) ) {
          bNameFound = true;
          itemName = iy;
          break;
        }
      }
      break;
    }
  }
  if ( bNameFound ) {
    std::string e( "Name " + sName + " already exists in " + sStrategy );
    throw std::invalid_argument( e );
  }
  if ( !bStrategyFound ) {
    itemStrategy = InsertItem( sStrategy.c_str() );
    bStrategyFound = true;
    SetItemText( itemStrategy, sStrategy.c_str() );
    SetItemData( itemStrategy, NULL );
  }
  itemName = InsertItem( sName.c_str(), itemStrategy );
  SetItemData( itemName, reinterpret_cast<DWORD_PTR>( pDataView ) );
}

void CChartInstrumentTree::Remove(const std::string &sStrategy, const std::string &sName) {

  HTREEITEM itemStrategy;
  HTREEITEM itemName;

  bool bStrategyFound = false;
  bool bNameFound = false;
  CString s;
  for ( HTREEITEM ix = GetNextItem( TVI_ROOT, TVGN_CHILD ); NULL != ix; ix = GetNextItem( ix, TVGN_NEXT ) ) {
    s = GetItemText( ix );
    if ( 0 == s.Compare( sStrategy.c_str() ) ) {
      bStrategyFound = true;
      itemStrategy = ix;
      for ( HTREEITEM iy = GetNextItem( itemStrategy, TVGN_CHILD ); NULL != iy; iy = GetNextItem( iy, TVGN_NEXT ) ) {
        s = GetItemText( iy );
        if ( 0 == s.Compare( sName.c_str() ) ) {
          bNameFound = true;
          itemName = iy;
          break;
        }
      }
      break;
    }
  }
  if ( !bNameFound ) {
    std::string e( "Name " + sName + " doesn't exist in " + sStrategy );
    throw std::invalid_argument( e );
  }
  CChartDataView *p = reinterpret_cast<CChartDataView *>( GetItemData( itemName ) );
  OnRemove( p );
  DeleteItem( itemName );
}

BEGIN_MESSAGE_MAP(CChartInstrumentTree, CTreeCtrl)
END_MESSAGE_MAP()

void CChartInstrumentTree::ProcessSelectionChanged(LPNMTREEVIEWA pNMTreeView) {
  CChartDataView *pDataView 
    = reinterpret_cast<CChartDataView *>( GetItemData( pNMTreeView->itemNew.hItem ) ) ;
  //if ( NULL != pDataView ) OnClick( pDataView );
  OnClick( pDataView );  // need to repaint a blank window when a data view disappears
}