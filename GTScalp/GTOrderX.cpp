#include "StdAfx.h"
//#include "GTScalp.h"
#include "GTOrderX.h"

CGTOrderX::CGTOrderX(const CGTStockX *stock, EOrderType ot, EOrderSide os, long cntShares) {
  //GTOrder::bArcaAutoRoute = d.bArcaAutoRoute;
  //GTOrder::bIsldAutoRoute = d.bIsldAutoRoute;
  //GTOrder::bPegCap = d.bPegCap;
  //(GTOrder) *this = d;
  //memcpy( this, st, sizeof( GTOrder ) );
  memcpy( (GTOrder*) this, &stock->m_defOrder, sizeof( GTOrder ) );
  //GTOrder::szAccountID = stock->m_session.m_account.szAccountID;
  //GTOrder::szAccountID = theApp.m_session1.m_account.szAccountID;
  GetSystemTime( &stCreated );
  dwTimeInForce = TIF_DAY;
  method = METHOD_ISLD;
  place = MMID_ISLD;
  lpUserData = this;
  dwShare = cntShares;
  bIsldAutoRoute = 1;
  switch ( ot ) {
    case EOrderType::Limit:
      chPriceIndicator = PRICE_INDICATOR_LIMIT;
      break;
    case EOrderType::LimitStop:
      chPriceIndicator = PRICE_INDICATOR_STOPLIMIT;
      break;
    case EOrderType::Market:
      chPriceIndicator = PRICE_INDICATOR_MARKET;
      break;
    case EOrderType::Stop:
      chPriceIndicator = PRICE_INDICATOR_STOP;
      break;
  }
  switch ( os ) {
    case EOrderSide::Buy:
      chSide = 'B';
      break;
    case EOrderSide::Sell:
      chSide = 'S';
      break;
    case EOrderSide::Short: 
      chSide = 'S';
      break;
  }
}

CGTOrderX::~CGTOrderX(void) {
}

void CGTOrderX::OnSendingOrder(const GTSending &gtsending) {
  dwTraderSeqNo = gtsending.dwTraderSeqNo;
  if ( NULL != OnOrderSent ) {
    CString sId;  sId.Format( "%d", dwTraderSeqNo );
    CString sTime; sTime.Format( "%06x", gtsending.nEntryTime );
    CString sSide; sSide.Format( "%c", gtsending.chEntrySide );
    CString sQuan; sQuan.Format( "%d", gtsending.nEntryShares );
    CString sPrice; sPrice.Format( "%0.2f", gtsending.dblEntryPrice );

    OnOrderSent( (LPCSTR) sId, (LPCSTR) sTime, (LPCSTR) sSide, 
      (LPCSTR) sQuan, (LPCSTR) sPrice, _T( "Sent" ), (DWORD_PTR) this );
  }
}

/*
void CGTOrderX::OnErrorMessage( const GTErrMsg &gterrmsg ) {
  CString sId;
  gterrmsg.
  sId.Format( "%d", gterrmsg.dwTraderSeqNo );
  if ( NULL != OnUpdateOrderStatus ) OnUpdateOrderStatus( (LPCTSTR) sId, gterrmsg.szText );
}
*/

void CGTOrderX::OnTraded( const GTTrade &trade ) {
  CString sId;
  sId.Format( "%d", trade.dwTraderSeqNo );
  if ( 0 == trade.nExecRemainShares ) {
    if ( NULL != OnOrderComplete ) {
      OnOrderComplete( sId );
    }
  }
  else {
    if ( NULL != OnUpdateOrderStatus ) {
      OnUpdateOrderStatus( sId, "Partial" );
    }
  }
}

void CGTOrderX::OnCancelled( const GTCancel &cancel ) {
  // at some time, leave on screen and need manual delete for confirmation
  CString sId;
  sId.Format( "%d", cancel.dwTraderSeqNo );
  if ( NULL != OnOrderComplete ) {
    OnOrderComplete( (LPCSTR) sId );
  }
}

void CGTOrderX::OnRejected( const GTReject &reject ) {
  // at some time, leave on screen and need manual delete for confirmation
  CString sId;
  sId.Format( "%d", reject.dwTraderSeqNo );
  if ( NULL != OnOrderComplete ) {
    OnOrderComplete( (LPCSTR) sId );
  }
}

void CGTOrderX::OnRemoved( const GTRemove &remove ) {
  // at some time, leave on screen and need manual delete for confirmation
  CString sId;
  sId.Format( "%d", remove.dwTraderSeqNo );
  if ( NULL != OnOrderComplete ) {
    OnOrderComplete( (LPCSTR) sId );
  }
}

void CGTOrderX::OnPending(const GTPending &pending) {
  memcpy( &m_pending, &pending, sizeof( pending ) );
}

const GTPending &CGTOrderX::GetPending() {
  return m_pending;
}