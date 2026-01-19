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

#include <memory>
#include <string>
#include <chrono>
#include <iostream>
//#include <sstream>
#include <stdexcept>

#include <boost/log/trivial.hpp>

#include <boost/regex.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/local_time_adjustor.hpp>

#include <boost/spirit/include/qi.hpp>

#include <OUCommon/TimeSource.h>

#include <OUCommon/KeyWordMatch.h>

#include <TFTrading/KeyTypes.h>
#include <TFTrading/OrderManager.h>

#include "client/TagValue.h"
#include "client/OrderState.h"
#include "client/EClientSocket.h"
#include "client/CommissionReport.h"

#include "IBTWS.h"

namespace {

  const unsigned int maxRequestsInTransit( 5 );

// TODO: use spirit to parse?  will it be faster?
struct DecodeStatusWord {
  enum EStatus{ Unknown, PreSubmitted, PendingSubmit, PendingCancel, Submitted, Cancelled, Filled, Inactive };
  DecodeStatusWord( void ): kwm( Unknown, 50 ) {
    kwm.AddPattern( "Cancelled", Cancelled );
    kwm.AddPattern( "Filled", Filled );
    kwm.AddPattern( "Inactive", Inactive );
    kwm.AddPattern( "PreSubmitted", PreSubmitted );
    kwm.AddPattern( "Submitted", Submitted );
    kwm.AddPattern( "PendingSubmit", PendingSubmit );
    kwm.AddPattern( "PendingCancel", PendingCancel );
  }
  EStatus Match( const std::string& status ) { return kwm.FindMatch( status ); };
private:
  ou::KeyWordMatch<EStatus> kwm;;
};

DecodeStatusWord dsw;

namespace qi = boost::spirit::qi;
using vString_t = std::vector<std::string>;

template<typename Iterator>
struct ParseStrings: qi::grammar<Iterator, vString_t()> {

  ParseStrings(): ParseStrings::base_type( start ) {

    ruleString %= +( qi::char_ - qi::char_(",") );
    start %= ruleString >> ( *(qi::lit(',') >> ruleString ) );

  }

  qi::rule<Iterator, std::string()> ruleString;
  qi::rule<Iterator, vString_t()> start;

};

} // namespace anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace ib { // Interactive Brokers

TWS::TWS( const std::string &acctCode, const std::string &address, unsigned int port )
: EWrapper()
, ProviderInterface<TWS,Symbol>()
, m_osSignal( 1000 )
, m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 )
, m_idClient( 0 )
, m_nxtReqId( 1 )
, m_bEvictorStarted( false )
{
  m_sName = "IB";
  m_nID = keytypes::EProviderIB;
  pSymbol_t p;
  m_vTickerToSymbol.push_back( p );  // first ticker is 1, so preload with nothing at position 0
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
  m_bProvidesGreeks = true;
  m_bProvidesBrokerInterface = true;
}

TWS::~TWS() {

  if ( m_thrdRequestEvictor.joinable() ) {
    m_thrdRequestEvictor.join();
  }

  for ( vRequest_t::value_type& vt: m_vRequestRecycling ) {
    delete vt;
    vt = nullptr;
  }
  m_vRequestRecycling.clear();

  m_vTickerToSymbol.clear();
  m_mapContractToSymbol.clear();

  Disconnect( true, false, true );

}

void TWS::ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth ) {
  //std::string month( boost::lexical_cast<std::string,boost::uint16_t>( nMonth ) );
  contract.lastTradeDateOrContractMonth
    = boost::lexical_cast<std::string,boost::uint16_t>( nYear )
    //+ ( ( 1 == month.size() ) ? "0" : "" )
    //+ month
    += ( ( 9 < nMonth ) ? "" : "0" ) + boost::lexical_cast<std::string>( nMonth );
    ;
}

void TWS::ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {
  ContractExpiryField( contract, nYear, nMonth );
  contract.lastTradeDateOrContractMonth
    //+= boost::lexical_cast<std::string,boost::uint16_t>( nDay )
    += ( ( 9 < nDay ) ? "" : "0" ) + boost::lexical_cast<std::string>( nDay );
    ;
}

void TWS::Connect() {

    if ( m_pTWS ) {
      BOOST_LOG_TRIVIAL(info) << "IB EClientSocket exists";
    }
    else {
      m_pTWS = std::make_unique<EClientSocket>( this, &m_osSignal );
    }

    if ( m_pTWS->isConnected() ) {
      assert( m_thrdIBMessages.joinable() );
    }
    else {
      OnConnecting( 0 );

      if ( m_thrdIBMessages.joinable() ) { // clean out previous stuff
        m_thrdIBMessages.join();
      }

      //m_pTWS->setConnectOptions( "+PACEAPI" ); //  https://www.interactivebrokers.com/en/index.php?f=24356 (needs version >=974)
      bool bOk = m_pTWS->eConnect( m_sIPAddress.c_str(), m_nPort, m_idClient );
      if ( bOk ) {

        m_bConnected = true;

        {
          std::unique_lock lock( m_mutexThreadSync );

          m_bThreadSync = false;
          m_thrdIBMessages = std::move( std::thread( std::bind( &TWS::processMessages, this ) ) );
          m_cvThreadSync.wait(
            lock,
            [this](){
              if ( m_bThreadSync ) {
                //std::cout << "thread notified" << std::endl;
                m_pTWS->reqCurrentTime();
                usleep( 50000 );
                m_pTWS->reqNewsBulletins( true );
          //     m_pTWS->reqOpenOrders();
                //ExecutionFilter filter;
                //pTWS->reqExecutions( filter );
                usleep( 50000 );
                m_pTWS->reqAccountUpdates( true, "" );

                OnConnected( 0 );
              }
              return m_bThreadSync;
            } );
        }
      }
      else {
        // need to integrate common stuff from Disconnect, most everything but the thread bit
        std::cout << "TWS::Connect fail" << std::endl;
        Disconnect( false, false, true );
      }
    }
}

// this is executed in non-main thread, and the events below will be called from the processing here
// so... be aware of cross thread issues
void TWS::processMessages() {

  std::unique_ptr<EReader> pReader;
  pReader = std::make_unique<EReader>( m_pTWS.get(), &m_osSignal );
  pReader->start();

  {
    std::scoped_lock lock( m_mutexThreadSync );
    m_bThreadSync = true;
  }
  //std::cout << "thread notifying" << std::endl;
  m_cvThreadSync.notify_one();

  try { // should this be within the while?
    while ( m_bConnected ) {

      errno = 0;
      m_osSignal.waitForSignal();
      pReader->processMsgs();

      switch ( errno ) {
        case 0:  // ignore, heartbeat?
          break;
        case 2:  // ignore, per-message signal
          //BOOST_LOG_TRIVIAL(debug) << "IB processMessages 2";
          errno = 0;
          break;
        default:
          BOOST_LOG_TRIVIAL(error)
            << "IB status id=" << errno << " [" << strerror(errno) << "]"
            << ",connected=" << m_pTWS->isConnected()
            ;
          errno = 0;
      }
    }

  }
  catch(...) {
    BOOST_LOG_TRIVIAL(error) << "IB socket failure, need to disconnect and restart ...";
    // probably need to run Disconnect or DisconnectCommon
    m_bConnected = false;  // placeholder for debug
  }

  //BOOST_LOG_TRIVIAL(debug) << "IB pReader.reset()";
  pReader.reset(); // NOTE: this deadlocks with connectionClosed()

  //BOOST_LOG_TRIVIAL(debug) << "IB processMessages exit";

}

void TWS::Disconnect() {
  Disconnect( false, false, true );
}

void TWS::Disconnect( bool bFinal, bool bIBInitiated, bool bUserInitiated ) {

  assert( bIBInitiated != bUserInitiated );

  // TODO: check to see if there are any watches happening, and get them disconnected

  if ( m_pTWS ) {  // called by IB, so deadlock when exiting

    //BOOST_LOG_TRIVIAL(debug) << "IB Disconnecting";
    OnDisconnecting( 0 );
    m_bConnected = false;

    if ( bUserInitiated ) {
      //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect SignalEnd edisconnect pre";
      m_pTWS->eDisconnect( false );
      //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect SignalEnd edisconnect post";
    }

    if ( bUserInitiated ) { // can't join in connectionClosed thread, will deadlock pReader.reset()
      //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect SignalEnd threadjoin pre";
      if ( m_thrdIBMessages.joinable() ) {
        m_thrdIBMessages.join();  // wait for message processing to exit
      }
      //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect delete pre";
      m_pTWS.reset();  // NOTE: since it still exists, re-use it when possible (exists with bIBInitiated)
      //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect delete post";
    }

    //BOOST_LOG_TRIVIAL(debug) << "IB Disconnect signal pre";
    OnDisconnected( 0 );

  }
  else {
    BOOST_LOG_TRIVIAL(warning) << "IB already disconnected";
  }
}

void TWS::connectionClosed() {
  BOOST_LOG_TRIVIAL(info) << "IB Initiated Connection Close";
  Disconnect( false, true, false );
}

void TWS::RequestContractDetails(
  const std::string& sSymbolBaseName
, pInstrument_t& pInstrument
, fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone
) {
  assert( 0 == pInstrument->GetContract() );  // handle this better, ie, return gently, or create exception
  Contract contract;
  contract.symbol = sSymbolBaseName;  // separately, as it may differ from IQFeed or others
  contract.currency = pInstrument->GetCurrencyName(); // check if these match
  if ( pInstrument->GetExchangeName().empty() ) {
    contract.exchange = "SMART";
  }
  else {
    contract.exchange = pInstrument->GetExchangeName();
  }
  //contract.exchange = pInstrument->GetExchangeName();  // need lookup table from IQFeed exchanges
  //std::cout << "Exchange supplied to TWS: " << pInstrument->GetExchangeName() << std::endl;
  contract.secType = szSecurityType[ pInstrument->GetInstrumentType() ];
  switch ( pInstrument->GetInstrumentType() ) {
  case InstrumentType::Stock:
    contract.exchange = "SMART"; // override iqfeed supplied value
    break;
  case InstrumentType::Option:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    contract.strike = pInstrument->GetStrike();
    contract.right = pInstrument->GetOptionSide();
    contract.exchange = "SMART"; // override iqfeed supplied value
    //contract.multiplier = boost::lexical_cast<std::string>( pInstrument->GetMultiplier() );  // needed for small options
    break;
  case InstrumentType::Future:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    contract.tradingClass = sSymbolBaseName;
    //if ( "COMEX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC options, IQFeed supplied
    //if ( "COMEX_GBX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC options, IQFeed supplied
    //if ( "CME" == pInstrument->GetExchangeName() ) contract.exchange = "GLOBEX";   // ES options, IQFeed supplied
    break;
  case InstrumentType::FuturesOption:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    contract.tradingClass = sSymbolBaseName;
    contract.strike = pInstrument->GetStrike();
    contract.right = pInstrument->GetOptionSide();
    //if ( "COMEX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC, IQFeed supplied
    //if ( "COMEX_GBX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC, IQFeed supplied
    //if ( "CME" == pInstrument->GetExchangeName() ) contract.exchange = "GLOBEX";   // ES?, IQFeed supplied
    break;
  case InstrumentType::Currency:
    //contract.exchange = "IDEAL"; // can be IDEAL (paper) or IDEALPRO (real)
    contract.secType = "CASH";
    contract.symbol = ou::tf::Currency::Name[ pInstrument->GetCurrencyBase() ];
    contract.currency = ou::tf::Currency::Name[ pInstrument->GetCurrencyCounter() ];
    break;
  default:
    assert( false );
  }
  RequestContractDetails( contract, std::move( fProcess ), std::move( fDone ), pInstrument );
}

void TWS::RequestContractDetails( const Contract& contract, fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone ) {
  // results supplied at contractDetails()
  pInstrument_t pInstrument;  // just allocate, and pass as empty
  RequestContractDetails( contract, std::move( fProcess ), std::move( fDone ), pInstrument );
}

void TWS::RequestContractDetails(
  const Contract& contract, fOnContractDetail_t&& fProcess, fOnContractDetailDone_t&& fDone, pInstrument_t& pInstrument
) {

  // pInstrument can be empty, or can have an instrument
  // results supplied at contractDetails()

  bool bStartEvictor( false );
  Request* pRequest = nullptr;
  mapActiveRequests_t::size_type size {};

  //std::cout << "Requesting " << pInstrument->GetInstrumentName() << std::endl;

  {
    std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );

    if ( 0 == m_vRequestRecycling.size() ) {
      pRequest = new Request();
    }
    else {
      pRequest = m_vRequestRecycling.back();
      m_vRequestRecycling.pop_back();
    }

    pRequest->id = m_nxtReqId++;
    pRequest->bIntransit = false;
    pRequest->fOnContractDetail = std::move( fProcess );
    pRequest->fOnContractDetailDone = std::move( fDone );
    pRequest->pInstrument = pInstrument;
    pRequest->contract = contract;
    pRequest->dtSubmitted = std::chrono::system_clock::now();

    m_mapActiveRequests[ pRequest->id ] = pRequest;

    if ( !m_bEvictorStarted ) {
      bStartEvictor = true;
    }
  } // end scoped_lock

  if ( bStartEvictor ) {
    //std::cout << "Evictor starting" << std::endl;

    if ( m_thrdRequestEvictor.joinable() ) { // wait for previous thread to complete
      m_thrdRequestEvictor.join();
    }

    m_bEvictorStarted = true;

    // TODO: move this into UpdateActiveRequests()?
    m_thrdRequestEvictor = std::move( std::thread(
      [this](){
        bool bContinue( true );
        fOnContractDetailDone_t fOnContractDetailDone( nullptr );
        do {
          using namespace std::chrono_literals;
          vRequest_t vRequestsForEvictionNotify;
          std::this_thread::sleep_for( 50ms );

          {
            std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );

            for ( mapActiveRequests_t::value_type& vt: m_mapActiveRequests ) {
              Request* pRequest = vt.second;
              assert( 0 != pRequest->id );
              if ( !pRequest->bIntransit ) break;

              pRequest->cntEvictionByTimer++;
              if ( 20 < pRequest->cntEvictionByTimer ) { // 20 x 50ms is 1 second
                std::chrono::time_point<std::chrono::system_clock> finished
                  = std::chrono::system_clock::now();
                std::chrono::duration<double, std::milli> elapsed = finished - pRequest->dtSubmitted;
                BOOST_LOG_TRIVIAL(error)
                  << "IB details failed (timed) id "
                  <<        pRequest->id
                  << "," << pRequest->cntEvictionByTimer
                  << "," << pRequest->pInstrument->GetInstrumentName()
                  << "," << elapsed.count() << "ms"
                  ;
                //m_mapActiveRequests.erase( pRequest->id );
                vRequestsForEvictionNotify.push_back( pRequest );
              }
            }

            bContinue = ( 0 < m_mapActiveRequests.size() );
            if ( !bContinue ) {
              m_bEvictorStarted = false;
            }
          } // end scoped_lock

          for ( vRequest_t::value_type pRequest: vRequestsForEvictionNotify ) {
            fOnContractDetailDone = std::move( pRequest->fOnContractDetailDone );
            if ( fOnContractDetailDone ) { // need to call endofcontract to clean this up
              fOnContractDetailDone( false );
              fOnContractDetailDone = nullptr;
            }
          }

          {
            std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );
            for ( vRequest_t::value_type pRequest: vRequestsForEvictionNotify ) {
              m_mapActiveRequests.erase( pRequest->id ); // comes prior to clear
              pRequest->Clear();
              m_vRequestRecycling.push_back( pRequest );
            }
          }

          if ( bContinue ) {
            UpdateActiveRequests();
          }

        }
        while ( bContinue );
        //std::cout << "Evictor stopping" << std::endl;
      }
    )); // end eviction thread definition
  } // if bStartEvictor

  UpdateActiveRequests();

}

void TWS::UpdateActiveRequests() {

  vRequest_t vRequestsToSubmit;

  {
    std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );

    unsigned int cntInTransit {};

    for ( mapActiveRequests_t::value_type& vt: m_mapActiveRequests ) {
      Request* pRequest = vt.second;
      assert( 0 != pRequest->id );
      if ( pRequest->bIntransit ) {
        cntInTransit++;
      }
      else {
        if ( maxRequestsInTransit > cntInTransit ) {
          vRequestsToSubmit.push_back( pRequest );
          pRequest->bIntransit = true;
          cntInTransit++;
        }
        else {
          break;
        }
      }
    }
  }

  for ( vRequest_t::value_type pRequest: vRequestsToSubmit ) {
    m_pTWS->reqContractDetails( pRequest->id, pRequest->contract );
  }

}

//IBSymbol *TWS::NewCSymbol( const std::string &sSymbolName ) {
TWS::pSymbol_t TWS::NewCSymbol( Symbol::pInstrument_t pInstrument ) {
  // todo:  check that contract doesn't already exist
  TickerId ticker = ++m_curTickerId;
//  pSymbol_t pSymbol( new Symbol( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIB ), pInstrument, ticker ) );  // is there someplace with the IB specific symbol name, or is it set already?  (this simply creates the object, no additional function here)
  pSymbol_t pSymbol( new Symbol( pInstrument->GetInstrumentName( ID() ), pInstrument, ticker ) );  // is there someplace with the IB specific symbol name, or is it set already?  (this simply creates the object, no additional function here)
  // todo:  do an existance check on the instrument/symbol
  ProviderInterface<TWS,Symbol>::AddCSymbol( pSymbol );
  m_vTickerToSymbol.push_back( pSymbol );
  m_mapContractToSymbol.insert( mapContractToSymbol_t::value_type( pInstrument->GetContract(), pSymbol ) );
  return pSymbol;
}

void TWS::StartQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void TWS::StopQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void TWS::StartTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void TWS::StopTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void TWS::StartGreekWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void TWS::StopGreekWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void TWS::StartQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( !pIBSymbol->GetQuoteTradeWatchInProgress() ) {
    // start watch
    Contract contract;
    contract.conId = pIBSymbol->GetInstrument()->GetContract();  // mostly enough to have contract id
    contract.exchange = pIBSymbol->GetInstrument()->GetExchangeName();
    contract.currency = pIBSymbol->GetInstrument()->GetCurrencyName();
    pIBSymbol->SetQuoteTradeWatchInProgress();
    //pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "100,101,104,165,221,225,236", false );
    TagValueListSPtr pMktDataOptions;
    m_pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "", false, false, pMktDataOptions );
  }
}

void TWS::StopQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( pIBSymbol->QuoteWatchNeeded() || pIBSymbol->TradeWatchNeeded() || pIBSymbol->GreekWatchNeeded() ) {
    // don't do anything if either a quote or trade or greek watch still in progress
  }
  else {
    // stop watch
    m_pTWS->cancelMktData( pIBSymbol->GetTickerId() );
    pIBSymbol->ResetQuoteTradeWatchInProgress();
  }
}

//void TWS::StartDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
//  if ( !pIBSymbol->GetDepthWatchInProgress() ) {
//    // start watch
//    pIBSymbol->SetDepthWatchInProgress();
//  }
//}

//void TWS::StopDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
//  if ( pIBSymbol->DepthWatchNeeded() ) {
//  }
//  else {
//    // stop watch
//    pIBSymbol->ResetDepthWatchInProgress();
//  }
//}

// indexed with InstrumentType::EInstrumentType
const char *TWS::szSecurityType[] = {
  "NULL", "STK", "OPT", "FUT", "FOP", "CASH", "IND" };  // InsrumentType::EInstrumentType
const char *TWS::szOrderType[] = {
  "UNKN", "MKT", "LMT", "STP", "STPLMT", "NULL",        // OrderType::enumOrderType
  "TRAIL", "TRAILLIMIT", "MKTCLS", "LMTCLS", "SCALE" };

void TWS::PlaceOrder( pOrder_t pOrder ) {

  ::Order twsorder;
  twsorder.orderId  = pOrder->GetOrderId();
  twsorder.parentId = pOrder->GetParentOrderId();

  Contract contract;
  contract.conId = pOrder->GetInstrument()->GetContract();  // mostly enough to have contract id
  contract.exchange = pOrder->GetInstrument()->GetExchangeName();
  contract.currency = pOrder->GetInstrument()->GetCurrencyName();

  std::string s;
  switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
    case InstrumentType::Stock:
      contract.exchange = "SMART";
      break;
    case InstrumentType::Future:
      //
//      s.Format( "%04d%02d", pOrder->GetInstrument()->GetExpiryYear(), pOrder->GetInstrument()->GetExpiryMonth() );
//      contract.lastTradeDateOrContractMonth = s;
      //if ( "CBOT" == contract.exchange ) contract.exchange = "ECBOT";
      if ( 0 != pOrder->GetInstrument()->GetContract() ) {
      }
      else {
        assert( false );  // need to fix this formatter, use the boost::gregorian date stuff
      }
      break;
    case InstrumentType::Currency:
      //twsorder.cashQty = 20000;
      //contract.currency = "";
      contract.secType = "CASH";
      contract.symbol = Currency::Name[pOrder->GetInstrument()->GetCurrencyBase()];
      contract.currency = Currency::Name[pOrder->GetInstrument()->GetCurrencyCounter()];
      break;
    default:
      break;
  }
  twsorder.action = pOrder->GetOrderSideName();
  twsorder.totalQuantity = __bid64_from_uint32( pOrder->GetQuanOrdered() );
  twsorder.orderType = szOrderType[ pOrder->GetOrderType() ];

  twsorder.tif = ou::tf::TimeInForce::Name[ pOrder->GetTimeInForce() ];

  switch ( pOrder->GetTimeInForce() ) {
    case ou::tf::ETimeInForce::GoodTillDate:
      {
        //twsorder.goodTillDate = "20080625 16:00:00";
        ptime dt( pOrder->GetGoodTillDate() );
        std::stringstream ss;
        ss << dt.time_of_day() << " UTC";
        twsorder.goodTillDate
          = ou::tf::Instrument::BuildDate( dt.date() )
          + " "
          + ss.str();
      }
      break;
    default:
      break;
  }
  switch ( pOrder->GetTimeInForce() ) { // will need to confirm how this is signalled
    case ou::tf::ETimeInForce::GoodAfterTime:
      {
        //twsorder.goodAfterTime = "20080625 09:30:00";
        ptime dt( pOrder->GetGoodAfterTime() );
        std::stringstream ss;
        ss << dt.time_of_day();
        twsorder.goodAfterTime
          = ou::tf::Instrument::BuildDate( dt.date() )
          + " "
          + ss.str();
      }
      break;
    default:
      break;
  }

  switch ( pOrder->GetOrderType() ) {
    case OrderType::Limit:
      twsorder.lmtPrice = pOrder->GetPrice1();
      twsorder.auxPrice = 0;
      break;
    case OrderType::Stop:
      twsorder.lmtPrice = 0;
      twsorder.auxPrice = pOrder->GetPrice1();
      break;
    case OrderType::Trail:
      twsorder.lmtPrice = 0;
      twsorder.trailStopPrice = pOrder->GetPrice1(); // stop price
      twsorder.auxPrice = pOrder->GetPrice2(); // trail amount
      break;
    case OrderType::StopLimit:
      twsorder.lmtPrice = pOrder->GetPrice1();
      twsorder.auxPrice = pOrder->GetPrice2();
      break;
    default:
      twsorder.lmtPrice = 0;
      twsorder.auxPrice = 0;
  }
  twsorder.transmit   = pOrder->GetTransmit();
  twsorder.outsideRth = pOrder->GetOutsideRTH();
  //twsorder.whatIf = true;

  ProviderInterface<TWS,Symbol>::PlaceOrder( pOrder ); // any underlying initialization
  m_pTWS->placeOrder( twsorder.orderId, contract, twsorder );
}

void TWS::PlaceComboOrder( pOrder_t pOrderEntry, pOrder_t pOrderStop ) {
  pOrderEntry->SetTransmit( false );
  PlaceOrder( pOrderEntry );
  pOrderStop->SetParentOrderId( pOrderEntry->GetOrderId() );
  PlaceOrder( pOrderStop );
}

void TWS::PlaceBracketOrder( pOrder_t pOrderEntry, pOrder_t pOrderProfit, pOrder_t pOrderStop ) {
  pOrderEntry->SetTransmit( false );
  PlaceOrder( pOrderEntry );         // limit or market
  pOrderProfit->SetParentOrderId( pOrderEntry->GetOrderId() );
  pOrderProfit->SetTransmit( false );
  PlaceOrder( pOrderProfit );        // limit
  pOrderStop->SetParentOrderId( pOrderEntry->GetOrderId() );
  PlaceOrder( pOrderStop );          // stop or trail
}

void TWS::PlaceBagOrder( const Bag& bag ) {

  // https://www.interactivebrokers.com/campus/trading-lessons/python-complex-orders/

  assert( 0 < bag.vOrder.size() );

  ::Contract contract;
  contract.symbol = bag.name;
  contract.secType = "BAG";
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.comboLegs = std::make_shared<Contract::ComboLegList>();

  // definitions for reference
  // typedef std::shared_ptr<ComboLeg> ComboLegSPtr;
	// typedef std::vector<ComboLegSPtr> ComboLegList;
	// typedef std::shared_ptr<ComboLegList> ComboLegListSPtr;

  for ( auto& pOrder: bag.vOrder ) {
    ou::tf::Order& order( *pOrder );
    ComboLegSPtr leg = std::make_shared<ComboLeg>();
    leg->conId = order.GetInstrument()->GetContract();
    leg->ratio = order.GetQuanOrdered();
    //leg->ratio = 1;
    leg->action = order.GetOrderSideName();
    //leg->exchange = "SMART";
    switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
      case InstrumentType::Future:
      case InstrumentType::FuturesOption:
        leg->exchange = pOrder->GetInstrument()->GetExchangeName();
        break;
      default:
        leg->exchange = "SMART";
        break;
    }
    contract.comboLegs->push_back( leg );
  }

  ::Order order;
  //order.orderId = bag.order_id;
  order.orderId = 0;
  order.action = ou::tf::OrderSide::Name[ bag.side ];
  order.orderType = szOrderType[ bag.type ];
  order.lmtPrice = bag.price;
  order.auxPrice = 0;
  order.totalQuantity = __bid64_from_uint32( bag.quantity );
  order.tif = bag.tif;
  order.outsideRth = bag.outside_rth;
  order.transmit = true;

  order.smartComboRoutingParams = std::make_shared<::TagValueList>();
  order.smartComboRoutingParams->push_back( std::make_shared<::TagValue>( "NonGuaranteed", "1" ) );

  m_pTWS->placeOrder( bag.order_id, contract, order );

}

void TWS::CancelOrder( pOrder_t pOrder ) {
  ProviderInterface<TWS,Symbol>::CancelOrder( pOrder );
  m_pTWS->cancelOrder( pOrder->GetOrderId() );
}

void TWS::tickPrice( TickerId tickerId, TickType tickType, double price, const TickAttrib& attrib ) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    Symbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickPrice " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << price << std::endl;
    pSym->AcceptTickPrice( tickType, price );
  }
}

void TWS::tickSize( TickerId tickerId, TickType tickType, Decimal size ) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    Symbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickSize " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << size << std::endl;
    pSym->AcceptTickSize( tickType, size );
  }
}

void TWS::tickOptionComputation(  TickerId tickerId, TickType tickType, int tickAttrib, double impliedVol, double delta,
	double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice ) {

  Symbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
  switch ( tickType ) {
    case MODEL_OPTION:
      pSym->Greeks( optPrice, undPrice, pvDividend, impliedVol, delta, gamma, vega, theta );
      break;
    case BID_OPTION_COMPUTATION:
      break;
    case ASK_OPTION_COMPUTATION:
      break;
    case LAST_OPTION_COMPUTATION:
      break;
    default:
      break;
  }
}

void TWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
//  std::cout << "tickGeneric " << m_vTickerToSymbol[ tickerId ]->Name() << ", " << TickTypeStrings[tickType] << ", " << value << std::endl;
}

void TWS::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    Symbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickString " << pSym->Name() << ", "
    //  << TickTypeStrings[tickType] << ", " << value;
    //std::cout << std::endl;
    pSym->AcceptTickString( tickType, value );
  }
}

void TWS::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
  double totalDividends, int holdDays, const std::string& futureExpiry, double dividendImpact, double dividendsToExpiry ) {
  m_ss.str("");
  m_ss << "tickEFP" << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void TWS::openOrder( ::OrderId orderId, const ::Contract& contract, const ::Order& order, const ::OrderState& state) {
  if ( order.whatIf ) {
    m_ss.str("");
    m_ss
      << "WhatIf:  ordid=" << orderId << ", cont.sym=" << contract.symbol
      << ", state.commission=" << state.commission
      << " " << state.commissionCurrency
      << ", state.equityWithLoanBefore=" << state.equityWithLoanBefore
      << ", state.equityWithLoanChange=" << state.equityWithLoanChange
      << ", state.equityWithLoanAfter=" << state.equityWithLoanAfter
      << ", state.initMarginBefore=" << state.initMarginBefore
      << ", state.initMarginChange=" << state.initMarginChange
      << ", state.initMarginAfter=" << state.initMarginAfter
      << ", state.maintMarginBefore=" << state.maintMarginBefore
      << ", state.maintMarginChange=" << state.maintMarginChange
      << ", state.maintMarginAfter=" << state.maintMarginAfter
      << ", state.maxcom=" << state.maxCommission
      << ", state.mincom=" << state.minCommission
      << std::endl;
//    OutputDebugString( m_ss.str().c_str() );
  }
  else {
    m_ss.str("");
    m_ss
      << "OpenOrder: ordid=" << orderId
      << ", state.stat=" << state.status
      << ", cont.sym=" << contract.symbol
      << ", order.action=" << order.action
      << ", state.commission=" << state.commission
      << " " << state.commissionCurrency
      //<< ", ord.id=" << order.orderId
      //<< ", ord.ref=" << order.orderRef
      //<< ", state.warning=" << state.warningText
      << std::endl;
//    std::cout << m_ss.str();  // ****
    //OutputDebugString( m_ss.str().c_str() );
    //if ( std::numeric_limits<double>::max(0) != state.commission )
    if ( 1e308 > state.commission )
      // reports total commission for order rather than increment
      OrderManager::GlobalInstance().ReportCommission( orderId, state.commission );
    // use spirit to do this to make it faster with a trie, or use keyword match
    DecodeStatusWord::EStatus status = dsw.Match( state.status );
    switch ( status ) {
      case DecodeStatusWord::Submitted:
        break;
      case DecodeStatusWord::Filled:
        break;
      case DecodeStatusWord::Cancelled:
        OrderManager::GlobalInstance().ReportCancellation( order.orderId );
        break;
      case DecodeStatusWord::Inactive:
        break;
      case DecodeStatusWord::PreSubmitted:
        break;
      case DecodeStatusWord::PendingSubmit:  // coincides with popup in TWS, can't remember what the message was, probably trading outside of regular hours, or due to something similar as described in next comment
        break;
      case DecodeStatusWord::PendingCancel:  // pendingsubmit, pendingcancel happened for order to short, but IB didn't have anything to loan for shorting
        break;
      case DecodeStatusWord::Unknown:
        assert( false );
        break;
    }
  }
  if ( state.warningText != "" ) {
    m_ss.str("");
    m_ss << "Open Order Warning: " << state.warningText << std::endl;
//    OutputDebugString( m_ss.str().c_str() );
  }
}

void TWS::orderStatus( OrderId orderId, const std::string& status, Decimal filled,
                         Decimal remaining, double avgFillPrice, int permId, int parentId,
                         double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice )
{
  if ( false ) {
    m_ss.str("");
    m_ss
      << "OrderStatus: ordid=" << orderId
      << ", stat=" << status
      << ", filled=" << decimalStringToDisplay( filled )
      << ", rem=" << decimalStringToDisplay( remaining )
      << ", avgfillprc=" << avgFillPrice
      << ", permid=" << permId
      //<< ", parentid=" << parentId
      << ", lfp=" << lastFillPrice
      //<< ", clid=" << clientId
      //<< ", yh=" << whyHeld
      << std::endl;
    //std::cout << m_ss.str();  // ****
//    OutputDebugString( m_ss.str().c_str() );
  }
  DecodeStatusWord::EStatus status_ = dsw.Match( status );
  switch ( status_ ) {
    case DecodeStatusWord::Cancelled:
      OrderManager::GlobalInstance().ReportCancellation( orderId );
      break;
    case DecodeStatusWord::Submitted:
    case DecodeStatusWord::Filled:
      break;
    case DecodeStatusWord::PreSubmitted:
      break;
    case DecodeStatusWord::PendingCancel:
      break;
    default:
      std::cout << "TWS::orderStatus: " << orderId << "," << status << std::endl;
  }
}

void TWS::execDetails( int reqId, const ::Contract& contract, const ::Execution& execution ) {
  m_ss.str("");
  m_ss
    << "execDetails: "
    << "  sym=" << contract.localSymbol
//    << ", oid=" << orderId
    << ", reqId=" << reqId
    << ", ex.oid=" << execution.orderId
    << ", ex.pr=" << execution.price
    << ", ex.sh=" << execution.shares
    << ", ex.sd=" << execution.side
    << ", ex.ti=" << execution.time
    << ", ex.ex=" << execution.exchange
    //<< ", ex.liq=" << execution.liquidation
    << ", ex.pid=" << execution.permId
    << ", ex.acct=" << execution.acctNumber
    //<< ", ex.clid=" << execution.clientId
    << ", ex.xid=" << execution.execId
    << std::endl;
//  std::cout << m_ss.str();  // ****
//  OutputDebugString( m_ss.str().c_str() );

  OrderSide::EOrderSide side = OrderSide::Unknown;
  if ( "BOT" == execution.side ) side = OrderSide::Buy;  // could try just first character for fast comparison
  if ( "SLD" == execution.side ) side = OrderSide::Sell;
  if ( OrderSide::Unknown == side ) {
    m_ss.str("");
    m_ss << "Unknown execution side: " << execution.side << std::endl;
//    OutputDebugString( m_ss.str().c_str() );
  }
  else {
    ou::tf::Execution exec( execution.price, execution.shares, side, execution.exchange, execution.execId );
    OrderManager::GlobalInstance().ReportExecution( execution.orderId, exec );
  }
}

void TWS::execDetailsEnd( int reqId) {}


/*

OrderStatus: ordid=1057, stat=Filled, fild=200, rem=0, avgfillprc=117, permid=2147126208, parentid=0, lfp=117, clid=0, yh=
OpenOrder: ordid=1057, cont.sym=ICE, ord.id=1057, ord.ref=, state.stat=Filled, state.warning=, order.action=BUY, state.commission=1 USD
OrderStatus: ordid=1057, stat=Filled, fild=200, rem=0, avgfillprc=117, permid=2147126208, parentid=0, lfp=117, clid=0, yh=
OpenOrder: ordid=1057, cont.sym=ICE, ord.id=1057, ord.ref=, state.stat=Filled, state.warning=, order.action=BUY, state.commission=1 USD
execDetails:   sym=ICE, oid=1057, ex.oid=1057, ex.pr=117, ex.sh=100, ex.sd=BOT, ex.ti=20080607  12:39:55, ex.ex=NYSE, ex.liq=0, ex.pid=2147126208, ex.acct=DU15067, ex.clid=0, ex.xid=0000ea6a.44f438e4.01.01
OrderStatus: ordid=1057, stat=Submitted, fild=100, rem=100, avgfillprc=117, permid=2147126208, parentid=0, lfp=117, clid=0, yh=
OpenOrder: ordid=1057, cont.sym=ICE, ord.id=1057, ord.ref=, state.stat=Submitted, state.warning=, order.action=BUY, state.commission=1 USD
OrderStatus: ordid=1057, stat=Submitted, fild=100, rem=100, avgfillprc=117, permid=2147126208, parentid=0, lfp=117, clid=0, yh=
OpenOrder: ordid=1057, cont.sym=ICE, ord.id=1057, ord.ref=, state.stat=Submitted, state.warning=, order.action=BUY, state.commission=1.79769e+308 USD
execDetails:   sym=ICE, oid=1057, ex.oid=1057, ex.pr=117, ex.sh=100, ex.sd=BOT, ex.ti=20080607  12:39:14, ex.ex=NYSE, ex.liq=0, ex.pid=2147126208, ex.acct=DU15067, ex.clid=0, ex.xid=0000ea6a.44f438d5.01.01
current time 1212851947
*/

// check for symbol existance and return, else exeption
TWS::pSymbol_t TWS::GetSymbol( long ContractId ) {
  mapContractToSymbol_t::iterator iterId = m_mapContractToSymbol.find( ContractId );
  if ( m_mapContractToSymbol.end() == iterId ) {
    throw std::out_of_range( "can't find contract" );
  }
  return iterId->second;
}

// check for symbol existance, and return, else add and return
TWS::pSymbol_t TWS::GetSymbol( pInstrument_t instrument ) {

  long contractId;
  contractId = instrument->GetContract();
  //assert( 0 != contractId );
  if ( 0 == contractId ) {
    throw std::runtime_error( "TWS::GetSymbol: contract id not supplied" );
  }

  pSymbol_t pSymbol;

  mapContractToSymbol_t::iterator iterId = m_mapContractToSymbol.find( contractId );
  if ( m_mapContractToSymbol.end() == iterId ) {
    pSymbol = NewCSymbol( instrument );
  }
  else {
    pSymbol = iterId->second;
  }

  return pSymbol;
}

void TWS::error(const int id, const int errorCode, const std::string& errorString) {
  switch ( errorCode ) {
    case 103: // Duplicate order id
        // id is the order number
      BOOST_LOG_TRIVIAL(error) << "IB error (1) " << id << ", " << errorCode << ", " << errorString;
      break;
    case 110: // The price does not conform to the minimum price variation for this contract.
        // id is the order number
        // TODO something like:  OrderManager::GlobalInstance().ReportCancellation( orderId );
      BOOST_LOG_TRIVIAL(error) << "IB error (2)" << id << ", " << errorCode << ", " << errorString;
      break;
    case 202:
      BOOST_LOG_TRIVIAL(info) << "IB order id " << id << ", " << errorString;
      break;
    case 478:  // IB error (3): 4, 478, Parameters in request conflicts with contract parameters received by contract id: requested currency NZD, in contract USD;
      BOOST_LOG_TRIVIAL(info) << "IB error (478): order id " << id << ", " << errorString;
      // TODO cancel the order
      break;
    case 1102: // Connectivity has been restored
      m_pTWS->reqAccountUpdates( true, "" );
      break;
    case 2104:  // datafarm connected ok
      break;
    case 200:  // no security definition has been found
      if ( 0 != OnSecurityDefinitionNotFound ) OnSecurityDefinitionNotFound();
      break;
    case 2106: //
    case 2158: //
      BOOST_LOG_TRIVIAL(warning) << "IB status " << errorCode << ", " << errorString;
      break;
    default:
      BOOST_LOG_TRIVIAL(error) << "IB error (3): " << id << ", " << errorCode << ", " << errorString;
      break;
  }
}

void TWS::winError( const std::string& str, int lastError) {
  //m_ss.str("");
  //m_ss << "winerror " << str << ", " << lastError << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
  BOOST_LOG_TRIVIAL(error) << "winerror " << str << ", " << lastError;
}

void TWS::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {
  //m_ss.str("");
  std::cout << "news bulletin " << msgId << "-" << msgType << "-" << originExch << ": " << newsMessage  << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void TWS::currentTime( long time ) {
  //m_ss.str("");
  //m_ss << "current time " << time << std::endl;
  //std::cout << m_ss.str() << std::endl;
  m_time = time;
}

void TWS::updateAccountTime (const std::string& timeStamp) {
  // about once a minute
}

void TWS::position( const std::string& account, const Contract & contract, Decimal position, double avgCost ) {
}

void TWS::positionEnd(void) {
}

void TWS::accountSummary( int i, const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4 ) {
}

void TWS::accountSummaryEnd( int i ) {
}

void TWS::marketDataType( TickerId id , int i ) {
}

void TWS::commissionReport( const CommissionReport& cr ) {
  //std::cout << "commissionReport " << cr.execId << ", " << cr.commission << ", " << cr.currency << ", " << cr.realizedPNL << std::endl;
}

// convert to boost::spirit?
// 2021/11/02: 20211102:1700-20211103:1500
void TWS::DecodeMarketHours( const std::string& mh, ptime& dtOpen, ptime& dtClose ) {
  //static const boost::regex rxFields( "([^:]+):([^;]+);([^:]+):(.+)" );
  static const boost::regex rxRange( "([0-9]{4})([0-9]{2})([0-9]{2}):([^;]+);" );
  //static const boost::regex rxFields( "([0-9]{4})([0-9]{2})([0-9]{2}):([^;]+);([0-9]{4})([0-9]{2})([0-9]{2}):(.+)" );
  //static const boost::regex rxTime( "([0-9]{4})-([0-9]{4})(?:,([0-9]{4})-([0-9]{4}))?" );
  static const boost::regex rxTime( "([0-9]{2})([0-9]{2})-([0-9]{4})([0-9]{2})([0-9]{2}):([0-9]{2})([0-9]{2})" );
  dtOpen = dtClose = boost::posix_time::special_values::not_a_date_time;
  boost::cmatch what;
  if ( !boost::regex_search( mh.c_str(), what, rxRange ) ) {
    std::runtime_error( "no proper market info found" );
  }
  else {
    // "20111015:CLOSED;20111017:1800-1715"
    // "20111015:CLOSED;20111017:0930-1600"
    // "20111119:CLOSED;20111120:0015-2355,0015-2355"
        // 2018/08/23: now looks like:
        //      20180823:0400-20180823:2000;20180824:0400-20180824:2000;20180825:CLOSED;20180826:CLOSED;...
    const std::string a( what[1].first, what[1].second ); // year
    const std::string b( what[2].first, what[2].second ); // month
    const std::string c( what[3].first, what[3].second ); // day
    const std::string d( what[4].first, what[4].second ); // after :
    //std::string e( what[5].first, what[5].second );
    //std::string f( what[6].first, what[6].second );
    //std::string g( what[7].first, what[7].second );
    //std::string h( what[8].first, what[8].second );
    if ( "CLOSED" == d ) { // set close to be same as open (midnight)
      dtOpen  = boost::posix_time::from_iso_string( a + b + c + "T" + "00" + "00" + "00" );
      //dtClose = boost::posix_time::from_iso_string( a + b + c + "T" + "00" + "00" + "00" );
      dtClose = dtOpen;
    }
    else {
      if ( !boost::regex_match( d.c_str(), what, rxTime ) ) {
        throw std::runtime_error( "no time range found" );
      }
      else {
        const std::string i( what[ 1].first, what[ 1].second ); // hour
        const std::string j( what[ 2].first, what[ 2].second ); // minute
        const std::string k( what[ 3].first, what[ 3].second ); // year
        const std::string l( what[ 4].first, what[ 4].second ); // month
        const std::string m( what[ 5].first, what[ 5].second ); // day
        const std::string n( what[ 6].first, what[ 6].second ); // hour
        const std::string o( what[ 7].first, what[ 7].second ); // minute
        //std::string p( what[ 8].first, what[ 8].second );
        //std::string q( what[ 9].first, what[ 9].second );
        //std::string r( what[10].first, what[10].second );
        try {
          dtOpen  = boost::posix_time::from_iso_string( a + b + c + "T" + i + j + "00" );
          dtClose = boost::posix_time::from_iso_string( k + l + m + "T" + n + o + "00" );
          if ( dtOpen > dtClose ) { // is this still a valid operation with new IB duration format?
            dtOpen -= boost::gregorian::date_duration( 1 );
          }
        }
        catch (...) {
          BOOST_LOG_TRIVIAL(error) << "DecodeMarketHours error: " << i << "-" << j << "-" << k << "-" << l << "-" << m << "-" << n << "-" << o;
        }
      }
    }
  }
  // adjust to previous day (may not be required with new IB format
  /*
  if ( 1 == dtClose.date().day_of_week() ) {  // monday should be friday close
    dtClose -= date_duration( 3 );
    dtOpen -= date_duration( 3 );
  }
  else {
    dtClose -= date_duration( 1 );
    dtOpen -= date_duration( 1 );
  }
  */
}

void TWS::contractDetails( int reqId, const ContractDetails& contractDetails ) {

  // instrument is constructed, but is not registered with InstrumentManager

  assert( 0 < contractDetails.contract.conId );

  fOnContractDetail_t handler = nullptr;
  pInstrument_t pInstrument;

  {
    mapActiveRequests_t::iterator iterRequest;
    std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );  // locks map updates
    iterRequest = m_mapActiveRequests.find( reqId );  // entry removed with contractDetailsEnd
    if ( m_mapActiveRequests.end() == iterRequest ) {
      throw std::runtime_error( "contractDetails out of sync" );  // this means the requests are in sync, and so could use linked list instead
    }
    handler = std::move( iterRequest->second->fOnContractDetail );
    iterRequest->second->fOnContractDetail = nullptr;
    pInstrument = iterRequest->second->pInstrument;  // might be empty
    // NOTE: request is removed from m_mapActiveRequests in contractDetailsEnd
  }

  // need some logic here (some or all of which may now be implemented, just needs a cleanup):
  // * if instrument is supplied, only supplement some existing information
  // * if instrument not supplied, then go through whole building instrument exercise, or will BuildInstrument supply additional information
  // * also, need logic to determine if we have the symbol, or not
  // * build instrument doesn't build the symbol, is performed later in the logic here, so can skip the assignment.
  // * need a truth table for what to do with instrument (present or not), and symbol(present or not)

  //  instrument *  symbol => don't build new symbol, fill new instrument with settings
  // !instrument *  symbol => assign from existing symbol, fill in... when would this happen?
  //  instrument * !symbol => build symbol, fill in and return passed in instrument
  // !instrument * !symbol => build symbol, fill in and return new instrument

  // what happens if instrument/contract already built, then in another incarnation of application, what happens?  should be ok, as contract id is in instrument.

  mapContractToSymbol_t::iterator iterMap = m_mapContractToSymbol.find( contractDetails.contract.conId );

  //assert( !( (0 != pInstrument.get()) && (m_mapContractToSymbol.end() != iter) ) );  // illegal call:  instrument and contract already exist, and should have been mated

  if ( ( nullptr == pInstrument.get() ) && ( m_mapContractToSymbol.end() != iterMap ) ) {
    pInstrument = iterMap->second->GetInstrument();
  }

  BuildInstrumentFromContractDetails( contractDetails, pInstrument );  // creates new contract, or uses existing one

  pInstrument->SetMinTick( contractDetails.minTick );

//  std::cout
//    << contractDetails.contract.localSymbol
//    << "," << contractDetails.marketRuleIds
//    << "," << contractDetails.validExchanges
//    << std::endl;

  using vString_t = std::vector<std::string>;

  using iterString_t = std::string::const_iterator;
  iterString_t bgn, end;

  ParseStrings<iterString_t> grammarStrings;
  bool bOk;

  vString_t vExchange;
  if ( 0 < contractDetails.validExchanges.size() ) {
    bgn = contractDetails.validExchanges.begin();
    end = contractDetails.validExchanges.end();

    bOk = parse( bgn, end, grammarStrings, vExchange );
    assert( bOk );
  }

  vString_t vMarketRuleId;
  if ( 0 < contractDetails.marketRuleIds.size() ) {
    bgn = contractDetails.marketRuleIds.begin();
    end = contractDetails.marketRuleIds.end();

    bOk = parse( bgn, end, grammarStrings, vMarketRuleId );
    assert( bOk );
  }

  assert( vExchange.size() == vMarketRuleId.size() );

  std::string sExchangeRule;
  for ( vString_t::size_type ix = 0; ix < vExchange.size(); ix++ ) {
    if ( 0 < ix ) sExchangeRule += ",";
    sExchangeRule += vExchange[ ix ] + '=' + vMarketRuleId[ ix ];
  }

  pInstrument->SetExchangeRules( sExchangeRule );

  ptime dtOpen;
  ptime dtClose;
  using tzEST_t = boost::date_time::local_adjustor<ptime, -5, us_dst>;
  //using tzATL_t = boost::date_time::local_adjustor<ptime, -4, us_dst>;

  if ( ( "EST" != contractDetails.timeZoneId ) && ( "EST5EDT" != contractDetails.timeZoneId ) ) {
    //std::cout << contractDetails.longName << " differing timezones, EST vs " << contractDetails.timeZoneId << std::endl;
  }
  else {
    // use for time zone conversion to UTC in the next part if not EST
    // make another 'using' above for anything else found
  }

//    std::cout << "IB: " << contractDetails.tradingHours << ", " << contractDetails.liquidHours << std::endl;
/*
  if ( 0 == contractDetails.tradingHours.size() ) {
    //std::cout << "TWS::contractDetails tradingHours is zero length" << std::endl;
  }
  else {
    try {
      DecodeMarketHours( contractDetails.tradingHours, dtOpen, dtClose );
      pInstrument->SetTimeTrading(
        //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtOpen ) ),
        //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtClose ) )
        // store the values in utc
        tzEST_t::local_to_utc( dtOpen ),
        tzEST_t::local_to_utc( dtClose )
        );
    }
    catch ( std::runtime_error& e ) {
      std::cout
        << "IBTWS tradingHours runtime error: " << e.what()
        << " [" << contractDetails.tradingHours << "]"
        << std::endl;
    }
    catch ( std::exception& e ) {
      std::cout
        << "IBTWS tradingHours exception: "
        << e.what()
        << " [" << contractDetails.tradingHours << "]" << std::endl;
    }
  }
*/
//    std::cout << "TH: " << pInstrument->GetTimeTrading().begin() << ", " << pInstrument->GetTimeTrading().end() << std::endl;
/*
  if ( 0 == contractDetails.liquidHours.size() ) {
    //std::cout << "TWS::contractDetails liquidHours is zero length" << std::endl;
  }
  else {
    try {
      DecodeMarketHours( contractDetails.liquidHours, dtOpen, dtClose );
      pInstrument->SetTimeLiquid(
        //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtOpen ) ),
        //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtClose ) )
        // store the values in utc
        tzEST_t::local_to_utc( dtOpen ),
        tzEST_t::local_to_utc( dtClose )
        );
    }
    catch ( std::runtime_error& e ) {
      std::cout << "IBTWS liquidHours error: " << e.what() << std::endl;
    }
  }
*/
//    std::cout << "LH: " << pInstrument->GetTimeLiquid().begin() << ", " << pInstrument->GetTimeLiquid().end() << std::endl;

  if ( m_mapContractToSymbol.end() == iterMap ) {  // create new symbol from contract
    pSymbol_t pSymbol = NewCSymbol( pInstrument );
  }
  try {
    if ( nullptr != handler )
      handler( contractDetails, pInstrument );
  }
  catch ( std::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "IBTWS::contractDetails handler exception: " << e.what();
  }
  catch (...) {
    BOOST_LOG_TRIVIAL(error) << "IBTWS contractDetails unknown error when delegating";
  }

}

void TWS::contractDetailsEnd( const reqId_t reqId ) {
  // not called when symbol not available
  // rely on timed evictor for clean up

  //std::cout << "contractDetailsEnd request " << reqId << std::endl;

  bool bFound( false );
  fOnContractDetailDone_t fOnContractDetailDone = nullptr;
  std::chrono::time_point<std::chrono::system_clock> dtSubmitted;

  {
    std::scoped_lock<std::mutex> lock( m_mutexActiveRequests );

    mapActiveRequests_t::iterator iterActiveRequests = m_mapActiveRequests.find( reqId );
    if ( m_mapActiveRequests.end() ==  iterActiveRequests ) {
      BOOST_LOG_TRIVIAL(error)
        << "IB request " << reqId << " early eviction **"
        ;
    }
    else {

      Request& request( *iterActiveRequests->second );

      //std::cout
      //  << "contractDetailsEnd found "
      //  << reqId << ","
      //  << vt.first << ","
      //  << vt.second->pInstrument->GetInstrumentName()
      //  << std::endl;

      dtSubmitted = request.dtSubmitted;
      fOnContractDetailDone = std::move( request.fOnContractDetailDone );

      request.Clear();
      m_vRequestRecycling.push_back( iterActiveRequests->second );
      m_mapActiveRequests.erase( iterActiveRequests );

      bFound = true;
    }
  }

  if ( nullptr != fOnContractDetailDone ) {
    fOnContractDetailDone( bFound ); // TODO: ensure this is called only once
  }

  if ( bFound ) {

    UpdateActiveRequests();

    if ( false ) {
      std::chrono::time_point<std::chrono::system_clock> finished
        = std::chrono::system_clock::now();
      std::chrono::duration<double, std::milli> elapsed = finished - dtSubmitted;
      BOOST_LOG_TRIVIAL(error) << "IB request roundtrip for " << reqId << ": " << elapsed.count();  // 250 - 650 ms (2022/05/24)
    }
  }

}

void TWS::bondContractDetails( int reqId, const ContractDetails& contractDetails ) {
}

void TWS::nextValidId( OrderId orderId) {
  // todo: put in a flag to prevent orders until we've passed through this code
  m_ss.str("");
  ou::tf::Order::idOrder_t id = OrderManager::GlobalInstance().CheckOrderId( orderId );
  if ( orderId > id ) {
    //m_ss << "old order id (" << id << "), new order id (" << orderId << ")";
    BOOST_LOG_TRIVIAL(info) << "IB old order id (" << id << "), new order id (" << orderId << ")";
  }
  else {
    //m_ss << "next order id (" << id << "), IB had (" << orderId << ")";
    BOOST_LOG_TRIVIAL(info) << "next order id (" << id << "), IB had (" << orderId << ")";
  }
  //std::cout << m_ss.str() << std::endl;
}

// called from contractDetails, info comes from IB
void TWS::BuildInstrumentFromContractDetails( const ContractDetails& details, pInstrument_t& pInstrument ) {

  const Contract& contract( details.contract );

  const std::string& sSymbol( contract.symbol );  // need to look at this (was sUnderlying)
  const std::string& sLocalSymbol( contract.localSymbol );  // and this to name them properly
        std::string  sExchange( contract.exchange );

  OptionSide::EOptionSide os( OptionSide::Unknown );

  // calculate expiry, used with FuturesOption, Option   "GLD   120210C00159000"
  boost::gregorian::date dtExpiryRequested( boost::gregorian::not_a_date_time );
  boost::gregorian::date dtExpiryInSymbol( boost::gregorian::not_a_date_time );
  try {
    if ( 0 != contract.lastTradeDateOrContractMonth.length() ) {
      // save actual date in instrument, as last-day-to-trade and expiry-date  in symbol naming varies between Fri and Sat
      dtExpiryRequested = boost::gregorian::from_undelimited_string( contract.lastTradeDateOrContractMonth );
    }
  }
  catch ( std::exception e ) {
    //m_ss.str("");
    std::cout << "IB contract expiry is funny: " << e.what() << std::endl;
    //OutputDebugString( m_ss.str().c_str() );
  }

  InstrumentType::EInstrumentType it;
  bool bFound( false );
  // could use keyword lookup here
  for ( int ix = InstrumentType::Unknown; ix < InstrumentType::_Count; ++ix ) {
    if ( 0 == strcmp( szSecurityType[ ix ], contract.secType.c_str() ) ) {
      it = static_cast<InstrumentType::EInstrumentType>( ix );
      bFound = true;
      break;
    }
  }
  if ( !bFound )
    throw std::out_of_range( "can't find instrument type" );

  if ( "" == sExchange ) sExchange = "SMART";

  // test if arriving instrument type matches expected instrument type
  if ( nullptr != pInstrument.get() ) {
    if ( pInstrument->GetInstrumentType() != it )
      throw std::runtime_error( "IBTWS::BuildInstrumentFromContract: Instrument types don't match" );
    pInstrument->SetExchangeName( sExchange );
  }

  mapSymbols_t::iterator iterSymbol;

  switch ( it ) {
    case InstrumentType::Stock:
      if ( nullptr == pInstrument.get() ) {
	      pInstrument = Instrument::pInstrument_t( new Instrument( sSymbol, it, sExchange ) );
      }
      else {
	      if ( pInstrument->GetInstrumentName( Instrument::eidProvider_t::EProviderIB ) != sSymbol )
          throw std::runtime_error( "IBTWS::BuildInstrumentFromContract: Stock, underlying no match" );
      }
      break;
    case InstrumentType::FuturesOption:
    case InstrumentType::Option:
      if ( "P" == contract.right ) os = OptionSide::Put;
      if ( "C" == contract.right ) os = OptionSide::Call;

      /*
        std::cout
          << "IB details: "
          <<        contractDetails.realExpirationDate
          << "," << contractDetails.lastTradeTime
          << "," << contractDetails.timeZoneId
      //    << "," << "lqdhrs=" << contractDetails.liquidHours
      //    << "," << "trdhrs=" << contractDetails.tradingHours
          << std::endl;
          // 20220425,15:00,US/Central
      */
      {
        boost::posix_time::ptime dtExpiry( boost::posix_time::not_a_date_time );
        if ( ( 0 != details.realExpirationDate.size() )
          && ( 0 != details.lastTradeTime.size() )
          && ( 0 != details.timeZoneId.size() )
        ) {
          const std::string& sDate( details.realExpirationDate );
          assert( 8 == sDate.size() );
          const std::string& sTime( details.lastTradeTime );
          assert( 5 == sTime.size() );
          boost::gregorian::date date(
            boost::lexical_cast<int>( sDate.substr( 0, 4 )),
            boost::lexical_cast<int>( sDate.substr( 4, 2 ) ),
            boost::lexical_cast<int>( sDate.substr( 6, 2 ) )
          );
          boost::posix_time::time_duration time(
                  boost::lexical_cast<int>( sTime.substr( 0, 2 ) ),
                  boost::lexical_cast<int>( sTime.substr( 3, 2 ) ),
                  0
          );

          std::string tz = details.timeZoneId; // TODO: need the keyword parser to lookup/replace faster
          if ( "US/Eastern" == tz ) tz = "America/New_York"; // TODO need to confirm what OPRA options have
          if ( "US/Central" == tz ) tz = "America/Chicago";
          if ( "US/Mountain" == tz ) tz = "America/Edmonton";
          if ( "US/Pacific" == tz ) tz = "America/Vancouver";
          dtExpiry = ou::TimeSource::GlobalInstance().ConvertRegionalToUtc( date, time, tz );
        }

        if ( 0 == pInstrument.get() ) {
          if ( boost::posix_time::not_a_date_time == dtExpiry ) {
            pInstrument = Instrument::pInstrument_t( new Instrument(
              sLocalSymbol, it, sExchange, dtExpiryRequested.year(), dtExpiryRequested.month(), dtExpiryRequested.day(),
              os, contract.strike ) );
          }
          else {
            pInstrument = Instrument::pInstrument_t( new Instrument(
              sLocalSymbol, it, sExchange,
              dtExpiry,
              os, contract.strike ) );
          }
        }
        else {
          //if ( pInstrument->GetUnderlyingName() != sUnderlying ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract underlying doesn't match" );
          if ( pInstrument->GetOptionSide() != os ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract side doesn't match" );
          if ( pInstrument->GetExpiry() != dtExpiryRequested ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract expiry doesn't match" );  // may also need to do an off by one error, futures may not match with out day
          if ( pInstrument->GetStrike() != contract.strike ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract strike doesn't match" );  //may have rounding issues
          if ( boost::posix_time::not_a_date_time != dtExpiry ) {
            pInstrument->SetExpiry( dtExpiry );
          }
        }
      }

//      try {
//        futures option looks like '"OG2V1 C1590"'
//        dtExpiryInSymbol = boost::gregorian::date( boost::gregorian::date(
//          boost::lexical_cast<int>( contract.localSymbol.substr(  6, 2 ) ) + 2000,
//          boost::lexical_cast<int>( contract.localSymbol.substr(  8, 2 ) ),
//          boost::lexical_cast<int>( contract.localSymbol.substr( 10, 2 ) )
//          ) );
//      }
//      catch ( std::exception e ) {
//        std::cout << "IB option contract expiry is funny: " << e.what() << " -- " << contract.localSymbol << std::endl;
//      }
//      SetCommonCalcExpiry probaby no longer required
//      pInstrument->SetCommonCalcExpiry( dtExpiryInSymbol );
      break;
    case InstrumentType::Future:
      if ( nullptr == pInstrument.get() ) {
	      pInstrument = Instrument::pInstrument_t( new Instrument( sSymbol, it, sExchange, dtExpiryRequested.year(), dtExpiryRequested.month() ) );
      }
      else {
      }
      break;
    case InstrumentType::Currency: {

      bFound = false;
      Currency::ECurrency base = Currency::_Count;
      for ( int ix = 0; ix < Currency::_Count; ++ix ) {
        if ( 0 == strcmp( Currency::Name[ ix ], contract.currency.c_str() ) ) {
          bFound = true;
          base = static_cast<Currency::ECurrency>( ix );
          break;
        }
      }
      if ( !bFound )
        throw std::out_of_range( "base currency lookup not found" );

      bFound = false;
      Currency::ECurrency counter = Currency::_Count;
      for ( int ix = 0; ix < Currency::_Count; ++ix ) {
        if ( 0 == strcmp( Currency::Name[ ix ], contract.symbol.c_str() ) ) {
          bFound = true;
          counter = static_cast<Currency::ECurrency>( ix );
          break;
        }
      }
      if ( !bFound )
        throw std::out_of_range( "counter currency lookup not found" );

      if ( nullptr == pInstrument.get() ) {
        pInstrument = std::make_shared<ou::tf::Instrument>( contract.localSymbol, ou::tf::InstrumentType::Currency,
          contract.exchange, base, counter
          );
      }
      }
      break;
    case InstrumentType::Index:
      assert( 0 == pInstrument.get() );  // maybe incorrect at some time in the future
      pInstrument = Instrument::pInstrument_t( new Instrument( sLocalSymbol, it, sExchange ) );
      break;
  }

  if ( nullptr == pInstrument )
    throw std::out_of_range( "unknown instrument type" );

  pInstrument->SetContract( contract.conId );
  //pInstrument->SetCurrency( );  // need to figure this out, use the currency calcs above
  if ( 0 < contract.multiplier.length() ) {
    pInstrument->SetMultiplier( boost::lexical_cast<unsigned long>( contract.multiplier ) );
  }
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIB, sLocalSymbol );

  //return pInstrument;  //legacy

}

void TWS::updatePortfolio( const Contract& contract, Decimal position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const std::string& accountName) {

//  pInstrument_t pInstrument;
//  IBSymbol* pSymbol;
//  mapGreeks_t::iterator iterGreeks;

  mapContractToSymbol_t::iterator iterId = m_mapContractToSymbol.find( contract.conId );
  if ( m_mapContractToSymbol.end() == iterId ) {
    // if we can't find an instrument, then create a new one.
//    pInstrument = BuildInstrumentFromContract( contract );
//    pSymbol = NewCSymbol( pInstrument );

    // preset option stuff.
//    structDeltaStuff stuff;
//    stuff.delta = 0;
//    stuff.impliedVolatility = 0;
//    stuff.modelPrice = 0;
//    stuff.position = position;
//    stuff.positionCalc = 0;
//    stuff.positionDelta = 0;
//    stuff.marketPrice = marketPrice;
//    stuff.averageCost = averageCost;

    //  change: look through mapDelta and insert if not found
//    m_mapGreeks.insert( pair_mapGreeks_t( pSymbol->GetTickerId(), stuff ) );

//    iterGreeks = m_mapGreeks.find( pSymbol->GetTickerId() );  // load iter for status print at end of this method

    // start market data
//    if ( 0 != position ) {
//      Contract contractMktData;
//      contractMktData = contract;
//      contractMktData.exchange = pInstrument->GetExchangeName();
//      pTWS->reqMktData( pSymbol->GetTickerId(), contractMktData, "100,101,104,106,221,225", false );
//      iterGreeks->second.bDataRequested = true;
//    }

  }
  else {
//    pSymbol = GetSymbol( iterId->first );
//    pInstrument = pSymbol->GetInstrument();

//    iterGreeks = m_mapGreeks.find( pSymbol->GetTickerId() );  // load iter for status print at end of this method

//    iterGreeks->second.position = position;
//    iterGreeks->second.marketPrice = marketPrice;
//    iterGreeks->second.averageCost = averageCost;

    // start/stop data depending upon position size
    if ( 0 == position ) {
//      if ( iterGreeks->second.bDataRequested ) {
//        pTWS->cancelMktData( pSymbol->GetTickerId() );
//        iterGreeks->second.bDataRequested = false;
//      }
    }
    else { // we have a position, so need data
/*      if ( !iterGreeks->second.bDataRequested ) {
        Contract contractMktData;
        contractMktData = contract;
        contractMktData.exchange = pInstrument->GetExchangeName();
        pTWS->reqMktData( pSymbol->GetTickerId(), contractMktData, "100,101,104,106,221,225", false );
        iterGreeks->second.bDataRequested = true;
      } */
    }
  }

  if ( false ) {
    m_ss.str("");
    m_ss << "portfolio item "
      << contract.symbol
      << " " << contract.localSymbol
      << "  id=" << contract.conId  // long
//      << ", type=" << InstrumentType::Name[ pInstrument->GetInstrumentType() ]
      << ", strike=" << contract.strike // double
//      << ", expire=" << iter->second.dtExpiry
//      << ", right=" << OptionSide::Name[ iter->second.os ]
      << ", pos=" << decimalStringToDisplay( position ) // int
      << ", price=" << marketPrice // double
      << ", val=" << marketValue // double
      << ", cost=" << averageCost // double
      << ", uPL=" << unrealizedPNL // double
      << ", rPL=" << realizedPNL // double
      //<< ", " << accountName
      << std::endl;
    std::cout << m_ss.str();
//    OutputDebugString( m_ss.str().c_str() );
  }
  if ( false ) {
    std::cout
      <<        contract.symbol
      << "," << contract.localSymbol
//      << "," << contract.strike // double
      << "," << contract.lastTradeDateOrContractMonth
      << "," << contract.primaryExchange
//      << "," << contract.conId  // long - available but don't display for now
//      << "," << contract.secId  // empty
      << "," << contract.multiplier
//      << ", type=" << InstrumentType::Name[ pInstrument->GetInstrumentType() ]
      << "," << decimalStringToDisplay( position ) // int
      << "," << marketPrice // double
      << "," << marketValue // double
      << "," << averageCost // double
      << "," << unrealizedPNL // double
      << "," << realizedPNL // double
      << "," << contract.currency
      //<< ", " << accountName
      << std::endl;
  }

  PositionDetail pd( contract.symbol, contract.localSymbol,
    contract.conId, contract.strike, contract.lastTradeDateOrContractMonth, contract.multiplier,
    position, marketPrice, marketValue, averageCost,
    unrealizedPNL, realizedPNL, contract.primaryExchange, contract.currency
    );
  if ( 0 != OnPositionDetailHandler ) OnPositionDetailHandler( pd );

}

// todo: use the keyword lookup to make this faster
//   key, bool, double, string
void TWS::updateAccountValue(const std::string& key, const std::string& val,
                                const std::string& currency, const std::string& accountName) {
  bool bEmit = false;
  if ( "AccountCode" == key ) bEmit = true;
  if ( "AccountReady" == key ) bEmit = true;
  if ( "AccountType" == key ) bEmit = true;
  if ( "AvailableFunds" == key ) bEmit = true;
  if ( "BuyingPower" == key ) {
    m_dblBuyingPower = atof( val.c_str() );
    bEmit = true;
    //std::cout << "**Buying Power " << m_dblBuyingPower << std::endl;
  }
  if ( "CashBalance" == key ) bEmit = true;
  if ( "Cushion" == key ) bEmit = true;
  if ( "GrossPositionValue" == key ) bEmit = true;
  if ( "PNL" == key ) bEmit = true;
  if ( "UnrealizedPnL" == key ) bEmit = true;
  if ( "SMA" == key ) bEmit = true;
  if ( "AvailableFunds" == key ) {
    m_dblAvailableFunds = atof( val.c_str() );
    bEmit = true;
  }
  if ( "MaintMarginReq" == key ) bEmit = true;
  if ( "InitMarginReq" == key ) bEmit = true;
  if ( bEmit ) {
  //if ( false ) {
    BOOST_LOG_TRIVIAL(info) << "IB account: " << key << ", " << val << ", " << currency << ", " << accountName;
  }
  AccountValue av( key, val, currency, accountName );
  if ( 0 != OnAccountValueHandler ) OnAccountValueHandler( av );
}

void TWS::marketRule( int marketRuleId, const vPriceIncrement_t& priceIncrements ) {

  for ( const vPriceIncrement_t::value_type& vt: priceIncrements ) {
    BOOST_LOG_TRIVIAL(info)
      << "IB::marketRule " << marketRuleId
      << " " << vt.lowEdge << "=" << vt.increment;
  }

  mapMarketRule_t::iterator iter = m_mapMarketRule.find( marketRuleId );
  if ( m_mapMarketRule.end() == iter ) {
    m_mapMarketRule.emplace( std::make_pair( marketRuleId, priceIncrements ) );
  }
  else {
    iter->second = priceIncrements; // over-write a default
  }

}

void TWS::Sync( pInstrument_t pInstrument ) {
  int rule = pInstrument->GetExchangeRule();
  MarketRuleExists( rule );
}

bool TWS::MarketRuleExists( const int rule ) {

  bool bExists( true );

  mapMarketRule_t::const_iterator iter = m_mapMarketRule.find( rule );
  if ( m_mapMarketRule.end() == iter ) {
    bExists = false;
    PriceIncrement pi;
    pi.lowEdge = 0.0;
    pi.increment = 0.01;
    vPriceIncrement_t vPriceIncrement;
    vPriceIncrement.push_back( pi );
    m_mapMarketRule.emplace( std::make_pair( rule, std::move( vPriceIncrement ) ) );
    m_pTWS->reqMarketRule( rule );
  }

  return bExists;
}

double TWS::GetInterval( const double price, const int rule ) {
  double interval( 0.01 );
  mapMarketRule_t::const_iterator iter = m_mapMarketRule.find( rule );
  if ( m_mapMarketRule.end() == iter ) {
    BOOST_LOG_TRIVIAL(warning)
      << "IB Price interval not found: " << rule
      << ", default to " << interval
      << " for price " << price
      ;
  }
  else {
    const vPriceIncrement_t& vIntervals( iter->second );
      // might use the search algorithm on this
      for ( const vPriceIncrement_t::value_type& level: vIntervals ) {
        if ( price >= ( 0.99 * level.lowEdge ) ) { // provides a rounding range
          interval = level.increment;
        }
        else {
          break;
        }
    }
  }
  assert( 0.0 < interval );
  return interval;
}

void TWS::updateMktDepth(TickerId id, int position, int operation, int side,
                            double price, Decimal size) {}
void TWS::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
                              int side, double price, Decimal size, bool isSmartDepth ) {}

void TWS::managedAccounts( const std::string& accountsList) {}
void TWS::receiveFA( faDataType pFaDataType, const std::string& cxml ) {}

void TWS::scannerParameters( const std::string &xml ) {}
void TWS::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
                         const std::string& distance, const std::string& benchmark, const std::string& projection,
                         const std::string& legsStr) {}
void TWS::scannerDataEnd(int reqId) {}

void TWS::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
                         Decimal volume, Decimal wap, int count) {}

void TWS::openOrderEnd() {}
void TWS::verifyMessageAPI( const std::string& apiData) {}
void TWS::verifyCompleted( bool isSuccessful, const std::string& errorText) {}
void TWS::displayGroupList( int reqId, const std::string& groups) {}
void TWS::displayGroupUpdated( int reqId, const std::string& contractInfo) {}
void TWS::verifyAndAuthMessageAPI( const std::string& apiData, const std::string& xyzChallange) {}
void TWS::verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText) {}
void TWS::connectAck() {}
void TWS::positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, Decimal pos, double avgCost) {}
void TWS::positionMultiEnd( int reqId) {}
void TWS::accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency) {}
void TWS::accountUpdateMultiEnd( int reqId) {}
void TWS::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass,
	const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes) {}
void TWS::securityDefinitionOptionalParameterEnd(int reqId) {}
void TWS::softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers) {}
void TWS::familyCodes(const std::vector<FamilyCode> &familyCodes) {}
void TWS::symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions) {}
void TWS::mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions) {}
void TWS::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, const std::string& headline, const std::string& extraData) {}
void TWS::smartComponents(int reqId, const SmartComponentsMap& theMap) {}
void TWS::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions) {}
void TWS::newsProviders(const std::vector<NewsProvider> &newsProviders) {}
void TWS::newsArticle(int requestId, int articleType, const std::string& articleText) {}
void TWS::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline) {}
void TWS::historicalNewsEnd(int requestId, bool hasMore) {}
void TWS::headTimestamp(int reqId, const std::string& headTimestamp) {}
void TWS::histogramData(int reqId, const HistogramDataVector& data) {}
void TWS::historicalDataUpdate(TickerId reqId, const Bar& bar) {}

void TWS::rerouteMktDataReq(int reqId, int conid, const std::string& exchange) {}
void TWS::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange) {}
void TWS::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL) {}
void TWS::pnlSingle(int reqId, Decimal pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value) {}
void TWS::historicalTicks(int reqId, const std::vector<HistoricalTick> &ticks, bool done) {}
void TWS::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk> &ticks, bool done) {}
void TWS::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast> &ticks, bool done) {}
void TWS::tickByTickAllLast(int reqId, int tickType, time_t time, double price, Decimal size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions) {}
void TWS::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, Decimal bidSize, Decimal askSize, const TickAttribBidAsk& tickAttribBidAsk) {}
void TWS::tickByTickMidPoint(int reqId, time_t time, double midPoint) {}
void TWS::orderBound(long long orderId, int apiClientId, int apiOrderId) {}
void TWS::completedOrder(const Contract& contract, const Order& order, const OrderState& orderState) {}
void TWS::completedOrdersEnd() {}
void TWS::replaceFAEnd(int reqId, const std::string& text) {}
void TWS::wshMetaData(int reqId, const std::string& dataJson) {}
void TWS::wshEventData(int reqId, const std::string& dataJson) {}

void TWS::accountDownloadEnd(const std::string& accountName) {}

void TWS::historicalData(TickerId reqId, const Bar& bar) {}
void TWS::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {}

void TWS::tickSnapshotEnd( int reqId) {}

void TWS::fundamentalData(TickerId reqId, const std::string& data) {}
void TWS::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract) {}

// TODO: there are more of these now, strings are used for diagnostics
// From client/EWrapper.h
const char *TWS::TickTypeStrings[] = {
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

} // namespace ib
} // namespace tf
} // namespace ou
