#include "StdAfx.h"
#include "DataRow.h"
#include <math.h>

CDataRow::CDataRow(unsigned int ixPrice, double Price ) { 

  char fmtDouble[] = "%0.2f";
  char fmtInt[] = "%d";
  char fmtPrice[] = "%d@%s";
  char fmtString[] = "%s";
  char szUnadornedPrice[ 40 ];
  _snprintf( szUnadornedPrice, 40, fmtDouble, Price );

  m_pAcct1PL = new CDataRowElement<double>( &bChanged, fmtDouble );
  m_pAcct2PL = new CDataRowElement<double>( &bChanged, fmtDouble );
  m_pAcctPL = new CDataRowElement<double>( &bChanged, fmtDouble );
  m_pAcct1BidPending = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pAcct2BidPending = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pBidQuan = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pPrice = new CDataRowElementPrice( &bChanged, fmtInt, szUnadornedPrice );
  m_pAskQuan = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pAcct1AskPending = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pAcct2AskPending = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pTicks = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pVolume = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pTickBuyVolume = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pTickSellVolume = new CDataRowElement<int>( &bChanged, fmtInt );
  m_pStaticIndicators = new CDataRowElementStaticIndicator( &bChanged, fmtString );
  m_pDynamicIndicators = new CDataRowElementDynamicIndicator( &bChanged, fmtString );

  PriceIndex = ixPrice;
  m_dblPrice = Price;

  bChanged = true;
}

CDataRow::~CDataRow(void) {
  delete m_pAcct1PL;
  delete m_pAcct2PL;
  delete m_pAcctPL;
  delete m_pAcct1BidPending;
  delete m_pAcct2BidPending;
  delete m_pBidQuan;
  delete m_pPrice;
  delete m_pAskQuan;
  delete m_pAcct1AskPending;
  delete m_pAcct2AskPending;
  delete m_pTicks;
  delete m_pVolume;
  delete m_pTickBuyVolume;
  delete m_pTickSellVolume;
  delete m_pStaticIndicators;
  delete m_pDynamicIndicators;
}

unsigned int CDataRow::DoubleToInt(double val, int multiplier) {
  return 
    (int) floor((val * multiplier)+0.5);
}

double CDataRow::IntToDouble( unsigned int val, int multiplier ) {
  return (double) val / (double) multiplier;
}

void CDataRow::SetMapToVisibleRow( CVisibleRow *pvr ) {
  m_pvr = pvr;
  m_pAcct1PL->SetVisibleElement( &pvr->m_veAcct1PL );
  m_pAcct2PL->SetVisibleElement( &pvr->m_veAcct2PL );
  m_pAcctPL->SetVisibleElement( &pvr->m_veAcctPL );
  m_pAcct1BidPending->SetVisibleElement( &pvr->m_veAcct1BidPending );
  m_pAcct2BidPending->SetVisibleElement( &pvr->m_veAcct2BidPending );
  m_pBidQuan->SetVisibleElement( &pvr->m_veBidQuan );
  m_pPrice->SetVisibleElement( &pvr->m_vePrice );
  m_pAskQuan->SetVisibleElement( &pvr->m_veAskQuan );
  m_pAcct1AskPending->SetVisibleElement( &pvr->m_veAcct1AskPending );
  m_pAcct2AskPending->SetVisibleElement( &pvr->m_veAcct2AskPending );
  m_pTicks->SetVisibleElement( &pvr->m_veTicks );
  m_pVolume->SetVisibleElement( &pvr->m_veVolume );
  m_pTickBuyVolume->SetVisibleElement( &pvr->m_veTickBuyVolume );
  m_pTickSellVolume->SetVisibleElement( &pvr->m_veTickSellVolume );
  m_pStaticIndicators->SetVisibleElement( &pvr->m_veStaticIndicators );
  m_pDynamicIndicators->SetVisibleElement( &pvr->m_veDynamicIndicators );
}

void CDataRow::UnsetMapToVisibleRow( void ) {
  m_pvr = NULL;
  m_pAcct1PL->SetVisibleElement( NULL );
  m_pAcct2PL->SetVisibleElement( NULL );
  m_pAcctPL->SetVisibleElement( NULL );
  m_pAcct1BidPending->SetVisibleElement( NULL );
  m_pAcct2BidPending->SetVisibleElement( NULL );
  m_pBidQuan->SetVisibleElement( NULL );
  m_pPrice->SetVisibleElement( NULL );
  m_pAskQuan->SetVisibleElement( NULL );
  m_pAcct1AskPending->SetVisibleElement( NULL );
  m_pAcct2AskPending->SetVisibleElement( NULL );
  m_pTicks->SetVisibleElement( NULL );
  m_pVolume->SetVisibleElement( NULL );
  m_pTickBuyVolume->SetVisibleElement( NULL );
  m_pTickSellVolume->SetVisibleElement( NULL );
  m_pStaticIndicators->SetVisibleElement( NULL );
  m_pDynamicIndicators->SetVisibleElement( NULL );
}

void CDataRow::Redraw() {

  if ( NULL != m_pvr ) {
    m_pAcct1PL->UpdateVisibleElement();
    m_pAcct2PL->UpdateVisibleElement();
    m_pAcctPL->UpdateVisibleElement();
    m_pAcct1BidPending->UpdateVisibleElement();
    m_pAcct2BidPending->UpdateVisibleElement();
    m_pBidQuan->UpdateVisibleElement();
    m_pPrice->UpdateVisibleElement();
    m_pAskQuan->UpdateVisibleElement();
    m_pAcct1AskPending->UpdateVisibleElement();
    m_pAcct2AskPending->UpdateVisibleElement();
    m_pTicks->UpdateVisibleElement();
    m_pVolume->UpdateVisibleElement();
    m_pTickBuyVolume->UpdateVisibleElement();
    m_pTickSellVolume->UpdateVisibleElement();
    m_pStaticIndicators->UpdateVisibleElement();
    m_pDynamicIndicators->UpdateVisibleElement();
  }

}

void CDataRow::CheckRefresh() {
  if ( bChanged ) {
    bChanged = false;  // before or after Redraw()?
    Redraw();
  }
}

void CDataRow::HandleAccount1BidRClick() {
  if ( NULL != OnAccount1BidRClick ) OnAccount1BidRClick( PriceIndex );
}

void CDataRow::HandleAccount2BidRClick() {
  if ( NULL != OnAccount2BidRClick ) OnAccount2BidRClick( PriceIndex );
}

void CDataRow::HandleAccount1AskRClick() {
  if ( NULL != OnAccount1AskRClick ) OnAccount1AskRClick( PriceIndex );
}

void CDataRow::HandleAccount2AskRClick() {
  if ( NULL != OnAccount2AskRClick ) OnAccount2AskRClick( PriceIndex );
}

void CDataRow::HandleAccount1BidLClick() {
  if ( NULL != OnAccount1BidLClick ) OnAccount1BidLClick( PriceIndex );
}

void CDataRow::HandleAccount2BidLClick() {
  if ( NULL != OnAccount2BidLClick ) OnAccount2BidLClick( PriceIndex );
}

void CDataRow::HandleAccount1AskLClick() {
  if ( NULL != OnAccount1AskLClick ) OnAccount1AskLClick( PriceIndex );
}

void CDataRow::HandleAccount2AskLClick() {
  if ( NULL != OnAccount2AskLClick ) OnAccount2AskLClick( PriceIndex );
}

