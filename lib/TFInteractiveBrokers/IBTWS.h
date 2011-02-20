/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

#ifndef IB_USE_STD_STRING
#define IB_USE_STD_STRING
#endif

#include <boost/shared_ptr.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/thread.hpp> 
#include <boost/bind.hpp>

#include <OUCommon/FastDelegate.h>
#include <OUCommon/Delegate.h>

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Order.h>

#include "IBSymbol.h"  // has settings for IBString, which affects the following TWS includes.

#include "TWS/EPosixClientSocket.h"
#include "TWS/EWrapper.h"

#include "TWS/Contract.h"
#include "TWS/Order.h"
#include "TWS/OrderState.h"
#include "TWS/Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CIBTWS : 
  public CProviderInterface<CIBTWS, CIBSymbol>, 
  public EWrapper 
{
public:

  typedef boost::shared_ptr<CIBTWS> pProvider_t;
  typedef CProviderInterface<CIBTWS, CIBSymbol> ProviderInterface_t;
  typedef CIBSymbol::pSymbol_t pSymbol_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef COrder::pOrder_t pOrder_t;
  typedef int reqId_t;  // request id type

  CIBTWS( const std::string &acctCode, const std::string &address = "127.0.0.1", unsigned int port = 7496 );
  ~CIBTWS(void);
  
  // From ProviderInterface:
  void Connect( void );
  void Disconnect( void );

  // From ProviderInterface Execution Section
  void PlaceOrder( pOrder_t order );
  void CancelOrder( pOrder_t order );

  // TWS Function Calls
  //  need to make a container of re-usable request ids to be looked up in order to return data to appropriate caller
  //   therefore, currently, caller needs to appropriately serialize the calls to keep requests one at a time
  //   ie, may need an array of OnContractDetailsHandler_t
  void RequestContractDetails( const Contract& contract ) { pTWS->reqContractDetails( NextReqId(), contract ); };
  typedef FastDelegate1<const ContractDetails&> OnContractDetailsHandler_t;
  void SetOnContractDetailsHandler( OnContractDetailsHandler_t function ) {
    OnContractDetails = function;
  }
  typedef FastDelegate0<void> OnContractDetailsDoneHandler_t;
  void SetOnContractDetailsDoneHandler( OnContractDetailsDoneHandler_t function ) {
    OnContractDetailsDone = function;
  }

  pInstrument_t BuildInstrumentFromContract( const Contract& contract );
  pSymbol_t GetSymbol( long ContractId );  // query existance
  pSymbol_t GetSymbol( pInstrument_t instrument );  // query for and add if doesn't exist

  // TWS Specific events
  // From TWS Wrapper:
  void connectionClosed();
  void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute);
  void tickSize( TickerId tickerId, TickType field, int size);
  void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
  void tickGeneric(TickerId tickerId, TickType tickType, double value);
  void tickString(TickerId tickerId, TickType tickType, const IBString& value);
  void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints,
	   double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry);
  void orderStatus( OrderId orderId, const IBString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const IBString& whyHeld);
  void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
  void openOrderEnd() {};  // **
  void execDetails( int reqId, const Contract& contract, const Execution& execution );
  void execDetailsEnd( int reqId) {};  // **
  void error(const int id, const int errorCode, const IBString errorString);
  void winError( const IBString &str, int lastError);
  void updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch);
  void currentTime(long time);
  void contractDetails( int reqId, const ContractDetails& contractDetails );
  void contractDetailsEnd( int reqId );
  void bondContractDetails( int reqId, const ContractDetails& contractDetails );
  void nextValidId( OrderId orderId);
  void updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const IBString& accountName);
  void updateAccountValue(const IBString& key, const IBString& val,
   const IBString& currency, const IBString& accountName);
  void updateAccountTime(const IBString& timeStamp);
  void accountDownloadEnd(const IBString& accountName) {};  // **
  void updateMktDepth(TickerId id, int position, int operation, int side,
      double price, int size);
  void updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation,
      int side, double price, int size);
  void managedAccounts( const IBString& accountsList);
     virtual void receiveFA(faDataType pFaDataType, const IBString& cxml);
  void historicalData(TickerId reqId, const IBString& date, double open, double high, 
	   double low, double close, int volume, int barCount, double WAP, int hasGaps);
  void scannerParameters(const IBString &xml);
  void scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const IBString &distance, const IBString &benchmark, const IBString &projection,
	   const IBString &legsStr);
  void scannerDataEnd(int reqId);
  void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count);
  void fundamentalData(TickerId reqId, const IBString& data) {};  // **
  void deltaNeutralValidation(int reqId, const UnderComp& underComp) {};  // **
  void tickSnapshotEnd( int reqId) {};  // **

protected:

  static char* TickTypeStrings[];

  std::string m_sAccountCode;
  std::string m_sIPAddress;
  unsigned int m_nPort;
  TickerId m_curTickerId;
  
  double m_dblBuyingPower;
  double m_dblAvailableFunds;

  static const char *szSecurityType[];
  static const char *szOrderType[];

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );

  // overridden from ProviderInterface
  void StartQuoteWatch( pSymbol_t pSymbol );
  void  StopQuoteWatch( pSymbol_t pSymbol );

  void StartTradeWatch( pSymbol_t pSymbol );
  void  StopTradeWatch( pSymbol_t pSymbol );

  void StartQuoteTradeWatch( pSymbol_t pSymbol );
  void  StopQuoteTradeWatch( pSymbol_t pSymbol );

  void StartDepthWatch( pSymbol_t pSymbol );
  void  StopDepthWatch( pSymbol_t pSymbol );

private:
  EPosixClientSocket *pTWS;
  long m_time;

  std::stringstream m_ss;  // for OutputDebugStrings in background thread

  OnContractDetailsHandler_t OnContractDetails;
  OnContractDetailsDoneHandler_t OnContractDetailsDone;

  // stuff comes back from IB with ticker id so use this to look up symbol, 
  //    which is stored in the map of the class from which we inherited
  std::vector<pSymbol_t> m_vTickerToSymbol;  

  // given a contract id, see if we have a symbol assigned
  typedef std::map<long, pSymbol_t> mapContractToSymbol_t;
  typedef std::pair<long, pSymbol_t> pair_mapContractToSymbol_t;
  mapContractToSymbol_t m_mapContractToSymbol;

  // do we actually need this anymore given that we have the above vector?
  // given a ticker id, see if we have a symbol assigned
//  typedef std::map<TickerId, pSymbol_t> mapTickerIdToSymbol_t;
//  typedef std::pair<TickerId, pSymbol_t> pair_mapContractToSymbol_t;
//  pair_mapContractToSymbol_t m_mapContractToSymbol;

  boost::thread m_thrdIBMessages;

  void ProcessMessages( void );

  std::vector<reqId_t> m_vReqId;
  reqId_t m_nxtReqId; 

  // todo:  turn this stuff into a class so that each request has its own fastdelegate
  reqId_t NextReqId( void ) {
    reqId_t tmp;
    if ( 0 == m_vReqId.size() ) {
      // starts with 0
      tmp = m_nxtReqId++;
    }
    else {
      tmp = m_vReqId.back();
      m_vReqId.pop_back();
    }
    return tmp;
  }

  void GiveBackReqId( reqId_t  id ) {
//    assert( 0 < id );
    assert( id < m_nxtReqId );
    m_vReqId.push_back( id );
  }
};

} // namespace tf
} // namespace ou
