/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <condition_variable>

#include <OUCommon/FastDelegate.h>
#include <OUCommon/Delegate.h>

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Order.h>

#include "client/EWrapper.h"
#include "client/EReaderOSSignal.h"
#include "client/EReader.h"

#include "client/Order.h"
#include "client/Contract.h"
#include "client/Execution.h"

#include "Bag.hpp"
#include "IBSymbol.h"  // has settings for IBString, which affects the following TWS includes.

class EClientSocket;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace ib { // Interactive Brokers

class TWS :
  public EWrapper
, public ProviderInterface<TWS, Symbol>
{
public:

  using pProvider_t = std::shared_ptr<TWS>;
  using ProviderInterface_t = ProviderInterface<TWS, Symbol>;
  using inherited_t = ProviderInterface_t;
  using pSymbol_t = Symbol::pSymbol_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pOrder_t = ou::tf::Order::pOrder_t;
  using reqId_t = int;  // request id type

  using Bar = ::Bar;
  using Order = ::Order;
  using Contract = ::Contract;
  using ContractDetails = ::ContractDetails;
  using Execution = ::Execution;

  TWS( const std::string& acctCode = "", const std::string& address = "127.0.0.1", unsigned int port = 7496 );
  virtual ~TWS();

  static pProvider_t Factory() {
    return std::make_shared<TWS>();
  }

  static pProvider_t Cast( inherited_t::pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<TWS>( pProvider );
  }

  void SetClientId( int idClient ) { m_idClient = idClient; }
  void SetClientPort( unsigned int nPort ) { m_nPort = nPort; }

  // From ProviderInterface:
  virtual void Connect();
  virtual void Disconnect();

  // From ProviderInterface Execution Section
  void PlaceOrder( pOrder_t order );
  void PlaceComboOrder( pOrder_t pOrderEntry, pOrder_t pOrderStop );
  void PlaceBracketOrder( pOrder_t pOrderEntry, pOrder_t pOrderProfit, pOrder_t pOrderStop );
  void PlaceBagOrder( const Bag& );
  void CancelOrder( pOrder_t order );

  static void ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth );
  static void ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay );

  using fOnContractDetail_t = std::function<void(const ContractDetails&, pInstrument_t&)>;
  using fOnContractDetailDone_t = std::function<void(bool)>; // true if success, false if stuck in queue;

  // primary instance:
  void RequestContractDetails( const std::string& sSymbolBaseName, pInstrument_t&,
                                                         fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone );
  // in use by TFOptions/PopulateWithIBOptions.cpp:63:38
  void RequestContractDetails( const Contract& contract, fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone );

  // in use by ComboTrading/ComboTrading.cpp:444:38, fix and move this to private
  void RequestContractDetails( const Contract& contract, fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone, pInstrument_t& );

  struct PositionDetail {
    std::string sSymbol;
    std::string sLocalSymbol;
    long contract;
    double strike;
    std::string sExpiry;
    std::string sMultiplier;
    int position;
    double marketPrice;
    double marketValue;
    double averageCost;
    double unrealizedPNL;
    double realizedPNL;
    std::string sExchange;
    std::string sCurrency;
    PositionDetail(
      const std::string& sSymbol_,
      const std::string& sLocalSymbol_,
      long contract_,
      double strike_,
      const std::string& sExpiry_,
      const std::string& sMultiplier_,
      int position_,
      double marketPrice_,
      double marketValue_,
      double averageCost_,
      double unrealizedPNL_,
      double realizedPNL_,
      const std::string& sExchange_,
      const std::string& sCurrency_
      ): sSymbol( sSymbol_ ), sLocalSymbol( sLocalSymbol_ ), contract( contract_ ), strike( strike_ ),
      sExpiry( sExpiry_ ), sMultiplier( sMultiplier_ ),
      position( position_ ), marketPrice( marketPrice_ ), marketValue( marketValue_ ),
      averageCost( averageCost_ ), unrealizedPNL( unrealizedPNL_ ), realizedPNL( realizedPNL_ ),
      sExchange( sExchange_ ), sCurrency( sCurrency_ ) {}
  };
  typedef FastDelegate1<const PositionDetail&> OnPositionDetailHandler_t;
  OnPositionDetailHandler_t OnPositionDetailHandler;

  struct AccountValue {
    std::string sKey;
    std::string sVal;
    std::string sCurrency;
    std::string sAccountName;
    AccountValue(
      const std::string& sKey_, const std::string& sVal_, const std::string& sCurrency_, const std::string& sAccountName_
      ): sKey( sKey_ ), sVal( sVal_ ), sCurrency( sCurrency_ ), sAccountName( sAccountName_ ) {}
  };
  typedef FastDelegate1<const AccountValue&> OnAccountValueHandler_t;
  OnAccountValueHandler_t OnAccountValueHandler;

  pSymbol_t GetSymbol( long ContractId );  // query existance
  pSymbol_t GetSymbol( pInstrument_t instrument );  // query for and add if doesn't exist

  void BuildInstrumentFromContractDetails( const ContractDetails&, pInstrument_t& );

  void Sync( pInstrument_t ); // for now, ensures we have relevant market rules
  double GetInterval( const double price, const int rule );

  // TWS Specific events
  #include "client/EWrapper_prototypes.h"

protected:

  static const char* TickTypeStrings[];

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

  //void StartDepthWatch( pSymbol_t pSymbol );
  //void  StopDepthWatch( pSymbol_t pSymbol );

  void StartGreekWatch( pSymbol_t pSymbol );
  void  StopGreekWatch( pSymbol_t pSymbol );

private:

  EReaderOSSignal m_osSignal;
  std::unique_ptr<EClientSocket> m_pTWS;

  std::mutex m_mutexThreadSync;
  std::condition_variable m_cvThreadSync;
  bool m_bThreadSync;

  std::thread m_thrdIBMessages;

  void ConnectOptions( const std::string& );
  void processMessages();

  void Disconnect( bool bFinal, bool bIBInitiated, bool bUserInitiated );

  long m_time;
  int m_idClient; // for session uniqueness when multiple applications are connected to TWS

  std::stringstream m_ss;  // for OutputDebugStrings in background thread

  // stuff comes back from IB with ticker id so use this to look up symbol,
  //    which is stored in the map of the class from which we inherited
  std::vector<pSymbol_t> m_vTickerToSymbol;

  // given a contract id, see if we have a symbol assigned
  using mapContractToSymbol_t = std::map<long, pSymbol_t>;
  mapContractToSymbol_t m_mapContractToSymbol;

  void DecodeMarketHours( const std::string&, ptime& dtOpen, ptime& dtClose );

  // ===

  struct Request {
    reqId_t id;
    bool bIntransit;
    size_t cntEvictionByTimer;  // last ditch timed eviction
    pInstrument_t pInstrument;  // add info to existing pInstrument, future use with BuildInstrumentFromContract
    fOnContractDetail_t fOnContractDetail;
    fOnContractDetailDone_t fOnContractDetailDone;
    Contract contract; // used when having to resubmit
    std::chrono::time_point<std::chrono::system_clock> dtSubmitted; // submission turn-around calculation

    Request()
      : id {}
      , bIntransit( false )
      , cntEvictionByTimer {}
      , fOnContractDetail( nullptr )
      , fOnContractDetailDone( nullptr )
      {};

    void Clear() {
      id = 0;
      bIntransit = false;
      cntEvictionByTimer = 0;
      pInstrument.reset();
      fOnContractDetail = nullptr;
      fOnContractDetailDone = nullptr;
    }
  };

  reqId_t m_nxtReqId;
  bool m_bEvictorStarted;
  std::thread m_thrdRequestEvictor;
  std::mutex m_mutexActiveRequests;

  using vRequest_t = std::vector<Request*>;
  vRequest_t m_vRequestRecycling;

  using mapActiveRequests_t = std::map<reqId_t, Request*>;
  mapActiveRequests_t m_mapActiveRequests;

  void UpdateActiveRequests();

  // ====

  using mapExchangeMarketRule_t = std::map<std::string,std::string>;  // exchange name, rule id -- needs to be changed to reflect retrieval
  mapExchangeMarketRule_t m_mapExchangeMarketRule;

  using vPriceIncrement_t = std::vector<PriceIncrement>;
  using mapMarketRule_t = std::map<int,vPriceIncrement_t>;
  mapMarketRule_t m_mapMarketRule;

  bool MarketRuleExists( const int );

};

} // namespace ib
} // namespace tf
} // namespace ou
