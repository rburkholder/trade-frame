#include "StdAfx.h"
#include "IBTWS.h"

#include "TWS\Contract.h"
#include "TWS\Order.h"
#include "TWS\OrderState.h"
#include "TWS\Execution.h"

#include <iostream>
#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CIBTWS::CIBTWS( const string &acctCode, const string &address, UINT port ): 
  CProviderInterface(), 
    pTWS( NULL ),
    m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 )
{
  CIBSymbol *p = NULL;
  m_vTickerToSymbol.push_back( p );
}

CIBTWS::~CIBTWS(void) {
  Disconnect();
  m_vTickerToSymbol.clear();
}

//void CIBTWS::Start() {

  //pTWS->reqAccountUpdates( true, m_sAccountCode );
  //pTWS->reqAllOpenOrders();

//}

void CIBTWS::Connect() {
  if ( NULL == pTWS ) {
    pTWS = new EClientSocket( this );
    pTWS->eConnect( m_sIPAddress.c_str(), m_nPort );
    OnConnected( 0 );
    pTWS->reqCurrentTime();
    pTWS->reqNewsBulletins( true );
  }
}

void CIBTWS::Disconnect() {
  // check to see if there are any watches happening, and get them disconnected
  if ( NULL != pTWS ) {
    pTWS->eDisconnect();
    delete pTWS;
    pTWS = NULL;
    OnDisconnected( 0 );
    std::cout << "IB Disconnected " << std::endl;
  }
}

CSymbol *CIBTWS::NewCSymbol( const std::string &sSymbolName ) {
  TickerId ticker = ++m_curTickerId;
  CIBSymbol *pSymbol = new CIBSymbol( sSymbolName, ticker );
  m_vTickerToSymbol.push_back( pSymbol );
  return pSymbol;
}

void CIBTWS::StartQuoteWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopQuoteWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartTradeWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopTradeWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartQuoteTradeWatch( CSymbol *pSymbol ) {
  CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( !pIBSymbol->m_bQuoteTradeWatchInProgress ) {
    // start watch
    pIBSymbol->m_bQuoteTradeWatchInProgress = true;
    Contract contract;
    contract.symbol = pSymbol->Name().c_str();
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.secType = "STK";
    //pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "100,101,104,165,221,225,236", false );
    pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "", false );
  }
}

void CIBTWS::StopQuoteTradeWatch( CSymbol *pSymbol ) {
  CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( pIBSymbol->QuoteWatchNeeded() || pIBSymbol->TradeWatchNeeded() ) {
    // don't do anything if either a quote or trade watch still in progress
  }
  else {
    // stop watch
    pTWS->cancelMktData( pIBSymbol->GetTickerId() );
    pIBSymbol->m_bQuoteTradeWatchInProgress = false;
  }
}

void CIBTWS::StartDepthWatch(CSymbol *pSymbol) {
  CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( !pIBSymbol->m_bDepthWatchInProgress ) {
    // start watch
    pIBSymbol->m_bDepthWatchInProgress = true;
  }
}

void CIBTWS::StopDepthWatch(CSymbol *pSymbol) {
  CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( pIBSymbol->DepthWatchNeeded() ) {
  }
  else {
    // stop watch
    pIBSymbol->m_bDepthWatchInProgress = false;
  }
}

void CIBTWS::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute) {
  CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
  //std::cout << "tickPrice " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << price << std::endl;
  pSym->AcceptTickPrice( tickType, price );
}

void CIBTWS::tickSize( TickerId tickerId, TickType tickType, int size) {
  CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
  //std::cout << "tickSize " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << size << std::endl;
  pSym->AcceptTickSize( tickType, size );
}

void CIBTWS::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
                                   double modelPrice, double pvDividend) {
  std::cout << "tickOptionComputation" << ", " << TickTypeStrings[tickType] << std::endl; 
}

void CIBTWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
  std::cout << "tickGeneric " << m_vTickerToSymbol[ tickerId ]->Name() << ", " << TickTypeStrings[tickType] << ", " << value << std::endl;
}

void CIBTWS::tickString(TickerId tickerId, TickType tickType, const CString& value) {
  CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
  //std::cout << "tickString " << pSym->Name() << ", " 
  //  << TickTypeStrings[tickType] << ", " << value;
  //std::cout << std::endl;
  pSym->AcceptTickString( tickType, value );
}

void CIBTWS::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const CString& formattedBasisPoints,
  double totalDividends, int holdDays, const CString& futureExpiry, double dividendImpact, double dividendsToExpiry ) {
  std::cout << "tickEFP" << std::endl;
}

void CIBTWS::orderStatus( OrderId orderId, const CString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
     double lastFillPrice, int clientId, const CString& whyHeld) {
  std::cout << "order status " << orderId << ", " << status << ", " 
    << filled << ", " << ", " << remaining << ", " << avgFillPrice << ", "
    << permId << ", " << parentId << ", " << lastFillPrice << ", " 
    << clientId << ", " << whyHeld << std::endl;
}

void CIBTWS::openOrder( OrderId orderId, const Contract& contract, const Order& order, const OrderState& state) {
  std::cout << "open order " << orderId << ", " << contract.symbol << ", "
    << order.orderId << ", " << order.orderRef << ", "
    << state.status << std::endl;
}

void CIBTWS::error(const int id, const int errorCode, const CString errorString) {
  std::cout << "error " << id << ", " << errorCode << ", " << errorString << std::endl;
}

void CIBTWS::winError( const CString &str, int lastError) {
  std::cout << "winerror " << str << ", " << lastError << std::endl;
}

void CIBTWS::updateNewsBulletin(int msgId, int msgType, const CString& newsMessage, const CString& originExch) {
  std::cout << "news bulletin " << msgId << ", " << msgType << ", " << newsMessage << ", " << originExch << std::endl;
}

void CIBTWS::currentTime(long time) {
  std::cout << "current time " << time << endl;
  m_time = time;
}

void CIBTWS::updateAccountTime(const CString& timeStamp) {
}

void CIBTWS::execDetails( OrderId orderId, const Contract& contract, const Execution& execution) {
  std::cout << "exec " << orderId << ", " << contract.symbol << ", " << execution.orderId 
    << ", " << execution.price << ", " << execution.shares << ", " << execution.side 
    << ", " << execution.time << std::endl;
}

void CIBTWS::contractDetails( const ContractDetails& contractDetails) {
  std::cout << "contract Details " << 
    contractDetails.orderTypes << ", " << contractDetails.minTick << std::endl;
}

void CIBTWS::bondContractDetails( const ContractDetails& contractDetails) {
}

void CIBTWS::nextValidId( OrderId orderId) {
  std::cout << "next valid id " << orderId << std::endl;
}

void CIBTWS::updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const CString& accountName) {
        std::cout << "portfolio " << contract.symbol << ", " << position << ", " 
    << marketPrice << ", " << marketValue << ", " << averageCost << ", "
    << unrealizedPNL << ", " << realizedPNL << ", " << accountName << std::endl;
}

void CIBTWS::updateAccountValue(const CString& key, const CString& val,
                                const CString& currency, const CString& accountName) {
  std::cout << "account value " << key << ", " << val << ", " << currency << ", " << accountName << std::endl;
}

void CIBTWS::connectionClosed() {
  std::cout << "connection closed" << std::endl;
}

void CIBTWS::updateMktDepth(TickerId id, int position, int operation, int side,
                            double price, int size) {
}

void CIBTWS::updateMktDepthL2(TickerId id, int position, CString marketMaker, int operation,
                              int side, double price, int size) {
}

void CIBTWS::managedAccounts( const CString& accountsList) {
}

void CIBTWS::receiveFA(faDataType pFaDataType, const CString& cxml) {
}

void CIBTWS::historicalData(TickerId reqId, const CString& date, double open, double high, 
                            double low, double close, int volume, int barCount, double WAP, int hasGaps) {
}

void CIBTWS::scannerParameters(const CString &xml) {
}

void CIBTWS::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
                         const CString &distance, const CString &benchmark, const CString &projection,
                         const CString &legsStr) {
}

void CIBTWS::scannerDataEnd(int reqId) {
}

void CIBTWS::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
                         long volume, double wap, int count) {
}

// From EWrapper.h
char *CIBTWS::TickTypeStrings[] = {
  "BID_SIZE", "BID", "ASK", "ASK_SIZE", "LAST", "LAST_SIZE",
				"HIGH", "LOW", "VOLUME", "CLOSE",
				"BID_OPTION_COMPUTATION", 
				"ASK_OPTION_COMPUTATION", 
				"LAST_OPTION_COMPUTATION",
				"MODEL_OPTION",
				"OPEN",
				"LOW_13_WEEK",
				"HIGH_13_WEEK",
				"LOW_26_WEEK",
				"HIGH_26_WEEK",
				"LOW_52_WEEK",
				"HIGH_52_WEEK",
				"AVG_VOLUME",
				"OPEN_INTEREST",
				"OPTION_HISTORICAL_VOL",
				"OPTION_IMPLIED_VOL",
				"OPTION_BID_EXCH",
				"OPTION_ASK_EXCH",
				"OPTION_CALL_OPEN_INTEREST",
				"OPTION_PUT_OPEN_INTEREST",
				"OPTION_CALL_VOLUME",
				"OPTION_PUT_VOLUME",
				"INDEX_FUTURE_PREMIUM",
				"BID_EXCH",
				"ASK_EXCH",
				"AUCTION_VOLUME",
				"AUCTION_PRICE",
				"AUCTION_IMBALANCE",
				"MARK_PRICE",
				"BID_EFP_COMPUTATION",
				"ASK_EFP_COMPUTATION",
				"LAST_EFP_COMPUTATION",
				"OPEN_EFP_COMPUTATION",
				"HIGH_EFP_COMPUTATION",
				"LOW_EFP_COMPUTATION",
				"CLOSE_EFP_COMPUTATION",
				"LAST_TIMESTAMP",
				"SHORTABLE",
				"NOT_SET"
};