#include "StdAfx.h"
//#include "GTScalp.h"
#include "GTStockX.h"

#include <iostream>
using namespace std;

CGTStockX::CGTStockX(CGTSessionX &session, LPCSTR pszStock): GTStock(session, pszStock) {
  strcpy( GTStock::m_defOrder.szAccountID, session.m_pAccount32->szAccountID );
  strcpy( GTStock::m_defOrder.szStock, GTStock::m_szStock );
}

CGTStockX::~CGTStockX(void){
}

int CGTStockX::OnGotQuoteLevel1(GTLevel1 *pRcd) {
  //CString s;
  //s.Format(_T("Quote: Bid %d@%0.2f, Ask %d@%0.2f"), 
  //  pRcd->nBidSize, 
  //  pRcd->dblBidPrice, 
  //  pRcd->nAskSize,
  //  pRcd->dblAskPrice
    //pRcd->nTickDirect
  //  );
  //theApp.pConsoleMessages->WriteLine( s );
  //GTime32 gtTime = pRcd->gtime;
  //const GDate32 *gtDate = GTStock::m_pSysDate32;
  ptime dt = ptime( 
    boost::gregorian::date( GTStock::m_pSysDate32->nYear, GTStock::m_pSysDate32->chMonth, GTStock::m_pSysDate32->chDay ), 
    time_duration( pRcd->gtime.chHour, pRcd->gtime.chMin, pRcd->gtime.chSec, pRcd->gtime.chSec100 ) );
  CQuote quote(
    dt, 
    pRcd->dblBidPrice, 
    pRcd->nBidSize, 
    pRcd->dblAskPrice,
    pRcd->nAskSize );
  if ( NULL != OnQuoteLevel1 ) OnQuoteLevel1( quote );
  // delete pQuote; // needes to be deleted by called
  return GTStock::OnGotQuoteLevel1(pRcd);
}

int CGTStockX::OnGotQuotePrint(GTPrint *pRcd) {
  //CString s;
  //s.Format( _T("Trade: %c %d@%0.2f %d"), 
  //  pRcd->chSource, 
  //  pRcd->dwShares, 
  //  pRcd->dblPrice, 
  //  pRcd->exchangecode
    //pRcd->rgbColor
  //  );
  //theApp.pConsoleMessages->WriteLine( s );
  ptime dt = ptime( 
    boost::gregorian::date( GTStock::m_pSysDate32->nYear, GTStock::m_pSysDate32->chMonth, GTStock::m_pSysDate32->chDay ), 
    time_duration( pRcd->gtime.chHour, pRcd->gtime.chMin, pRcd->gtime.chSec, pRcd->gtime.chSec100 ) );
  //CTrade *pTrade = new CTrade( dt, pRcd->dblPrice, pRcd->dwShares );
  CTrade trade( dt, pRcd->dblPrice, pRcd->dwShares );
  //if ( NULL != OnPrint ) OnPrint( pRcd->dwShares, pRcd->dblPrice );
  if ( NULL != OnPrint ) OnPrint( trade );
  //delete pTrade;  // needs to be deleted by called
  return GTStock::OnGotQuotePrint(pRcd);
}

int CGTStockX::OnGotLevel2Record(GTLevel2 *pRcd) {

  //char szMMID[10];
  //copymmid( szMMID, pRcd->mmid );
  //char Side = pRcd->chSide;
  //long Shares = pRcd->dwShares;
  //double Price = pRcd->dblPrice;

  ptime dt = ptime( 
    boost::gregorian::date( GTStock::m_pSysDate32->nYear, GTStock::m_pSysDate32->chMonth, GTStock::m_pSysDate32->chDay ), 
    time_duration( pRcd->gtime.chHour, pRcd->gtime.chMin, pRcd->gtime.chSec, pRcd->gtime.chSec100 ) );
  CMarketDepth md( dt, pRcd->chSide, pRcd->dwShares, pRcd->dblPrice, pRcd->mmid );

  //CString s;
  //s.Format( "L2: %c, %d@%0.2f, %s", 
  //  Side,
  //  Shares,
  //  Price,
  //  szMMID
  //  );
  //theApp.pConsoleMessages->WriteLine( s );

  //if ( NULL != OnLevel2 ) OnLevel2( Side, Shares, Price, szMMID );
  if ( NULL != OnLevel2 ) OnLevel2( md );

  int returnValue = GTStock::OnGotLevel2Record(pRcd);
  return returnValue;
}

int CGTStockX::OnGotLevel2Refresh() {
  //theApp.pConsoleMessages->WriteLine( "OnGotLevel2Refresh" );
  cout << "OnGotLevel2Refresh" << endl;
  return GTStock::OnGotLevel2Refresh();
}

int CGTStockX::OnGotLevel2Display() {
  //theApp.pConsoleMessages->WriteLine( "OnGotLevel2Display" );
  cout << "OnGotLevel2Display" << endl;
  // draw what we have
  //this->m_level2.
  return GTStock::OnGotLevel2Display();
}

int CGTStockX::OnGotLevel2Clear( MMID mmid ) {
  char szMMID[10];
  CString s;
  s.Format( "OnGotLevel2Clear: %s", copymmid( szMMID, mmid ) );
  //theApp.pConsoleMessages->WriteLine( s );
  cout << s << endl;
  // inhibit drawing, and clear everything
  return GTStock::OnGotLevel2Clear(mmid);
}

int CGTStockX::OnBestBidPriceChanged() {
  //sCallbackText.Format( "OnBestBidPriceChanged: %0.2f", GTStock::m_level2.GetBestBidPrice() );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  return GTStock::OnBestBidPriceChanged();
}

int CGTStockX::OnBestAskPriceChanged() {
  //sCallbackText.Format( "OnBestAskPriceChanged: %0.2f", GTStock::m_level2.GetBestAskPrice() );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  return GTStock::OnBestAskPriceChanged();
}

int CGTStockX::PrePlaceOrder(GTOrder &order) {
  sCallbackText.Format( "PrePlaceOrder" );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::PrePlaceOrder(order);
}

int CGTStockX::PostPlaceOrder(GTOrder &order) {
  sCallbackText.Format( "PostPlaceOrder" );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::PostPlaceOrder(order);
}

int CGTStockX::OnSendingOrder(const GTSending &gtsending) {
  sCallbackText.Format( "OnSendingOrder" );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  CGTOrderX *pOrder = (CGTOrderX*) gtsending.lpUserData;
  mapOrder.insert( pairOrder( gtsending.dwTraderSeqNo, pOrder ) );
  pOrder->OnSendingOrder( gtsending );
  return GTStock::OnSendingOrder(gtsending);
}

int CGTStockX::OnExecMsgErrMsg(const GTErrMsg &errmsg) {
  sCallbackText.Format( "OnExecMsgErrMsg: orderid=%d, err=%d, symbol=%s, msg=%s", 
    errmsg.dwOrderSeqNo, errmsg.nErrCode, errmsg.szStock, errmsg.szText );
  // note that this is dwOrderSeqNo not dwTraderSeqNo
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  //iterOrder = mapOrder.find( errmsg.dwOrderSeqNo );
  //if ( mapOrder.end() != iterOrder ) {
  //  CGTOrderX *pOrder = iterOrder -> second;
  //  pOrder->OnErrorMessage( errmsg );
  //}
  return GTStock::OnExecMsgErrMsg(errmsg);
}

int CGTStockX::OnExecMsgOpenPosition(const GTOpenPosition &open) {
  // is this called upon connection, so as to automate open position maintenance?
  sCallbackText.Format( "OnExecMsgOpenPosition %08x %06x %s %c %d@%0.2f", 
    open.nOpenDate, open.nOpenTime, open.szStock, open.chOpenSide, open.nOpenShares, open.dblOpenPrice );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  if ( NULL != OnOpenPosition ) OnOpenPosition( open.dblOpenPrice, open.chOpenSide, open.nOpenShares );
  return GTStock::OnExecMsgOpenPosition(open);
}

int CGTStockX::OnExecMsgTrade(const GTTrade &trade) {
  sCallbackText.Format( "OnExecMsgTrade %d %s %c %c entry=%0.2f exec=%0.2f cost=%0.2f exec=%d remain=%d", 
    trade.dwTraderSeqNo,
    trade.szStock, trade.chExecSide, trade.chPriceIndicator, 
    trade.dblEntryPrice, trade.dblExecPrice, trade.dblCost,
    trade.nExecShares, trade.nExecRemainShares);
  iterOrder = mapOrder.find( trade.dwTraderSeqNo );
  if ( mapOrder.end() != iterOrder ) {
    CGTOrderX *pOrder = iterOrder -> second;
    pOrder->OnTraded( trade );
  }
  else {
    sCallbackText.Append( " (dwTraderSeqNo not found)" );
  }
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgTrade(trade);
}

int CGTStockX::OnExecMsgPending(const GTPending &pending) {

  char szMMID1[10];
  char szMMID2[10];

  sCallbackText.Format( "OnExecMsgPending %d method=%s place=%s", pending.dwTraderSeqNo, 
    copymmid( szMMID1, pending.method ), copymmid( szMMID2, pending.place ) );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  //CGTOrderX *pOrder = (CGTOrderX*) pending.
  return GTStock::OnExecMsgPending(pending);
}

int CGTStockX::OnExecMsgSending(const GTSending &sending) {
  sCallbackText.Format( "OnExecMsgSending seq=%d tickno=%d ref=%s", 
    sending.dwTraderSeqNo, sending.dwTicketNo, sending.szRefNo );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  //sending.
  return GTStock::OnExecMsgSending(sending);
}

int CGTStockX::OnExecMsgCanceling(const GTCancel &cancel) {
  sCallbackText.Format( "OnExecMsgCanceling %d canceler=%d remain=%d cnt=%d ref=%s reason=%s",
    cancel.dwTraderSeqNo, cancel.nCanceler, cancel.nCancelRemainShares, cancel.nCancelShares, cancel.szRefNo, cancel.szCancelReason);
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgCanceling(cancel);
}

int CGTStockX::OnExecMsgCancel(const GTCancel &cancel) {
  sCallbackText.Format( "OnExecMsgCancel %d price=%0.2f pend=%0.2f cnt=%d reason=%s", 
    cancel.dwTraderSeqNo, cancel.dblCancelPrice, cancel.dblPendPrice, cancel.nCancelShares, cancel.szCancelReason);
  iterOrder = mapOrder.find( cancel.dwTraderSeqNo );
  if ( mapOrder.end() != iterOrder ) {
    CGTOrderX *pOrder = iterOrder -> second;
    pOrder->OnCancelled( cancel );
  }
  else {
    sCallbackText.Append( " (dwTraderSeqNo not found)" );
  }
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgCancel(cancel);
}

int CGTStockX::OnExecMsgReject(const GTReject &reject) {
  sCallbackText.Format( "OnExecMsgReject %d cnt=%d reason=$s",
    reject.dwTraderSeqNo, reject.nRejectShares, reject.szRejectReason );
  iterOrder = mapOrder.find( reject.dwTraderSeqNo );
  if ( mapOrder.end() != iterOrder ) {
    CGTOrderX *pOrder = iterOrder -> second;
    pOrder->OnRejected( reject );
  }
  else {
    sCallbackText.Append( " (dwTraderSeqNo not found)" );
  }
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgReject(reject);
}

int CGTStockX::OnExecMsgRemove(const GTRemove &remove) {
  sCallbackText.Format( "OnExecMsgRemove %d %s", remove.dwTraderSeqNo, remove.szRemoveReason );
  iterOrder = mapOrder.find( remove.dwTraderSeqNo );
  if ( mapOrder.end() != iterOrder ) {
    CGTOrderX *pOrder = iterOrder -> second;
    pOrder->OnRemoved( remove );
  }
  else {
    sCallbackText.Append( " (dwTraderSeqNo not found)" );
  }
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgRemove(remove);
}

int CGTStockX::OnExecMsgRejectCancel(const GTRejectCancel &rejectcancel) {
  sCallbackText.Format( "OnExecMsgRejectCancel %d %s", rejectcancel.dwTraderSeqNo, rejectcancel.szRejectReason );
  //theApp.pConsoleMessages->WriteLine( sCallbackText );
  cout << sCallbackText << endl;
  return GTStock::OnExecMsgRejectCancel(rejectcancel);
}

