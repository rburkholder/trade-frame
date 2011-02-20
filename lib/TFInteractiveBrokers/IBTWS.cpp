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

#include "StdAfx.h"

#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

#include <boost/lexical_cast.hpp>

#include <TFTrading/KeyTypes.h>
#include <TFTrading/OrderManager.h>

#include "IBTWS.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CIBTWS::CIBTWS( const std::string &acctCode, const std::string &address, unsigned int port ): 
  CProviderInterface<CIBTWS,CIBSymbol>(), 
  EWrapper(),
  pTWS( NULL ),
  m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 ),
//  m_dblPortfolioDelta( 0 ),
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

CIBTWS::~CIBTWS(void) {
  Disconnect();
  m_vTickerToSymbol.clear();
  m_mapContractToSymbol.clear();
}

void CIBTWS::Connect() {
  if ( NULL == pTWS ) {
    pTWS = new EPosixClientSocket( this );
    bool bReturn = pTWS->eConnect( m_sIPAddress.c_str(), m_nPort );
    assert( bReturn );
    m_bConnected = true;
    m_thrdIBMessages = boost::thread( boost::bind( &CIBTWS::ProcessMessages, this ) );
    pTWS->reqCurrentTime();
    pTWS->reqNewsBulletins( true );
    pTWS->reqOpenOrders();
    //ExecutionFilter filter;
    //pTWS->reqExecutions( filter );
    pTWS->reqAccountUpdates( true, "" );
    OnConnected( 0 );
  }
}

void CIBTWS::Disconnect() {
  // check to see if there are any watches happening, and get them disconnected
  if ( NULL != pTWS ) {
    m_bConnected = false;
    pTWS->eDisconnect();
    m_thrdIBMessages.join();  // wait for message processing to exit
    delete pTWS;
    pTWS = NULL;
    OnDisconnected( 0 );
    m_ss.str("");
    m_ss << "IB Disconnected " << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }
}

// this is executed in non-main thread, and the events below will be called from the processing here
void CIBTWS::ProcessMessages( void ) {
  bool bOK = true;
  while ( bOK && m_bConnected ) {
    bOK = pTWS->checkMessages();
  }
  m_bConnected = false;  // placeholder for debug

  // need to deal with pre=mature exit so that flags get reset
  // maybe a state machine would keep track
}

//CIBSymbol *CIBTWS::NewCSymbol( const std::string &sSymbolName ) {
CIBTWS::pSymbol_t CIBTWS::NewCSymbol( CIBSymbol::pInstrument_t pInstrument ) {
  // todo:  check that contract doesn't already exist
  TickerId ticker = ++m_curTickerId;
  pSymbol_t pSymbol( new CIBSymbol( pInstrument, ticker ) );  // is there someplace with the IB specific symbol name, or is it set already?
  // todo:  do an existance check on the instrument/symbol
  CProviderInterface<CIBTWS,CIBSymbol>::AddCSymbol( pSymbol );
  m_vTickerToSymbol.push_back( pSymbol );
  m_mapContractToSymbol.insert( pair_mapContractToSymbol_t( pInstrument->GetContract(), pSymbol ) );
  return pSymbol;
}

void CIBTWS::StartQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopQuoteWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopTradeWatch( pSymbol_t pSymbol ) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( !pIBSymbol->GetQuoteTradeWatchInProgress() ) {
    // start watch
    Contract contract;
    contract.conId = pIBSymbol->GetInstrument()->GetContract();  // mostly enough to have contract id
    contract.exchange = pIBSymbol->GetInstrument()->GetExchangeName();
    contract.currency = pIBSymbol->GetInstrument()->GetCurrencyName();
    pIBSymbol->SetQuoteTradeWatchInProgress();
    //pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "100,101,104,165,221,225,236", false );
    pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "", false );
  }
}

void CIBTWS::StopQuoteTradeWatch( pSymbol_t pIBSymbol ) {
  if ( pIBSymbol->QuoteWatchNeeded() || pIBSymbol->TradeWatchNeeded() ) {
    // don't do anything if either a quote or trade watch still in progress
  }
  else {
    // stop watch
    pTWS->cancelMktData( pIBSymbol->GetTickerId() );
    pIBSymbol->ResetQuoteTradeWatchInProgress();
  }
}

void CIBTWS::StartDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
  if ( !pIBSymbol->GetDepthWatchInProgress() ) {
    // start watch
    pIBSymbol->SetDepthWatchInProgress();
  }
}

void CIBTWS::StopDepthWatch( pSymbol_t pIBSymbol) {  // overridden from base class
  if ( pIBSymbol->DepthWatchNeeded() ) {
  }
  else {
    // stop watch
    pIBSymbol->ResetDepthWatchInProgress();
  }
}

// indexed with InstrumentType::enumInstrumentTypes
const char *CIBTWS::szSecurityType[] = { "NULL", "STK", "OPT", "FUT", "FOP", "CASH", "IND" };  // InsrumentType::enumInstrumentType
const char *CIBTWS::szOrderType[] = { "UNKN", "MKT", "LMT", "STP", "STPLMT", "NULL",     // OrderType::enumOrderType
                   "TRAIL", "TRAILLIMIT", "MKTCLS", "LMTCLS", "SCALE" };

void CIBTWS::PlaceOrder( pOrder_t pOrder ) {

  Order twsorder; 
  twsorder.orderId = pOrder->GetOrderId();

  Contract contract;
  contract.conId = pOrder->GetInstrument()->GetContract();  // mostly enough to have contract id
  contract.exchange = pOrder->GetInstrument()->GetExchangeName();
  contract.currency = pOrder->GetInstrument()->GetCurrencyName();

  IBString s;
  switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
    case InstrumentType::Stock:
      contract.exchange = "SMART";
      break;
    case InstrumentType::Future:
      assert( false );  // need to fix this formatter
//      s.Format( "%04d%02d", pOrder->GetInstrument()->GetExpiryYear(), pOrder->GetInstrument()->GetExpiryMonth() );
//      contract.expiry = s;
      if ( "CBOT" == contract.exchange ) contract.exchange = "ECBOT";
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

  CProviderInterface<CIBTWS,CIBSymbol>::PlaceOrder( pOrder ); // any underlying initialization
  pTWS->placeOrder( twsorder.orderId, contract, twsorder );
}

void CIBTWS::CancelOrder( pOrder_t pOrder ) {
  CProviderInterface<CIBTWS,CIBSymbol>::CancelOrder( pOrder );
  pTWS->cancelOrder( pOrder->GetOrderId() );
}

void CIBTWS::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickPrice " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << price << std::endl;
    pSym->AcceptTickPrice( tickType, price );
  }
}

void CIBTWS::tickSize( TickerId tickerId, TickType tickType, int size) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickSize " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << size << std::endl;
    pSym->AcceptTickSize( tickType, size );
  }
}

void CIBTWS::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice ) {

  CIBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
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

void CIBTWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
//  std::cout << "tickGeneric " << m_vTickerToSymbol[ tickerId ]->Name() << ", " << TickTypeStrings[tickType] << ", " << value << std::endl;
}

void CIBTWS::tickString(TickerId tickerId, TickType tickType, const IBString& value) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol::pSymbol_t pSym( m_vTickerToSymbol[ tickerId ] );
    //std::cout << "tickString " << pSym->Name() << ", " 
    //  << TickTypeStrings[tickType] << ", " << value;
    //std::cout << std::endl;
    pSym->AcceptTickString( tickType, value );
  }
}

void CIBTWS::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints,
  double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry ) {
  m_ss.str("");
  m_ss << "tickEFP" << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CIBTWS::orderStatus( OrderId orderId, const IBString &status, int filled,
                         int remaining, double avgFillPrice, int permId, int parentId,
                         double lastFillPrice, int clientId, const IBString& whyHeld) 
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
//    OutputDebugString( m_ss.str().c_str() );
  }
}

void CIBTWS::openOrder( OrderId orderId, const Contract& contract, const Order& order, const OrderState& state) {
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
    OutputDebugString( m_ss.str().c_str() );
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
      << ", state.warning=" << state.warningText 
      << std::endl; 
//    OutputDebugString( m_ss.str().c_str() );
    //if ( std::numeric_limits<double>::max(0) != state.commission ) 
    if ( 1e308 > state.commission ) 
      COrderManager::Instance().ReportCommission( orderId, state.commission ); 
  }
  if ( state.warningText != "" ) {
    m_ss.str("");
    m_ss << "Open Order Warning: " << state.warningText << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }
}

void CIBTWS::execDetails( int reqId, const Contract& contract, const Execution& execution ) {
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
  OutputDebugString( m_ss.str().c_str() );

  OrderSide::enumOrderSide side = OrderSide::Unknown;
  if ( "BOT" == execution.side ) side = OrderSide::Buy;  // could try just first character for fast comparison
  if ( "SLD" == execution.side ) side = OrderSide::Sell;
  if ( OrderSide::Unknown == side ) {
    m_ss.str("");
    m_ss << "Unknown execution side: " << execution.side << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }
  else {
    CExecution exec( execution.price, execution.shares, side, execution.exchange, execution.execId );
    COrderManager::Instance().ReportExecution( execution.orderId, exec );
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
CIBTWS::pSymbol_t CIBTWS::GetSymbol( long ContractId ) {
  mapContractToSymbol_t::iterator iterId = m_mapContractToSymbol.find( ContractId );
  if ( m_mapContractToSymbol.end() == iterId ) {
    throw std::out_of_range( "can't find contract" );
  }
  return iterId->second;
}

// check for symbol existance, and return, else add and return
CIBTWS::pSymbol_t CIBTWS::GetSymbol( pInstrument_t instrument ) {

  long contractId;
  contractId = instrument->GetContract();
  assert( 0 != contractId );

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

void CIBTWS::error(const int id, const int errorCode, const IBString errorString) {
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
      m_ss.str("");
      m_ss << "error " << id << ", " << errorCode << ", " << errorString << std::endl;
      OutputDebugString( m_ss.str().c_str() );
      break;
  }
}

void CIBTWS::winError( const IBString &str, int lastError) {
  m_ss.str("");
  m_ss << "winerror " << str << ", " << lastError << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CIBTWS::updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch) {
  m_ss.str("");
  m_ss << "news bulletin " << msgId << ", " << msgType << ", " << newsMessage << ", " << originExch << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CIBTWS::currentTime(long time) {
  m_ss.str("");
  m_ss << "current time " << time << std::endl;
  OutputDebugString( m_ss.str().c_str() );
  m_time = time;
}

void CIBTWS::updateAccountTime(const IBString& timeStamp) {
}

void CIBTWS::contractDetails( int reqId, const ContractDetails& contractDetails ) {
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
  OutputDebugString( m_ss.str().c_str() );
  if ( NULL != OnContractDetails ) OnContractDetails( contractDetails );
}

void CIBTWS::contractDetailsEnd( int reqId ) {
  GiveBackReqId( reqId );
  if ( NULL != OnContractDetailsDone ) OnContractDetailsDone();
}

void CIBTWS::bondContractDetails( int reqId, const ContractDetails& contractDetails ) {
}

void CIBTWS::nextValidId( OrderId orderId) {
  // todo: put in a flag to prevent orders until we've passed through this code
  m_ss.str("");
  COrder::idOrder_t id = COrderManager::Instance().CheckOrderId( orderId );
  if ( orderId > id ) {
    m_ss << "old order id (" << id << "), new order id (" << orderId << ")" << std::endl;
  }
  else {
    m_ss << "next order id (" << id << "), IB had (" << orderId << ")" << std::endl;
  }
  
  OutputDebugString( m_ss.str().c_str() );
}

CIBTWS::pInstrument_t CIBTWS::BuildInstrumentFromContract( const Contract& contract ) {

  pInstrument_t pInstrument;

  std::string sUnderlying( contract.symbol );
  std::string sLocalSymbol( contract.localSymbol );
  std::string sExchange( contract.exchange );

  OptionSide::enumOptionSide os = OptionSide::Unknown;

  // calculate expiry, used with FuturesOption, Option, Future
  ptime dtExpiry = not_a_date_time;
  try {  // is this only calculated on futures and options?
    if ( 0 != contract.expiry.length() ) {
      std::string s( contract.expiry );
      boost::gregorian::date d( boost::gregorian::from_undelimited_string( s ) );
      dtExpiry = ptime( d );
    }
  }
  catch ( std::exception e ) {
      m_ss.str("");
      m_ss << "contract expiry is funny: " << e.what() << std::endl;
      OutputDebugString( m_ss.str().c_str() );
  }

  InstrumentType::enumInstrumentTypes it;
  bool bFound = false;
  for ( int ix = InstrumentType::Unknown; ix < InstrumentType::_Count; ++ix ) {
    if ( 0 == strcmp( szSecurityType[ ix ], contract.secType.c_str() ) ) {
      it = static_cast<InstrumentType::enumInstrumentTypes>( ix );
      bFound = true;
      break;
    }
  }
  if ( !bFound ) 
    throw std::out_of_range( "can't find instrument type" );

  m_mapSymbols_t::iterator iterSymbol;

  switch ( it ) {
    case InstrumentType::Stock: 
      if ( "" == sExchange ) sExchange = "SMART";
      pInstrument = CInstrument::pInstrument_t( new CInstrument( sUnderlying, it, sExchange ) );
      break;
    case InstrumentType::FuturesOption:
    case InstrumentType::Option:
      if ( "P" == contract.right ) os = OptionSide::Put;
      if ( "C" == contract.right ) os = OptionSide::Call;
      if ( "" == sExchange ) sExchange = "SMART";
      iterSymbol = m_mapSymbols.find( sUnderlying );
      if ( m_mapSymbols.end() == iterSymbol ) {
        throw std::runtime_error( "CIBTWS::BuildInstrumentFromContract underlying not found" );
      }
      pInstrument = CInstrument::pInstrument_t( new CInstrument( 
        sLocalSymbol, it, sExchange, dtExpiry.date().year(), dtExpiry.date().month(), dtExpiry.date().day(), 
        iterSymbol->second->GetInstrument(), 
        os, contract.strike ) );
      break;
    case InstrumentType::Future:
      if ( "" == sExchange ) sExchange = "SMART";
      pInstrument = CInstrument::pInstrument_t( new CInstrument( sUnderlying, it, sExchange, dtExpiry.date().year(), dtExpiry.date().month() ) );
      break;
    case InstrumentType::Currency:
      bFound = false;
      Currency::enumCurrency base = Currency::_Count;
      for ( int ix = 0; ix < Currency::_Count; ++ix ) {
        if ( 0 == strcmp( Currency::Name[ ix ], sUnderlying.c_str() ) ) {
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

      if ( "" == sExchange ) sExchange = "IDEALPRO";

      pInstrument = CInstrument::pInstrument_t( new CInstrument( sLocalSymbol, sUnderlying, it, sExchange, base, counter ) );
      break;
  }
  if ( NULL == pInstrument ) 
    throw std::out_of_range( "instrument type not accounted for" );
  pInstrument->SetContract( contract.conId );
  if ( 0 < contract.multiplier.length() ) {
    pInstrument->SetMultiplier( boost::lexical_cast<unsigned long>( contract.multiplier ) );
  }

  return pInstrument;

}

void CIBTWS::updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const IBString& accountName) {

//  pInstrument_t pInstrument;
//  CIBSymbol* pSymbol;
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

  if ( true ) {
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
//    OutputDebugString( m_ss.str().c_str() );
  }

//  CPortfolio::structUpdatePortfolioRecord record( pInstrument.get(), position, marketPrice, averageCost );
//  OnUpdatePortfolioRecord( record );

}

// todo: use the keyword lookup to make this faster
//   key, bool, double, string
void CIBTWS::updateAccountValue(const IBString& key, const IBString& val,
                                const IBString& currency, const IBString& accountName) {
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
    //std::cout << "account value " << key << ", " << val << ", " << currency << ", " << accountName << std::endl;
  }
}

void CIBTWS::connectionClosed() {
  m_ss.str("");
  m_ss << "connection closed" << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CIBTWS::updateMktDepth(TickerId id, int position, int operation, int side,
                            double price, int size) {
}

void CIBTWS::updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation,
                              int side, double price, int size) {
}

void CIBTWS::managedAccounts( const IBString& accountsList) {
}

void CIBTWS::receiveFA(faDataType pFaDataType, const IBString& cxml) {
}

void CIBTWS::historicalData(TickerId reqId, const IBString& date, double open, double high, 
                            double low, double close, int volume, int barCount, double WAP, int hasGaps) {
}

void CIBTWS::scannerParameters(const IBString &xml) {
}

void CIBTWS::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
                         const IBString &distance, const IBString &benchmark, const IBString &projection,
                         const IBString &legsStr) {
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

} // namespace tf
} // namespace ou
