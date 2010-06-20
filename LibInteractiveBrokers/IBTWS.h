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

#include <LibCommon/FastDelegate.h>
#include <LibCommon/Delegate.h>

#include <LibTrading/TradingEnumerations.h>
#include <LibTrading/Instrument.h>
#include <LibTrading/ProviderInterface.h>

#include "TWS/EPosixClientSocket.h"
#include "TWS/EWrapper.h"

#include "TWS/Contract.h"
#include "TWS/Order.h"
#include "TWS/OrderState.h"
#include "TWS/Execution.h"

#include "IBSymbol.h"

class CIBTWS : 
  public CProviderInterface<CIBTWS, CIBSymbol>, 
  public EWrapper 
{
public:

  typedef CProviderInterface<CIBTWS, CIBSymbol> ProviderInterface_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef int reqId_t;  // request id type

  CIBTWS( const string &acctCode, const string &address = "127.0.0.1", unsigned int port = 7496 );
  ~CIBTWS(void);
  
  // From ProviderInterface:
  void Connect( void );
  void Disconnect( void );

  // From ProviderInterface Execution Section
  void PlaceOrder( COrder *order );
  //virtual void CancelOrder( unsigned long nOrderId );
  void CancelOrder( COrder *order );

  // TWS Function Calls
  //  need ot make a container of re-usable request ids to be looked up in order to return data to appropriate caller
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
  
  std::vector<CIBSymbol*> m_vTickerToSymbol;  // stuff comes back from IB with ticker id so use this to look up symbol, which is stored in the map of the class from which we inherited

  double m_dblBuyingPower;
  double m_dblAvailableFunds;

  static const char *szSecurityType[];
  static const char *szOrderType[];

  pInstrument_t BuildInstrumentFromContract( const Contract& contract );

  CIBSymbol *NewCSymbol( pInstrument_t pInstrument );

  // overridden from ProviderInterface
  void StartQuoteWatch( CIBSymbol* pSymbol );
  void  StopQuoteWatch( CIBSymbol* pSymbol );

  void StartTradeWatch( CIBSymbol* pSymbol );
  void  StopTradeWatch( CIBSymbol* pSymbol );

  void StartQuoteTradeWatch( CIBSymbol* pSymbol );
  void  StopQuoteTradeWatch( CIBSymbol* pSymbol );

  void StartDepthWatch( CIBSymbol* pSymbol );
  void  StopDepthWatch( CIBSymbol* pSymbol );

private:
  EPosixClientSocket *pTWS;
  long m_time;

  std::stringstream m_ss;  // for OutputDebugStrings in background thread

  OnContractDetailsHandler_t OnContractDetails;
  OnContractDetailsDoneHandler_t OnContractDetailsDone;

  struct structDeltaStuff {
    double delta;
    double impliedVolatility;
    double modelPrice;
    int position;
    int positionCalc;  // used by incremental option calculations.
    double positionDelta;
    double marketPrice;
    double averageCost;
    bool bDataRequested;
    structDeltaStuff(): delta( 0 ), positionDelta( 0 ), bDataRequested( false ) {};
  };

  // keep track of option greeks, maybe put in symbol at some point in time
  typedef std::map<TickerId, structDeltaStuff> mapGreeks_t;
  typedef std::pair<TickerId, structDeltaStuff> pair_mapGreeks_t;
  mapGreeks_t m_mapGreeks;

  // given a contract id, see if we have a symbol assigned for the symbol id
  typedef std::map<long, TickerId> mapContractToSymbolId_t;
  typedef std::pair<long, TickerId> pair_mapContractToSymbolId_t;
  mapContractToSymbolId_t m_mapContractToSymbolId;

  double m_dblPortfolioDelta;

  boost::thread m_thrdIBMessages;

  void ProcessMessages( void );

  std::vector<reqId_t> m_vReqId;
  reqId_t m_nxtReqId; 

  reqId_t NextReqId( void ) {
    reqId_t tmp;
    if ( 0 == m_vReqId.size() ) {
      tmp = ++m_nxtReqId;
    }
    else {
      tmp = m_vReqId.back();
      m_vReqId.pop_back();
    }
    return tmp;
  }

  void GiveBackReqId( reqId_t  id ) {
    assert( 0 < id );
    assert( id < m_nxtReqId );
    m_vReqId.push_back( id );
  }
};
