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

RunSequence::RunSequence(void) 
  : m_portfolio( "test" )
{
}

RunSequence::~RunSequence(void) {
}

void RunSequence::Run( void ) {

  typedef ou::tf::CSimulationProvider::pProvider_t pProviderSim_t;
  pProviderSim_t m_pProvider;

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pInstrument = mgr.Exists( "TEST" ) ? mgr.Get( "TEST" ) : mgr.ConstructInstrument( "TEST", "SMART", ou::tf::InstrumentType::Stock );

  m_pProvider.reset( new ou::tf::CSimulationProvider );
  m_pProvider->SetGroupDirectory( "/app/TestSimulator" );

  m_pProvider->OnConnected.Add( MakeDelegate( this, &RunSequence::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Add( MakeDelegate( this, &RunSequence::HandleProviderDisconnected ) );
  m_pProvider->Connect();

  m_pProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleQuote ) );
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

  m_pProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &RunSequence::HandleQuote ) );
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
  ptime dt( trade.DateTime() );
  std::cout << dt << " trade " << std::endl;
  m_trades.Append( trade );
}

void RunSequence::HandleQuote( const ou::tf::Quote& quote ) {
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
