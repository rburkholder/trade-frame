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

#ifdef _WIN32
#include "StdAfx.h"
#else
#include "linux/StdAfx.h"
#endif

#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

#include <boost/regex.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>

#include <OUCommon/KeyWordMatch.h>
#include <OUCommon/Debug.h>

#include <TFTrading/KeyTypes.h>
#include <TFTrading/OrderManager.h>

#include "Shared/CommissionReport.h"

#include "IBTWS.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct DecodeStatusWord {
  enum enumStatus{ Unknown, PreSubmitted, PendingSubmit, PendingCancel, Submitted, Cancelled, Filled, Inactive };
  DecodeStatusWord( void ): kwm( Unknown, 50 ) {
    kwm.AddPattern( "Cancelled", Cancelled );
    kwm.AddPattern( "Filled", Filled );
    kwm.AddPattern( "Inactive", Inactive );
    kwm.AddPattern( "PreSubmitted", PreSubmitted );
    kwm.AddPattern( "Submitted", Submitted );
    kwm.AddPattern( "PendingSubmit", PendingSubmit );
    kwm.AddPattern( "PendingCancel", PendingCancel );
  }
  enumStatus Match( const std::string& status ) { return kwm.FindMatch( status ); };
private:
  KeyWordMatch<enumStatus> kwm;;
};

DecodeStatusWord dsw;


IBTWS::IBTWS( const std::string &acctCode, const std::string &address, unsigned int port ): 
  ProviderInterface<IBTWS,IBSymbol>(), 
  EWrapper(),
  pTWS( NULL ),
  m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 ),
//  m_dblPortfolioDelta( 0 ),
  m_idClient( 0 ),
  m_nxtReqId( 0 )
{
  m_sName = "IB";
  m_nID = keytypes::EProviderIB;
  pSymbol_t p;
  m_vTickerToSymbol.push_back( p );  // first ticker is 1, so preload with nothing at position 0
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
  m_bProvidesGreeks = true;
  m_pProvidesBrokerInterface = true;
}

IBTWS::~IBTWS(void) {
  Disconnect();
  m_vTickerToSymbol.clear();
  m_mapContractToSymbol.clear();
}

void IBTWS::ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth ) {
  //std::string month( boost::lexical_cast<std::string,boost::uint16_t>( nMonth ) );
  contract.expiry 
    = boost::lexical_cast<std::string,boost::uint16_t>( nYear ) 
    //+ ( ( 1 == month.size() ) ? "0" : "" ) 
    //+ month
    += ( ( 9 < nMonth ) ? "" : "0" ) + boost::lexical_cast<std::string>( nMonth );
    ; 
}

void IBTWS::ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {
  ContractExpiryField( contract, nYear, nMonth );
  contract.expiry 
    //+= boost::lexical_cast<std::string,boost::uint16_t>( nDay ) 
    += ( ( 9 < nDay ) ? "" : "0" ) + boost::lexical_cast<std::string>( nDay );
    ; 
}

void IBTWS::Connect() {
  if ( NULL == pTWS ) {
    OnConnecting( 0 );
    pTWS = new EPosixClientSocket( this );
    bool bReturn = pTWS->eConnect( m_sIPAddress.c_str(), m_nPort, m_idClient );
    if ( bReturn ) {
      m_bConnected = true;
      m_thrdIBMessages = boost::thread( boost::bind( &IBTWS::ProcessMessages, this ) );
      pTWS->reqCurrentTime();
      pTWS->reqNewsBulletins( true );
      pTWS->reqOpenOrders();
      //ExecutionFilter filter;
      //pTWS->reqExecutions( filter );
      pTWS->reqAccountUpdates( true, "" );
      OnConnected( 0 );
    }
    else {
      // need to integrate common stuff from Disconnect, most everything but the thread bit
      DisconnectCommon( false );
    }
  }
}

void IBTWS::Disconnect() {
  // check to see if there are any watches happening, and get them disconnected
  if ( NULL != pTWS ) {
    DisconnectCommon( true );
  }
}

void IBTWS::DisconnectCommon( bool bSignalEnd ){
    OnDisconnecting( 0 );
    m_bConnected = false;
    if ( bSignalEnd ) {
      pTWS->eDisconnect();
      m_thrdIBMessages.join();  // wait for message processing to exit
    }
    delete pTWS;
    pTWS = NULL;
    OnDisconnected( 0 );
    m_ss.str("");
    m_ss << "IB Disconnected " << std::endl;
//    OutputDebugString( m_ss.str().c_str() );
}

// this is executed in non-main thread, and the events below will be called from the processing here
// so... be aware of cross thread issues
void IBTWS::ProcessMessages( void ) {
  bool bOK = true;
  while ( bOK && m_bConnected ) {
  // maybe only activate while messages are queued up
  //   but will lose something when receiving market data
  //while ( m_bConnected ) {
    bOK = pTWS->checkMessages();  // code in EClientSocketBaseImpl.h has code change on linux for select()
  }
  m_bConnected = false;  // placeholder for debug

  // need to deal with pre=mature exit so that flags get reset
  // maybe a state machine would keep track
}

// ** associate the instrument with the request structure.  buildinstrumentfrom contract then can fill/check/validate as needed

void IBTWS::RequestContractDetails( 
                                   const std::string& sSymbolBaseName, pInstrument_t pInstrument, 
                                   OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone 
) {
  assert( 0 == pInstrument->GetContract() );  // handle this better, ie, return gently, or create exception
  Contract contract;
  contract.symbol = sSymbolBaseName;  // separately, as it may differ from IQFeed or others
  contract.currency = pInstrument->GetCurrencyName(); // check if these match
  contract.exchange = "SMART";
  std::cout << "Exchange supplied to IBTWS: " << pInstrument->GetExchangeName() << std::endl;
  contract.secType = szSecurityType[ pInstrument->GetInstrumentType() ];
  switch ( pInstrument->GetInstrumentType() ) {
  case InstrumentType::Stock:
    // nothing to do?
    break;
  case InstrumentType::Option:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    contract.strike = pInstrument->GetStrike();
    contract.right = pInstrument->GetOptionSide();
    //contract.multiplier = boost::lexical_cast<std::string>( pInstrument->GetMultiplier() );  // needed for small options
    break;
  case InstrumentType::Future:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    if ( "COMEX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC options
    if ( "CME" == pInstrument->GetExchangeName() ) contract.exchange = "GLOBEX";   // ES options
    break;
  case InstrumentType::FuturesOption:
    ContractExpiryField( contract, pInstrument->GetExpiryYear(), pInstrument->GetExpiryMonth(), pInstrument->GetExpiryDay() );
    contract.strike = pInstrument->GetStrike();
    contract.right = pInstrument->GetOptionSide();
    if ( "COMEX" == pInstrument->GetExchangeName() ) contract.exchange = "NYMEX";  // GC
    if ( "CME" == pInstrument->GetExchangeName() ) contract.exchange = "GLOBEX";   // ES?
    break;
  }
  RequestContractDetails( contract, fProcess, fDone, pInstrument );
}

void IBTWS::RequestContractDetails( const Contract& contract, OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone ) {
  // results supplied at contractDetails()
  pInstrument_t pInstrument;  // just allocate, and pass as empty
  RequestContractDetails( contract, fProcess, fDone, pInstrument );
}

void IBTWS::RequestContractDetails( 
  const Contract& contract, OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone, pInstrument_t pInstrument ) {
  // 2014/01/28 not complete yet, BuildInstrumentFromContract not converted over
  // pInstrument can be empty, or can have an instrument
  // results supplied at contractDetails()
  structRequest_t* pRequest;
  pRequest = 0;
  // needs to be thread protected:
  boost::mutex::scoped_lock lock(m_mutexContractRequest);
  if ( 0 == m_vInActiveRequestId.size() ) {
    pRequest = new structRequest_t( m_nxtReqId++, fProcess, fDone, pInstrument );
  }
  else {
    pRequest = m_vInActiveRequestId.back();
    m_vInActiveRequestId.pop_back();
    pRequest->id = m_nxtReqId++;
    pRequest->fProcess = fProcess;
    pRequest->fDone = fDone;
    pRequest->pInstrument = pInstrument;
  }
  m_mapActiveRequestId[ pRequest->id ] = pRequest;
  pTWS->reqContractDetails( pRequest->id, contract );
}

//IBSymbol *IBTWS::NewCSymbol( const std::string &sSymbolName ) {
IBTWS::pSymbol_t IBTWS::NewCSymbol( IBSymbol::pInstrument_t pInstrument ) {
  // todo:  check that contract doesn't already exist
  TickerId ticker = ++m_curTickerId;
//  pSymbol_t pSymbol( new IBSymbol( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIB ), pInstrument, ticker ) );  // is there someplace with the IB specific symbol name, or is it set already?  (this simply creates the object, no additional function here)
  pSymbol_t pSymbol( new IBSymbol( pInstrument->GetInstrumentName( ID() ), pInstrument, ticker ) );  // is there someplace with the IB specific symbol name, or is it set already?  (this simply creates the object, no additional function here)
  // todo:  do an existance check on the instrument/symbol
  ProviderInterface<IBTWS,IBSymbol>::AddCSymbol( pSymbol );
  m_vTickerToSymbol.push_back( pSymbol );
  m_mapContractToSymbol.insert( pair_mapContractToSymbol_t( pInstrument->GetContract(), pSymbol ) );
  return pSymbol;
}

void IBTWS::StartQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void IBTWS::StopQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void IBTWS::StartTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void IBTWS::StopTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void IBTWS::StartGreekWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void IBTWS::StopGreekWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void IBTWS::StartQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( !pIBSymbol->GetQuoteTradeWatchInProgress() ) {
    // start watch
    Contract contract;
    contract.conId = pIBSymbol->GetInstrument()->GetContract();  // mostly enough to have contract id
    contract.exchange = pIBSymbol->GetInstrument()->GetExchangeName();
    contract.currency = pIBSymbol->GetInstrument()->GetCurrencyName();
    pIBSymbol->SetQuoteTradeWatchInProgress();
    //pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "100,101,104,165,221,225,236", false );
    TagValueListSPtr pMktDataOptions;
    pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "", false, pMktDataOptions );
  }
}

void IBTWS::StopQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( pIBSymbol->QuoteWatchNeeded() || pIBSymbol->TradeWatchNeeded() || pIBSymbol->GreekWatchNeeded() ) {
    // don't do anything if either a quote or trade or greek watch still in progress
  }
  else {
    // stop watch
    pTWS->cancelMktData( pIBSymbol->GetTickerId() );
    pIBSymbol->ResetQuoteTradeWatchInProgress();
  }
}

void IBTWS::StartDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
  if ( !pIBSymbol->GetDepthWatchInProgress() ) {
    // start watch
    pIBSymbol->SetDepthWatchInProgress();
  }
}

void IBTWS::StopDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
  if ( pIBSymbol->DepthWatchNeeded() ) {
  }
  else {
    // stop watch
    pIBSymbol->ResetDepthWatchInProgress();
  }
}

// indexed with InstrumentType::enumInstrumentTypes
const char *IBTWS::szSecurityType[] = { 
  "NULL", "STK", "OPT", "FUT", "FOP", "CASH", "IND" };  // InsrumentType::enumInstrumentType
const char *IBTWS::szOrderType[] = { 
  "UNKN", "MKT", "LMT", "STP", "STPLMT", "NULL",     // OrderType::enumOrderType
  "TRAIL", "TRAILLIMIT", "MKTCLS", "LMTCLS", "SCALE" };

void IBTWS::PlaceOrder( pOrder_t pOrder ) {

  ::Order twsorder; 
  twsorder.orderId = pOrder->GetOrderId();

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
//      contract.expiry = s;
      //if ( "CBOT" == contract.exchange ) contract.exchange = "ECBOT";
      if ( 0 != pOrder->GetInstrument()->GetContract() ) {
      }
      else {
        assert( false );  // need to fix this formatter, use the boost::gregorian date stuff
      }
      break;
  }
  twsorder.action = pOrder->GetOrderSideName();
  twsorder.totalQuantity = pOrder->GetQuantity();
  twsorder.orderType = szOrderType[ pOrder->GetOrderType() ];
  twsorder.tif = "DAY";
  //twsorder.goodAfterTime = "20080625 09:30:00";
  //twsorder.goodTillDate = "20080625 16:00:00";
  switch ( pOrder->GetOrderType() ) {
    case OrderType::Limit:
      twsorder.lmtPrice = pOrder->GetPrice1();
      twsorder.auxPrice = 0;
      break;
    case OrderType::Stop:
      twsorder.auxPrice = pOrder->GetPrice1();
      twsorder.auxPrice = 0;
      break;
    case OrderType::StopLimit:
      twsorder.lmtPrice = pOrder->GetPrice1();
      twsorder.auxPrice = pOrder->GetPrice2();
      break;
    default:
      twsorder.lmtPrice = 0;
      twsorder.auxPrice = 0;
  }
  twsorder.transmit = true;
  twsorder.outsideRth = pOrder->GetOutsideRTH();
  //twsorder.whatIf = true;

  ProviderInterface<IBTWS,IBSymbol>::PlaceOrder( pOrder ); // any underlying initialization
  pTWS->placeOrder( twsorder.orderId, contract, twsorder );
}

void IBTWS::CancelOrder( pOrder_t pOrder ) {
  ProviderInterface<IBTWS,IBSymbol>::CancelOrder( pOrder );
  pTWS->cancelOrder( pOrder->GetOrderId() );
}

void IBTWS::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    IBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickPrice " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << price << std::endl;
    pSym->AcceptTickPrice( tickType, price );
  }
}

void IBTWS::tickSize( TickerId tickerId, TickType tickType, int size) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    IBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickSize " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << size << std::endl;
    pSym->AcceptTickSize( tickType, size );
  }
}

void IBTWS::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice ) {

  IBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
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

void IBTWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
//  std::cout << "tickGeneric " << m_vTickerToSymbol[ tickerId ]->Name() << ", " << TickTypeStrings[tickType] << ", " << value << std::endl;
}

void IBTWS::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    IBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickString " << pSym->Name() << ", " 
    //  << TickTypeStrings[tickType] << ", " << value;
    //std::cout << std::endl;
    pSym->AcceptTickString( tickType, value );
  }
}

void IBTWS::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
  double totalDividends, int holdDays, const std::string& futureExpiry, double dividendImpact, double dividendsToExpiry ) {
  m_ss.str("");
  m_ss << "tickEFP" << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void IBTWS::openOrder( OrderId orderId, const Contract& contract, const ::Order& order, const OrderState& state) {
  if ( order.whatIf ) {
    m_ss.str("");
    m_ss
      << "WhatIf:  ordid=" << orderId << ", cont.sym=" << contract.symbol
      << ", state.commission=" << state.commission
      << " " << state.commissionCurrency
      << ", state.equitywithloan=" << state.equityWithLoan 
      << ", state.initmarg=" << state.initMargin
      << ", state.maintmarg=" << state.maintMargin
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
      OrderManager::Instance().ReportCommission( orderId, state.commission ); 
    // use spirit to do this to make it faster with a trie, or use keyword match
    DecodeStatusWord::enumStatus status = dsw.Match( state.status );
    switch ( status ) {
    case DecodeStatusWord::Submitted:
      break;
    case DecodeStatusWord::Filled:
      break;
    case DecodeStatusWord::Cancelled:
      OrderManager::Instance().ReportCancellation( order.orderId );
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

void IBTWS::orderStatus( OrderId orderId, const std::string& status, int filled,
                         int remaining, double avgFillPrice, int permId, int parentId,
                         double lastFillPrice, int clientId, const std::string& whyHeld) 
{
  if ( true ) {
    m_ss.str("");
    m_ss
      << "OrderStatus: ordid=" << orderId 
      << ", stat=" << status 
      << ", fild=" << filled 
      << ", rem=" << remaining 
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
  DecodeStatusWord::enumStatus status_ = dsw.Match( status );
  switch ( status_ ) {
    case DecodeStatusWord::Cancelled:
      OrderManager::Instance().ReportCancellation( orderId );
      break;
    case DecodeStatusWord::Submitted:
    case DecodeStatusWord::Filled:
      break;
    default:
      std::cout << "IBTWS::orderStatus: " << orderId << "," << status << std::endl;
  }
}

void IBTWS::execDetails( int reqId, const Contract& contract, const ::Execution& execution ) {
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

  OrderSide::enumOrderSide side = OrderSide::Unknown;
  if ( "BOT" == execution.side ) side = OrderSide::Buy;  // could try just first character for fast comparison
  if ( "SLD" == execution.side ) side = OrderSide::Sell;
  if ( OrderSide::Unknown == side ) {
    m_ss.str("");
    m_ss << "Unknown execution side: " << execution.side << std::endl;
//    OutputDebugString( m_ss.str().c_str() );
  }
  else {
    Execution exec( execution.price, execution.shares, side, execution.exchange, execution.execId );
    OrderManager::Instance().ReportExecution( execution.orderId, exec );
  }
}

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
IBTWS::pSymbol_t IBTWS::GetSymbol( long ContractId ) {
  mapContractToSymbol_t::iterator iterId = m_mapContractToSymbol.find( ContractId );
  if ( m_mapContractToSymbol.end() == iterId ) {
    throw std::out_of_range( "can't find contract" );
  }
  return iterId->second;
}

// check for symbol existance, and return, else add and return
IBTWS::pSymbol_t IBTWS::GetSymbol( pInstrument_t instrument ) {

  long contractId;
  contractId = instrument->GetContract();
  //assert( 0 != contractId );
  if ( 0 == contractId ) {
    throw std::runtime_error( "IBTWS::GetSymbol: contract id not supplied" );
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

void IBTWS::error(const int id, const int errorCode, const std::string errorString) {
  switch ( errorCode ) {
    case 1102: // Connectivity has been restored
      pTWS->reqAccountUpdates( true, "" );
      break;
    case 2104:  // datafarm connected ok
      break;
    case 200:  // no security definition has been found
      if ( 0 != OnSecurityDefinitionNotFound ) OnSecurityDefinitionNotFound();
      break;
    default:
//      m_ss.str("");
//      m_ss << "error " << id << ", " << errorCode << ", " << errorString << std::endl;
//      OutputDebugString( m_ss.str().c_str() );
      std::cout << "error " << id << ", " << errorCode << ", " << errorString << std::endl;
      break;
  }
}

void IBTWS::winError( const std::string& str, int lastError) {
  //m_ss.str("");
  //m_ss << "winerror " << str << ", " << lastError << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
  std::cout << "winerror " << str << ", " << lastError << std::endl;
}

void IBTWS::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {
  //m_ss.str("");
  std::cout << "news bulletin " << msgId << "-" << msgType << "-" << originExch << ": " << newsMessage  << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
}

void IBTWS::currentTime(long time) {
  m_ss.str("");
  m_ss << "current time " << time << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
  m_time = time;
}

void IBTWS::updateAccountTime (const std::string& timeStamp) {
}

void IBTWS::position( const std::string& s, const Contract & c, int, double d) {
}

void IBTWS::positionEnd(void) {
}

void IBTWS::accountSummary( int i, const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4 ) {
}

void IBTWS::accountSummaryEnd( int i ) {
}

void IBTWS::verifyMessageAPI( const std::string& s ) {
}

void IBTWS::verifyCompleted( bool b, const std::string& s ) {
}

void IBTWS::marketDataType( TickerId id , int i ) {
}

void IBTWS::commissionReport( const CommissionReport& cr ) { 
  std::cout << "commissionReport " << cr.execId << ", " << cr.commission << ", " << cr.currency << ", " << cr.realizedPNL << std::endl;
}

// convert to boost::spirit?
void IBTWS::DecodeMarketHours( const std::string& mh, ptime& dtOpen, ptime& dtClose ) {
  static const boost::regex rxFields( "([^:]+):([^;]+);([^:]+):(.+)" );
  //static const boost::regex rxFields( "([0-9]{4})([0-9]{2})([0-9]{2}):([^;]+);([0-9]{4})([0-9]{2})([0-9]{2}):(.+)" );
  static const boost::regex rxTime( "([0-9]{4})-([0-9]{4})(?:,([0-9]{4})-([0-9]{4}))?" );
  dtOpen = dtClose = boost::posix_time::special_values::not_a_date_time;
  boost::cmatch what; 
  if ( !boost::regex_match( mh.c_str(), what, rxFields ) ) {
    std::runtime_error( "no proper market info found" );
  }
  else {
    // "20111015:CLOSED;20111017:1800-1715"
    // "20111015:CLOSED;20111017:0930-1600"
    // "20111119:CLOSED;20111120:0015-2355,0015-2355"
    //std::string a( what[1].first, what[1].second );
    //std::string b( what[2].first, what[2].second );
    std::string c( what[3].first, what[3].second );
    std::string d( what[4].first, what[4].second );
    //std::string e( what[5].first, what[5].second );
    //std::string f( what[6].first, what[6].second );
    //std::string g( what[7].first, what[7].second );
    //std::string h( what[8].first, what[8].second );
    if ( "CLOSED" == d ) {
    }
    else {
      if ( !boost::regex_match( d.c_str(), what, rxTime ) ) {
        std::runtime_error( "no time range found" );
      }
      else {
        std::string i( what[1].first, what[1].second );
        std::string j( what[2].first, what[2].second );
        //std::string k( what[3].first, what[3].second );
        //std::string l( what[4].first, what[4].second );
        dtOpen = boost::posix_time::from_iso_string( c + "T" + i + "00" );
        dtClose = boost::posix_time::from_iso_string( c + "T" + j + "00" );
      }
    }
  }
  if ( dtOpen > dtClose ) {
    dtOpen -= boost::gregorian::date_duration( 1 );
  }
  // adjust to previouis day
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

void IBTWS::contractDetails( int reqId, const ContractDetails& contractDetails ) {

  // instrument is constructed, but is not registered with InstrumentManager
#if DEBUG
  m_ss.str("");
  m_ss << "contract Details "
    << contractDetails.marketName << ", "
    << contractDetails.longName << ", "
    << contractDetails.summary.symbol << ", "
    << contractDetails.summary.localSymbol << ", "
    << contractDetails.summary.secType << ", "
    << contractDetails.summary.conId << ", "
    << contractDetails.summary.strike << ", "
    << contractDetails.summary.right  << ", "
    << contractDetails.summary.expiry 
    << std::endl;
//  OutputDebugString( m_ss.str().c_str() );
#endif

  assert( 0 < contractDetails.summary.conId );

  OnContractDetailsHandler_t handler = 0;
  mapActiveRequestId_t::iterator iterRequest;
  {
    boost::mutex::scoped_lock lock(m_mutexContractRequest);  // locks map updates
    iterRequest = m_mapActiveRequestId.find( reqId );  // entry removed with contractDetailsEnd
    if ( m_mapActiveRequestId.end() == iterRequest ) {
      throw std::runtime_error( "contractDetails out of sync" );  // this means the requests are in sync, and so could use linked list instead
    }
  }

  handler = iterRequest->second->fProcess;
  pInstrument_t pInstrument = iterRequest->second->pInstrument;  // might be empty

  // need some logic here:
  // * if instrument is supplied, only supplement some existing information
  // * if instrument not supplied, then go through whole building instrument exercise, or will BuildInstrument supply additional information
  // * also, need to logic to determine if we have the symbol, or not
  // * build instrument doesn't build the symbol, is performed later in the logic here, so can skip the assignment.
  // * need a truth table for what to do with instrument (present or not), and symbol(present or not)

  //  instrument *  symbol => don't build new symbol, fill new instrument with settings
  // !instrument *  symbol => assign from existing symbol, fill in... when would this happen?
  //  instrument * !symbol => build symbol, fill in and return passed in instrument
  // !instrument * !symbol => build symbol, fill in and return new instrument

  // what happens if instrument/contract already built, then in another incarnation of application, what happens?  should be ok, as contract id is in instrument.

  mapContractToSymbol_t::iterator iterMap = m_mapContractToSymbol.find( contractDetails.summary.conId );

  //assert( !( (0 != pInstrument.get()) && (m_mapContractToSymbol.end() != iter) ) );  // illegal call:  instrument and contract already exist, and should have been mated

  if ( ( 0 == pInstrument.get() ) && ( m_mapContractToSymbol.end() != iterMap ) ) {
    pInstrument = iterMap->second->GetInstrument();
  }
  
  BuildInstrumentFromContract( contractDetails.summary, pInstrument );  // creates new contract, or uses existing one

  pInstrument->SetMinTick( contractDetails.minTick );

  ptime dtOpen;
  ptime dtClose;
  typedef boost::date_time::local_adjustor<ptime, -5, us_dst> tzEST_t;
  typedef boost::date_time::local_adjustor<ptime, -4, us_dst> tzATL_t;

  if ( "EST" != contractDetails.timeZoneId ) {
    std::cout << contractDetails.longName << " differing timezones, EST vs " << contractDetails.timeZoneId << std::endl;
  }

//    std::cout << "IB: " << contractDetails.tradingHours << ", " << contractDetails.liquidHours << std::endl;

  DecodeMarketHours( contractDetails.tradingHours, dtOpen, dtClose );
  pInstrument->SetTimeTrading( 
    //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtOpen ) ), 
    //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtClose ) ) 
    // store the values in utc
    tzEST_t::local_to_utc( dtOpen ), 
    tzEST_t::local_to_utc( dtClose ) 
    );

//    std::cout << "TH: " << pInstrument->GetTimeTrading().begin() << ", " << pInstrument->GetTimeTrading().end() << std::endl;

  DecodeMarketHours( contractDetails.liquidHours, dtOpen, dtClose );
  pInstrument->SetTimeLiquid( 
    //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtOpen ) ), 
    //tzATL_t::utc_to_local( tzEST_t::local_to_utc( dtClose ) ) 
    // store the values in utc
    tzEST_t::local_to_utc( dtOpen ), 
    tzEST_t::local_to_utc( dtClose )
    );

//    std::cout << "LH: " << pInstrument->GetTimeLiquid().begin() << ", " << pInstrument->GetTimeLiquid().end() << std::endl;

  if ( m_mapContractToSymbol.end() == iterMap ) {  // create new symbol from contract
    pSymbol_t pSymbol = NewCSymbol( pInstrument );
  }

  if ( 0 != handler ) 
    handler( contractDetails, pInstrument );

}

void IBTWS::contractDetailsEnd( int reqId ) {  
  // not called when no symbol available
  OnContractDetailsDoneHandler_t handler = 0;
  {
    boost::mutex::scoped_lock lock(m_mutexContractRequest);
    mapActiveRequestId_t::iterator iterRequest = m_mapActiveRequestId.find( reqId );
    if ( m_mapActiveRequestId.end() == iterRequest ) {
      throw std::runtime_error( "contractDetailsEnd out of sync" );
    }
    reqId_t id = iterRequest->second->id;
    handler = iterRequest->second->fDone;
    m_vInActiveRequestId.push_back( iterRequest->second );
    m_mapActiveRequestId.erase( iterRequest );
//    while ( 0 != m_mapActiveRequestId.size() ) {
      // check for expired / ignored requests
//      iterRequest = m_mapActiveRequestId.begin();
//      if ( id < iterRequest->second->id ) break;
//      m_vInActiveRequestId.push_back( iterRequest->second );
//      m_mapActiveRequestId.erase( iterRequest );
//    }
  }
  if ( NULL != handler ) 
    handler();
}

void IBTWS::bondContractDetails( int reqId, const ContractDetails& contractDetails ) {
}

void IBTWS::nextValidId( OrderId orderId) {
  // todo: put in a flag to prevent orders until we've passed through this code
  m_ss.str("");
  Order::idOrder_t id = OrderManager::Instance().CheckOrderId( orderId );
  if ( orderId > id ) {
    m_ss << "old order id (" << id << "), new order id (" << orderId << ")" << std::endl;
  }
  else {
    m_ss << "next order id (" << id << "), IB had (" << orderId << ")" << std::endl;
  }
  
//  OutputDebugString( m_ss.str().c_str() );
}

// called from contractDetails, info comes from IB
void IBTWS::BuildInstrumentFromContract( const Contract& contract, pInstrument_t& pInstrument ) {

  std::string sBaseName( contract.symbol );  // need to look at this (was sUnderlying)
  std::string sLocalSymbol( contract.localSymbol );  // and this to name them properly
  std::string sExchange( contract.exchange );

  OptionSide::enumOptionSide os( OptionSide::Unknown );

  // calculate expiry, used with FuturesOption, Option   "GLD   120210C00159000"
  boost::gregorian::date dtExpiryRequested( boost::gregorian::not_a_date_time );
  boost::gregorian::date dtExpiryInSymbol( boost::gregorian::not_a_date_time );
  try {
    if ( 0 != contract.expiry.length() ) {
      // save actual date in instrument, as last-day-to-trade and expiry-date  in symbol naming varies between Fri and Sat
      dtExpiryRequested = boost::gregorian::from_undelimited_string( contract.expiry );
    }
  }
  catch ( std::exception e ) {
    //m_ss.str("");
    std::cout << "IB contract expiry is funny: " << e.what() << std::endl;
    //OutputDebugString( m_ss.str().c_str() );
  }

  InstrumentType::enumInstrumentTypes it;
  bool bFound( false );
  // could use keyword lookup here
  for ( int ix = InstrumentType::Unknown; ix < InstrumentType::_Count; ++ix ) {
    if ( 0 == strcmp( szSecurityType[ ix ], contract.secType.c_str() ) ) {
      it = static_cast<InstrumentType::enumInstrumentTypes>( ix );
      bFound = true;
      break;
    }
  }
  if ( !bFound ) 
    throw std::out_of_range( "can't find instrument type" );

  if ( "" == sExchange ) sExchange = "SMART";

  // test if arriving instrument type matches expected instrument type
  if ( 0 != pInstrument.get() ) {
    if ( pInstrument->GetInstrumentType() != it ) 
      throw std::runtime_error( "IBTWS::BuildInstrumentFromContract: Instrument types don't match" );
    pInstrument->SetExchangeName( sExchange );
  }

  m_mapSymbols_t::iterator iterSymbol;
  
  switch ( it ) {
    case InstrumentType::Stock: 
      if ( 0 == pInstrument.get() ) {
	      pInstrument = Instrument::pInstrument_t( new Instrument( sBaseName, it, sExchange ) );
      }
      else {
	      if ( pInstrument->GetInstrumentName( Instrument::eidProvider_t::EProviderIB ) != sBaseName ) 
          throw std::runtime_error( "IBTWS::BuildInstrumentFromContract: Stock, underlying no match" );
      }
      break;
    case InstrumentType::FuturesOption:
    case InstrumentType::Option:
      if ( "P" == contract.right ) os = OptionSide::Put;
      if ( "C" == contract.right ) os = OptionSide::Call;
      if ( 0 == pInstrument.get() ) {
	      pInstrument = Instrument::pInstrument_t( new Instrument( 
	        sLocalSymbol, it, sExchange, dtExpiryRequested.year(), dtExpiryRequested.month(), dtExpiryRequested.day(), 
	        os, contract.strike ) );
      }
      else {
        //if ( pInstrument->GetUnderlyingName() != sUnderlying ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract underlying doesn't match" );
        if ( pInstrument->GetOptionSide() != os ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract side doesn't match" );
        if ( pInstrument->GetExpiry() != dtExpiryRequested ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract expiry doesn't match" );  // may also need to do an off by one error, futures may not match with out day
        if ( pInstrument->GetStrike() != contract.strike ) throw std::runtime_error( "IBTWS::BuildInstrumentFromContract strike doesn't match" );  //may have rounding issues
      }
      try {
        dtExpiryInSymbol = boost::gregorian::date( boost::gregorian::date( 
          boost::lexical_cast<int>( contract.localSymbol.substr(  6, 2 ) ) + 2000,
          boost::lexical_cast<int>( contract.localSymbol.substr(  8, 2 ) ),
          boost::lexical_cast<int>( contract.localSymbol.substr( 10, 2 ) )
          ) ); 
      }
      catch ( std::exception e ) {
        std::cout << "IB option contract expiry is funny: " << e.what() << std::endl;
      }
      pInstrument->SetCommonCalcExpiry( dtExpiryInSymbol );
      break;
    case InstrumentType::Future:
      if ( 0 == pInstrument.get() ) {
	      pInstrument = Instrument::pInstrument_t( new Instrument( sBaseName, it, sExchange, dtExpiryRequested.year(), dtExpiryRequested.month() ) );
      }
      else {
      }
      break;
    case InstrumentType::Currency: {
      // 20151227 will need to step this to see if it works, with no sUnderlying
      bFound = false;
      Currency::enumCurrency base = Currency::_Count;
      for ( int ix = 0; ix < Currency::_Count; ++ix ) {
        if ( 0 == strcmp( Currency::Name[ ix ], sBaseName.c_str() ) ) {
          bFound = true;
          base = static_cast<Currency::enumCurrency>( ix );
          break;
        }
      }
      if ( !bFound ) 
        throw std::out_of_range( "base currency lookup not found" );

      const char* szCounter = NULL;
      szCounter = strchr( sLocalSymbol.c_str(), '.' );
      if ( NULL == szCounter ) 
        throw std::out_of_range( "counter currency not in LocalSymbol" );
      ++szCounter;  // advance to character after '.'

      bFound = false;
      Currency::enumCurrency counter = Currency::_Count;
      for ( int ix = 0; ix < Currency::_Count; ++ix ) {
        if ( 0 == strcmp( Currency::Name[ ix ], szCounter ) ) {
          bFound = true;
          counter = static_cast<Currency::enumCurrency>( ix );
          break;
        }
      }
      if ( !bFound ) 
        throw std::out_of_range( "counter currency lookup not found" );

      //if ( "" == sExchange )  // won't match because already set above
     sExchange = "IDEALPRO";

     assert( 0 == pInstrument.get() );  // maybe incorrect at some time in the future
      //pInstrument = Instrument::pInstrument_t( new Instrument( sLocalSymbol, sUnderlying, it, sExchange, base, counter ) );
      pInstrument = Instrument::pInstrument_t( new Instrument( sLocalSymbol, it, sExchange, base, counter ) );
      }
      break;
    case InstrumentType::Index:
      assert( 0 == pInstrument.get() );  // maybe incorrect at some time in the future
      pInstrument = Instrument::pInstrument_t( new Instrument( sLocalSymbol, it, sExchange ) );
      break;
  }
  if ( 0 == pInstrument ) 
    throw std::out_of_range( "unknown instrument type" );
  pInstrument->SetContract( contract.conId );
  //pInstrument->SetCurrency( );  // need to figure this out, use the currency calcs above
  if ( 0 < contract.multiplier.length() ) {
    pInstrument->SetMultiplier( boost::lexical_cast<unsigned long>( contract.multiplier ) );
  }
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIB, sLocalSymbol );

  //return pInstrument;  //legacy

}

void IBTWS::updatePortfolio( const Contract& contract, int position,
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
      << ", pos=" << position // int
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
      << "," << contract.expiry
      << "," << contract.primaryExchange
//      << "," << contract.conId  // long - available but don't display for now
//      << "," << contract.secId  // empty
      << "," << contract.multiplier
//      << ", type=" << InstrumentType::Name[ pInstrument->GetInstrumentType() ]
      << "," << position // int
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
    contract.conId, contract.strike, contract.expiry, contract.multiplier,
    position, marketPrice, marketValue, averageCost,
    unrealizedPNL, realizedPNL, contract.primaryExchange, contract.currency
    );
  if ( 0 != OnPositionDetailHandler ) OnPositionDetailHandler( pd );

}

// todo: use the keyword lookup to make this faster
//   key, bool, double, string
void IBTWS::updateAccountValue(const std::string& key, const std::string& val,
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
//  if ( bEmit ) {
  if ( false ) {
    std::cout << "account value " << key << ", " << val << ", " << currency << ", " << accountName << std::endl;
  }
  AccountValue av( key, val, currency, accountName );
  if ( 0 != OnAccountValueHandler ) OnAccountValueHandler( av );
}

void IBTWS::connectionClosed() {
  DEBUGOUT( "connection closed" << std::endl );
}

void IBTWS::updateMktDepth(TickerId id, int position, int operation, int side,
                            double price, int size) {
}

void IBTWS::updateMktDepthL2(TickerId id, int position, std::string marketMaker, int operation,
                              int side, double price, int size) {
}

void IBTWS::managedAccounts( const std::string& accountsList) {
}

void IBTWS::receiveFA(faDataType pFaDataType, const std::string& cxml) {
}

void IBTWS::historicalData(TickerId reqId, const std::string& date, double open, double high, 
                            double low, double close, int volume, int barCount, double WAP, int hasGaps) {
}

void IBTWS::scannerParameters(const std::string &xml) {
}

void IBTWS::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
                         const std::string& distance, const std::string& benchmark, const std::string& projection,
                         const std::string& legsStr) {
}

void IBTWS::scannerDataEnd(int reqId) {
}

void IBTWS::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
                         long volume, double wap, int count) {
}

// From EWrapper.h
char *IBTWS::TickTypeStrings[] = {
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

} // namespace tf
} // namespace ou
