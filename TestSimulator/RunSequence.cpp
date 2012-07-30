/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <TFSimulation/SimulationProvider.h>

#include "RunSequence.h"

RunSequence::RunSequence( const boost::gregorian::date& dateStart ) 
  : m_portfolio( "test" ),
      m_stateTimeFrame( EPreOpen ), m_stateTrading( ENeutral ),
    m_timeOpeningBell( 19, 0, 0 ),
    m_timeCancelTrades( 16, 40, 0 ),
    m_timeClosePositions( 16, 45, 0 ),
    m_timeClosingBell( 17, 0, 0 ),
    m_dtOpeningBell( dateStart, m_timeOpeningBell ),
    m_dtStartTrading( m_dtOpeningBell + time_duration( 0, 16, 0 ) ),
    m_dtCancelTrades( dateStart + date_duration( 1 ), m_timeCancelTrades ),
    m_dtClosePositions( dateStart + date_duration( 1 ), m_timeClosePositions ),
    m_dtClosingBell( dateStart + date_duration( 1 ), m_timeClosingBell )
{
}

RunSequence::~RunSequence(void) {
}

void RunSequence::Run( void ) {

  ;

  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;
  pProviderSim_t m_pProvider;

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
//  m_pInstrument = mgr.Exists( "TEST" ) ? mgr.Get( "TEST" ) : mgr.ConstructInstrument( "TEST", "SMART", ou::tf::InstrumentType::Stock );
  m_pInstrument = mgr.Exists( "+GCQ12" ) ? mgr.Get( "+GCQ12" ) : mgr.ConstructFuture( "+GCQ12", "SMART", 2012, 8 );
  m_pInstrument->SetMultiplier( 100 );
  m_pInstrument->SetMinTick( 0.1 );

  m_pProvider.reset( new ou::tf::CSimulationProvider );
  //m_pProvider->SetGroupDirectory( "/app/TestSimulator" );
  m_pProvider->SetGroupDirectory( "/app/semiauto/2012-Jul-22 18:08:14.285807" );

  m_pProvider->OnConnected.Add( MakeDelegate( this, &RunSequence::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Add( MakeDelegate( this, &RunSequence::HandleProviderDisconnected ) );
  m_pProvider->Connect();

  m_pProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleQuote2 ) );
  m_pProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleTrade ) );

  m_pProvider->SetOnSimulationComplete( MakeDelegate( this, &RunSequence::HandleSimulationComplete ) );

  m_pPosition.reset( new ou::tf::CPosition( m_pInstrument, m_pProvider, m_pProvider ) );
  m_portfolio.AddPosition( "pos", m_pPosition );

  m_portfolio.OnExecution.Add( MakeDelegate( this, &RunSequence::HandlePortfolioExecution ) );
  m_portfolio.OnCommission.Add( MakeDelegate( this, &RunSequence::HandlePortfolioCommission ) );

  m_pPosition->OnExecution.Add( MakeDelegate( this, &RunSequence::HandlePositionExecution ) );
  m_pPosition->OnCommission.Add( MakeDelegate( this, &RunSequence::HandlePositionCommission ) );

  m_pProvider->Run( false );

  m_portfolio.OnExecution.Remove( MakeDelegate( this, &RunSequence::HandlePortfolioExecution ) );
  m_portfolio.OnCommission.Remove( MakeDelegate( this, &RunSequence::HandlePortfolioCommission ) );

  m_pProvider->SetOnSimulationComplete( 0 );

  m_pProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleQuote2 ) );
  m_pProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleTrade ) );

  m_pProvider->OnConnected.Remove( MakeDelegate( this, &RunSequence::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Remove( MakeDelegate( this, &RunSequence::HandleProviderDisconnected ) );

}

void RunSequence::HandleProviderConnected( int ) {
}

void RunSequence::HandleProviderDisconnected( int ) {
}

void RunSequence::HandleSimulationComplete( void ) {
  std::stringstream ss;
  std::cout << "Portfolio: " << std::endl;  // does postions and portfolio
  m_portfolio.EmitStats( ss );
  std::cout << ss.str() << std::endl;
}

void RunSequence::HandleTrade( const ou::tf::Trade& trade ) {
  m_trade = trade;
  ptime dt( trade.DateTime() );
  std::cout << dt << " trade " << std::endl;
  m_trades.Append( trade );
  static ptime mark( date( 2012, 7, 22 ), time_duration( 22, 46, 42, 621469 ) );
  if ( trade.DateTime() == mark ) {
    std::cout << dt << " trade " << std::endl;
  }
}

void RunSequence::HandleQuote1( const ou::tf::Quote& quote ) {
  ptime dt( quote.DateTime() );
  std::cout << dt << " quote: B:" << quote.Bid() << "-A:" << quote.Ask() << std::endl;
  m_quotes.Append( quote );
  switch ( m_quotes.Size() ) {
  case 4:
    std::cout << "  order placed " << std::endl;
    m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 100, 23.0 );
    break;
  case 7:
    std::cout << "  mkt order placed " << std::endl;
    m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
    break;
  case 13:
    std::cout << "  low limit placed " << std::endl;
    m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1000, 16.0 );
    break;
  case 86:
    std::cout << "  pos cancelled " << std::endl;
    m_pPosition->CancelOrders();
    break;
  case 89:
    std::cout << "  pos closed " << std::endl;
    m_pPosition->ClosePosition();
    break;
  }
}

void RunSequence::HandleQuote2( const ou::tf::Quote& quote ) {

  m_quote = quote;
  ptime dt( quote.DateTime() );
  std::cout << dt << " quote: B:" << quote.Bid() << "-A:" << quote.Ask() << std::endl;

  static ptime mark( date( 2012, 7, 22 ), time_duration( 21, 7, 5, 368590 ) );
  if ( quote.DateTime() == mark ) {
    std::cout << dt << " quote " << std::endl;
  }

  if ( quote.IsValid() ) {
    m_quotes.Append( quote );

    switch ( m_stateTimeFrame ) {
    case EPreOpen:
      if ( m_dtOpeningBell <= dt ) {
        m_stateTimeFrame = EBellHeard;
      }
      break;
    case EBellHeard:
      m_stateTimeFrame = EPauseForQuotes;
      break;
    case EPauseForQuotes:
      if ( m_dtStartTrading <= dt ) {
        m_stateTimeFrame = EAfterBell;
      }
      break;
    case EAfterBell:
      m_stateTimeFrame = ETrading;
      break;
    case ETrading:
      if ( m_dtCancelTrades <= dt ) {
        m_pPosition->CancelOrders();
        m_stateTimeFrame = ECancelling;
      }
      else {
        Trade();
      }
      break;
    case ECancelling:
      if ( m_dtClosePositions <= dt ) {
        m_pPosition->ClosePosition();
        m_stateTimeFrame = EClosing;
      }
      break;
    case EGoingNeutral:
      assert( false );
      break;
    case EClosing:
      if ( m_dtClosingBell <= dt ) {
        m_stateTimeFrame = EAfterHours;
      }
      break;
    case EAfterHours:
      break;

    }
  }
}

void RunSequence::Trade( void ) {

  // run the gp generated formula
  bool bLong = m_trade.Price() <= m_quote.Bid();
  bool bShort = true;

  // execute with result
  switch ( m_stateTrading ) {
  case ENeutral:
    if ( bLong && !bShort ) { // go long
      m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
      m_stateTrading = ELong;
    }
    else {
      if ( !bLong && bShort ) { // go short
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        m_stateTrading = EShort;
      }
    }
    break;
  case ELong:
    if ( bShort ) { // exit
      m_pPosition->CancelOrders();
      m_pPosition->ClosePosition();
      m_stateTrading = ENeutral;
      if ( !bLong ) { // go short
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        m_stateTrading = EShort;
      }
    }
    break;
  case EShort:
    if ( bLong ) { // exit
      m_pPosition->CancelOrders();
      m_pPosition->ClosePosition();
      m_stateTrading = ENeutral;
      if ( !bShort ) { // go long
        m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
        m_stateTrading = ELong;
      }
    }
    break;
  }
}



void RunSequence::HandlePortfolioExecution( const ou::tf::CPortfolio*  ) { 
  std::cout << "Porfolio Execution: " 
    << m_portfolio.GetRow().dblRealizedPL << "," 
    << m_portfolio.GetRow().dblCommissionsPaid 
    << std::endl; 
}
void RunSequence::HandlePortfolioCommission( const ou::tf::CPortfolio*  ) { 
  std::cout << "Porfolio Commission: "     
    << m_portfolio.GetRow().dblRealizedPL << "," 
    << m_portfolio.GetRow().dblCommissionsPaid 
    << std::endl; 
}

void RunSequence::HandlePositionExecution( const ou::tf::CPosition::execution_delegate_t delegate ) {
  std::cout << "  execute:  " << delegate.second.GetSize() << "@" << delegate.second.GetPrice() << std::endl;
}

void RunSequence::HandlePositionCommission( const ou::tf::CPosition* pPosition ) {
}
