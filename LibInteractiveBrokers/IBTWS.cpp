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

#include "TWS\Contract.h"
#include "TWS\Order.h"
#include "TWS\OrderState.h"
#include "TWS\Execution.h"

#include "IBTWS.h"

CIBTWS::CIBTWS( const string &acctCode, const string &address, unsigned int port ): 
  CProviderInterface<CIBTWS,CIBSymbol>(), 
  EWrapper(),
  pTWS( NULL ),
  m_sAccountCode( acctCode ), m_sIPAddress( address ), m_nPort( port ), m_curTickerId( 0 ),
  m_dblPortfolioDelta( 0 )
{
  m_sName = "IB";
  m_nID = EProviderIB;
  CIBSymbol *p = NULL;
  m_vTickerToSymbol.push_back( p );  // first ticker is 1, so preload with nothing at position 0
}

CIBTWS::~CIBTWS(void) {
  Disconnect();
  m_vTickerToSymbol.clear();  // the provider class takes care of deleting CIBSymbol.
}

void CIBTWS::Connect() {
  if ( NULL == pTWS ) {
    pTWS = new EPosixClientSocket( this );
    pTWS->eConnect( m_sIPAddress.c_str(), m_nPort );
    m_bConnected = true;
    m_thrdIBMessages = boost::thread( boost::bind( &CIBTWS::ProcessMessages, this ) );
    OnConnected( 0 );
    pTWS->reqCurrentTime();
    pTWS->reqNewsBulletins( true );
    pTWS->reqOpenOrders();
    //ExecutionFilter filter;
    //pTWS->reqExecutions( filter );
    pTWS->reqAccountUpdates( true, "" );
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

CIBSymbol *CIBTWS::NewCSymbol( const std::string &sSymbolName ) {
  TickerId ticker = ++m_curTickerId;
  CIBSymbol *pSymbol = new CIBSymbol( sSymbolName, ticker );
  CProviderInterface<CIBTWS,CIBSymbol>::AddCSymbol( pSymbol );
  m_vTickerToSymbol.push_back( pSymbol );
  return pSymbol;
}

void CIBTWS::StartQuoteWatch(CIBSymbol *pSymbol) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopQuoteWatch(CIBSymbol *pSymbol) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartTradeWatch(CIBSymbol *pSymbol) {  // overridden from base class
  StartQuoteTradeWatch( pSymbol );
}

void CIBTWS::StopTradeWatch(CIBSymbol *pSymbol) {  // overridden from base class
  StopQuoteTradeWatch( pSymbol );
}

void CIBTWS::StartQuoteTradeWatch( CIBSymbol *pIBSymbol ) {
  //CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( !pIBSymbol->GetQuoteTradeWatchInProgress() ) {
    // start watch
    pIBSymbol->SetQuoteTradeWatchInProgress();
    Contract contract;
    contract.symbol = pIBSymbol->Name().c_str();
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.secType = "STK";  // todo:  get this information from the symbol
    //pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "100,101,104,165,221,225,236", false );
    pTWS->reqMktData( pIBSymbol->GetTickerId(), contract, "", false );
  }
}

void CIBTWS::StopQuoteTradeWatch( CIBSymbol *pIBSymbol ) {
  //CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( pIBSymbol->QuoteWatchNeeded() || pIBSymbol->TradeWatchNeeded() ) {
    // don't do anything if either a quote or trade watch still in progress
  }
  else {
    // stop watch
    pTWS->cancelMktData( pIBSymbol->GetTickerId() );
    pIBSymbol->ResetQuoteTradeWatchInProgress();
  }
}

void CIBTWS::StartDepthWatch(CIBSymbol *pIBSymbol) {  // overridden from base class
  //CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
  if ( !pIBSymbol->GetDepthWatchInProgress() ) {
    // start watch
    pIBSymbol->SetDepthWatchInProgress();
  }
}

void CIBTWS::StopDepthWatch(CIBSymbol *pIBSymbol) {  // overridden from base class
  //CIBSymbol *pIBSymbol = (CIBSymbol *) pSymbol;
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
//long CIBTWS::nOrderId = 1;

void CIBTWS::PlaceOrder( COrder *pOrder ) {

  CProviderInterface<CIBTWS,CIBSymbol>::PlaceOrder( pOrder ); // any underlying initialization
  Order twsorder; 
  twsorder.orderId = pOrder->GetOrderId();
  assert( twsorder.orderId >= m_idNextValid );

  //Contract contract2;
  //contract2.conId = 44678227;
  //pTWS->reqContractDetails( contract2 );

  Contract contract;
  contract.symbol = pOrder->GetInstrument()->GetSymbolName();
  contract.currency = pOrder->GetInstrument()->GetCurrencyName();
  contract.exchange = (*(pOrder->GetInstrument()->GetExchangeName()));
  contract.secType = szSecurityType[ pOrder->GetInstrument()->GetInstrumentType() ];
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
  pTWS->placeOrder( twsorder.orderId, contract, twsorder );
}

void CIBTWS::CancelOrder( COrder *pOrder ) {
  CProviderInterface<CIBTWS,CIBSymbol>::CancelOrder( pOrder );
  pTWS->cancelOrder( pOrder->GetOrderId() );
}

void CIBTWS::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
    //std::cout << "tickPrice " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << price << std::endl;
    pSym->AcceptTickPrice( tickType, price );
  }
}

void CIBTWS::tickSize( TickerId tickerId, TickType tickType, int size) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
    //std::cout << "tickSize " << pSym->Name() << ", " << TickTypeStrings[tickType] << ", " << size << std::endl;
    pSym->AcceptTickSize( tickType, size );
  }
}

void CIBTWS::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice ) {

  mapTickerIdToContract_t::iterator iterTicker = m_mapTickerIdToContract.find( tickerId );
  if ( m_mapTickerIdToContract.end() != iterTicker ) {
    mapDelta_t::iterator iter = m_mapDelta.find( iterTicker->second );
    if ( m_mapDelta.end() == iter ) {
      m_ss.str("");
      m_ss << "can't find option map fo ticker=" << tickerId << std::endl;
      OutputDebugString( m_ss.str().c_str() );
    }
    else {
  //    if ( ( MODEL_OPTION == tickType ) || ( LAST_OPTION_COMPUTATION == tickType ) ) {
      if ( ( MODEL_OPTION == tickType ) || ( false ) ) {
        iter->second.impliedVolatility = impliedVol;
        if ( MODEL_OPTION == tickType ) {
//          iter->second.modelPrice = modelPrice;
          m_dblPortfolioDelta -= iter->second.positionDelta;
          iter->second.delta = delta;
          iter->second.positionCalc = iter->second.position;
          iter->second.positionDelta = 100 * iter->second.position * delta;
          m_dblPortfolioDelta += iter->second.positionDelta;
        }
      }
    }
  //  if ( ( MODEL_OPTION == tickType ) || ( LAST_OPTION_COMPUTATION == tickType ) ) {
      if ( ( MODEL_OPTION == tickType ) || ( false ) ) {
      m_ss.str(""); 
      m_ss
        << "tickOptionComputation " 
        << iter->second.sUnderlying
        << " " << iter->second.sSymbol
    //    << "tickerid=" << tickerId
        << " " << TickTypeStrings[tickType] 
        << ", Implied Vol=" << impliedVol
        << ", Delta=" << delta
//        << ", ModelPrice=" << modelPrice
        << ", MarketPrice=" << iter->second.marketPrice
        << ", PositionDelta=" << iter->second.positionDelta
        << ", PortfolioDelta=" << m_dblPortfolioDelta
  //      << ", Dividend=" << pvDividend
        << std::endl; 
      OutputDebugString( m_ss.str().c_str() );
    }
  }

}

void CIBTWS::tickGeneric(TickerId tickerId, TickType tickType, double value) {
//  std::cout << "tickGeneric " << m_vTickerToSymbol[ tickerId ]->Name() << ", " << TickTypeStrings[tickType] << ", " << value << std::endl;
}

void CIBTWS::tickString(TickerId tickerId, TickType tickType, const IBString& value) {
  // we seem to get ticks even though we havn't requested them, so ensure we only accept 
  //   when a valid symbol has been defined
  if ( ( tickerId > 0 ) && ( tickerId <= m_curTickerId ) ) {
    CIBSymbol *pSym = m_vTickerToSymbol[ tickerId ];
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
    OutputDebugString( m_ss.str().c_str() );
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
    OutputDebugString( m_ss.str().c_str() );
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
//  ****************** need to redo this, orderid disappeared, to be replacedby reqId.
  m_ss.str("");
  m_ss  
    << "execDetails: " 
    << "  sym=" << contract.symbol 
//    << ", oid=" << orderId 
    //<< ", ex.oid=" << execution.orderId 
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
  if ( OrderSide::Unknown == side ) std::cout << "Unknown execution side: " << execution.side << std::endl;
  else {
//    CExecution exec( orderId, execution.price, execution.shares, side, 
//      execution.exchange, execution.execId );
//    COrderManager::Instance().ReportExecution( exec );
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

void CIBTWS::error(const int id, const int errorCode, const IBString errorString) {
  switch ( errorCode ) {
    case 1102: // Connectivity has been restored
      pTWS->reqAccountUpdates( true, "" );
      break;
    case 2104:  // datafarm connected ok
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
  m_ss << "current time " << time << endl;
  OutputDebugString( m_ss.str().c_str() );
  m_time = time;
}

void CIBTWS::updateAccountTime(const IBString& timeStamp) {
}

void CIBTWS::contractDetails( int reqId, const ContractDetails& contractDetails ) {
  m_ss.str("");
  m_ss << "contract Details " << 
    contractDetails.orderTypes << ", " << contractDetails.minTick << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CIBTWS::bondContractDetails( int reqId, const ContractDetails& contractDetails ) {
}

void CIBTWS::nextValidId( OrderId orderId) {
  m_ss.str("");
  m_ss << "next valid id " << orderId << std::endl;
  OutputDebugString( m_ss.str().c_str() );
  m_idNextValid = orderId;
}

void CIBTWS::updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const IBString& accountName) {

  mapDelta_t::iterator iter = m_mapDelta.find( contract.conId );
  if ( m_mapDelta.end() == iter ) {

    std::string sUnderlying( contract.symbol );
    std::string sLocalSymbol( contract.localSymbol );

    InstrumentType::enumInstrumentTypes it;
    bool bFound = false;
    for ( int ix = InstrumentType::Unknown; ix < InstrumentType::_Count; ++ix ) {
      if ( 0 == strcmp( szSecurityType[ ix ], contract.secType.c_str() ) ) {
        it = static_cast<InstrumentType::enumInstrumentTypes>( ix );
        bFound = true;
        break;
      }
    }
    if ( !bFound ) throw std::out_of_range( "can't find instrument type" );

    ptime dtExpiry = not_a_date_time;
    OptionSide::enumOptionSide os = OptionSide::Unknown;
    if ( 0 != contract.expiry.length() ) {
      std::string s( contract.expiry );
      boost::gregorian::date d( boost::gregorian::from_undelimited_string( s ) );
      dtExpiry = ptime( d );
    }
    std::string sExchange( contract.exchange );

    CInstrument::pInstrument_t pInstrument;
    switch ( it ) {
      case InstrumentType::Stock: 
        if ( "" == sExchange ) sExchange = "SMART";
        pInstrument = CInstrument::pInstrument_t( new CInstrument( sUnderlying, sExchange, it ) );
        break;
      case InstrumentType::FuturesOption:
      case InstrumentType::Option:
        if ( "P" == contract.right ) os = OptionSide::Put;
        if ( "C" == contract.right ) os = OptionSide::Call;
        if ( "" == sExchange ) sExchange = "SMART";
        pInstrument = CInstrument::pInstrument_t( new CInstrument( 
          sLocalSymbol, sExchange, it, dtExpiry.date().year(), dtExpiry.date().month(), sUnderlying, os, contract.strike ) );
        break;
      case InstrumentType::Future:
        if ( "" == sExchange ) sExchange = "SMART";
        pInstrument = CInstrument::pInstrument_t( new CInstrument( sUnderlying, sExchange, it, dtExpiry.date().year(), dtExpiry.date().month() ) );
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
        if ( !bFound ) throw std::out_of_range( "base currency lookup not found" );

        const char* szCounter = NULL;
        szCounter = strchr( sLocalSymbol.c_str(), '.' );
        if ( NULL == szCounter ) throw std::out_of_range( "counter currency not in LocalSymbol" );
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
        if ( !bFound ) throw std::out_of_range( "counter currency lookup not found" );

        if ( "" == sExchange ) sExchange = "IDEALPRO";

        pInstrument = CInstrument::pInstrument_t( new CInstrument( sLocalSymbol, sUnderlying, it, base, counter ) );
        break;
    }
    if ( NULL == pInstrument ) throw std::out_of_range( "instrument type not accounted for" );
    pInstrument->SetContract( contract.conId );

    //CIBSymbol* pSymbol = dynamic_cast<CIBSymbol*>( NewCSymbol( sLocalSymbol ) );  // *** this isn't done correctly, but good enough for now
    CIBSymbol* pSymbol = NewCSymbol( sLocalSymbol );

    structDeltaStuff stuff;
//    stuff.tickerId = pSymbol->GetTickerId();
    stuff.pInstrument = pInstrument;
    stuff.sSymbol = sLocalSymbol;
    stuff.sUnderlying = sUnderlying;
    stuff.delta = 0;
    stuff.impliedVolatility = 0;
    stuff.modelPrice = 0;
    stuff.strike = contract.strike;
    stuff.dtExpiry = dtExpiry;
    stuff.position = position;
    stuff.positionCalc = 0;
    stuff.positionDelta = 0;
    stuff.marketPrice = marketPrice;
    stuff.averageCost = averageCost;
    stuff.os = os;

    m_mapDelta.insert( pair_mapDelta_t( contract.conId, stuff ) );
    m_mapTickerIdToContract.insert( pair_mapTickerIdToContract_t( stuff.tickerId, contract.conId ) );

    iter = m_mapDelta.find( contract.conId );  // load iter for status print at end of this method

    Contract contractMktData;
    contractMktData = contract;
    contractMktData.exchange = sExchange.c_str();
    pTWS->reqMktData( stuff.tickerId, contractMktData, "100,101,104,106,221,225", false );
    iter->second.bDataRequested = true;

  }
  else {
    iter->second.position = position;
    iter->second.marketPrice = marketPrice;
    iter->second.averageCost = averageCost;
  }

  // start/stop data depending upon position size
  if ( 0 == position ) {
    if ( iter->second.bDataRequested ) {
      pTWS->cancelMktData( iter->second.tickerId );
      iter->second.bDataRequested = false;
    }
  }
  else { // we have a position, so need data
    if ( !iter->second.bDataRequested ) {
      Contract contractMktData;
      contractMktData = contract;
      contractMktData.exchange = iter->second.pInstrument.get()->GetExchangeName()->c_str();
      pTWS->reqMktData( iter->second.tickerId, contractMktData, "100,101,104,106,221,225", false );
      iter->second.bDataRequested = true;
    }
  }

  if ( true ) {
    m_ss.str("");
    m_ss << "portfolio item " 
      << contract.symbol
      << " " << contract.localSymbol
      << "  id=" << contract.conId  // long
      << ", type=" << InstrumentType::Name[ iter->second.pInstrument->GetInstrumentType() ]
      << ", strike=" << contract.strike // double
      << ", expire=" << iter->second.dtExpiry
      << ", right=" << OptionSide::Name[ iter->second.os ]
      << ", pos=" << position // int
      << ", price=" << marketPrice // double
      << ", val=" << marketValue // double
      << ", cost=" << averageCost // double
      << ", uPL=" << unrealizedPNL // double
      << ", rPL=" << realizedPNL // double
      //<< ", " << accountName 
      << std::endl;
    OutputDebugString( m_ss.str().c_str() );
  }

  CPortfolio::structUpdatePortfolioRecord record( iter->second.pInstrument.get(), position, marketPrice, averageCost );
  OnUpdatePortfolioRecord( record );

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