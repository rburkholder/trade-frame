#include "StdAfx.h"
#include "IBTWS.h"

#include "TWS\Contract.h"
#include "TWS\Order.h"
#include "TWS\OrderState.h"
#include "TWS\Execution.h"

#include <iostream>

CIBTWS::CIBTWS( const string &acctCode, const string &address, UINT port ): 
  CProviderInterface(), m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 )
{
}

CIBTWS::~CIBTWS(void) {
}

//void CIBTWS::Start() {
  //Contract contract;
  //contract.symbol = "GOOG";
  //contract.currency = "USD";
  //contract.exchange = "SMART";
  //contract.secType = "STK";
  //pTWS->reqAccountUpdates( true, m_sAccountCode );
  //pTWS->reqAllOpenOrders();
  //pTWS->reqNewsBulletins( true );
  //pTWS->reqCurrentTime();
  //pTWS->reqMktData( 1, contract, "100,101,104,165,221,225,236", false );
//}

void CIBTWS::Connect() {
  if ( NULL == pTWS ) {
    pTWS = new EClientSocket( this );
    pTWS->eConnect( m_sIPAddress.c_str(), m_nPort );
    OnConnected( 0 );
  }
}

void CIBTWS::Disconnect() {
  // check to see if there are any watches happening, and get them disconnected
  if ( NULL != pTWS ) {
    pTWS->eDisconnect();
    delete pTWS;
    pTWS = NULL;
    OnDisconnected( 0 );
  }
}

void CIBTWS::tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute) {
  //std::cout << "tickPrice " << tickerId << ", " << field << ", " << price << std::endl;
}

void CIBTWS::tickSize( TickerId tickerId, TickType field, int size) {
  //std::cout << "tickSize " << tickerId << ", " << field << ", " << size << std::endl;
}

void CIBTWS::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
                                   double modelPrice, double pvDividend) {
  std::cout << "tickOptionComputation" << std::endl;
}

void CIBTWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
  std::cout << "tickGeneric " << tickerId << ", " << tickType << ", " << value << std::endl;
}

void CIBTWS::tickString(TickerId tickerId, TickType tickType, const CString& value) {
  //std::cout << "tickString " << tickerId << ", " << tickType << ", " << value << std::endl;
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


