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

#include <boost/shared_ptr.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#ifndef IB_USE_STD_STRING
#define IB_USE_STD_STRING
#endif

#include "TWS/EPosixClientSocket.h"
#include "TWS/EWrapper.h"

#include <LibTrading/ProviderInterface.h>
#include <LibTrading/TradingEnumerations.h>
#include <LibTrading/Instrument.h>
#include "IBSymbol.h"

#include <LibCommon/Delegate.h>

class CIBTWS : public EWrapper, public CProviderInterface {
public:
  CIBTWS( const string &acctCode, const string &address = "127.0.0.1", unsigned int port = 7496 );
  ~CIBTWS(void);
  
  // From ProviderInterface:
  virtual void Connect( void );
  virtual void Disconnect( void );

  // From ProviderInterface Execution Section
  virtual void PlaceOrder( COrder *order );
  //virtual void CancelOrder( unsigned long nOrderId );
  virtual void CancelOrder( COrder *order );

  // TWS Specific events

  // From TWS Wrapper:
  virtual void connectionClosed();
  virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute);
  virtual void tickSize( TickerId tickerId, TickType field, int size);
  virtual void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double modelPrice, double pvDividend);
  virtual void tickGeneric(TickerId tickerId, TickType tickType, double value);
  virtual void tickString(TickerId tickerId, TickType tickType, const IBString& value);
  virtual void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints,
	   double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry);
   virtual void orderStatus( OrderId orderId, const IBString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const IBString& whyHeld);
  virtual void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
  virtual void execDetails( OrderId orderId, const Contract& contract, const Execution& execution);
  virtual void error(const int id, const int errorCode, const IBString errorString);
  virtual void winError( const IBString &str, int lastError);
  virtual void updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch);
  virtual void currentTime(long time);
  virtual void contractDetails( const ContractDetails& contractDetails);
  virtual void bondContractDetails( const ContractDetails& contractDetails);
  virtual void nextValidId( OrderId orderId);
  virtual void updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const IBString& accountName);
  virtual void updateAccountValue(const IBString& key, const IBString& val,
   const IBString& currency, const IBString& accountName);
  virtual void updateAccountTime(const IBString& timeStamp);
  virtual void updateMktDepth(TickerId id, int position, int operation, int side,
      double price, int size);
  virtual void updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation,
      int side, double price, int size);
  virtual void managedAccounts( const IBString& accountsList);
     virtual void receiveFA(faDataType pFaDataType, const IBString& cxml);
   virtual void historicalData(TickerId reqId, const IBString& date, double open, double high, 
	   double low, double close, int volume, int barCount, double WAP, int hasGaps);
   virtual void scannerParameters(const IBString &xml);
   virtual void scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const IBString &distance, const IBString &benchmark, const IBString &projection,
	   const IBString &legsStr);
   virtual void scannerDataEnd(int reqId);
   virtual void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count);

protected:
  std::string m_sAccountCode;
  std::string m_sIPAddress;
  unsigned int m_nPort;
  TickerId m_curTickerId;
  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );
  std::vector<CIBSymbol *> m_vTickerToSymbol;  // stuff comes back from IB with ticker id so use this to look up symbol, which is stored in the map of the class from which we inherited

  double m_dblBuyingPower;
  double m_dblAvailableFunds;

  static const char *szSecurityType[];
  static const char *szOrderType[];

  // overridden from ProviderInterface
  virtual void StartQuoteWatch( CSymbol *pSymbol );
  virtual void StopQuoteWatch( CSymbol *pSymbol );

  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );

  virtual void StartQuoteTradeWatch( CSymbol *pSymbol );
  virtual void StopQuoteTradeWatch( CSymbol *pSymbol );

  virtual void StartDepthWatch( CSymbol *pSymbol );
  virtual void StopDepthWatch( CSymbol *pSymbol );

  static char *TickTypeStrings[];

private:
  EPosixClientSocket *pTWS;
  long m_time;

  struct structDeltaStuff {
    TickerId tickerId;
    CInstrument::pInstrument_t pInstrument;
    long contractId;
    std::string sUnderlying;
    std::string sSymbol;
    double delta;
    double impliedVolatility;
    double modelPrice;
    double strike;
    ptime dtExpiry;
    int position;
    int positionCalc;  // used by incremental option calculations.
    double positionDelta;
    double marketPrice;
    double averageCost;
    OptionSide::enumOptionSide os;
    bool bDataRequested;
    structDeltaStuff(): delta( 0 ), positionDelta( 0 ), bDataRequested( false ) {};
  };

  typedef std::map<long, structDeltaStuff> mapDelta_t;
  typedef std::pair<long, structDeltaStuff> pair_mapDelta_t;
  mapDelta_t m_mapDelta;

  typedef std::map<TickerId, long> mapTickerIdToContract_t;
  typedef std::pair<TickerId, long> pair_mapTickerIdToContract_t;
  mapTickerIdToContract_t m_mapTickerIdToContract;

  double m_dblPortfolioDelta;
  
};
