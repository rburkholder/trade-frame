/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

//#include <TFHDF5TimeSeries/HDF5DataManager.h>
//#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFTrading/InstrumentManager.h>

#include "Strategy.h"

Strategy::Strategy(void) 
  : m_sim( new ou::tf::CSimulationProvider() ),
  m_sma1min( &m_quotes, 60 ), m_sma2min( &m_quotes, 120 ), m_sma3min( &m_quotes, 180 ),
  m_sma5min( &m_quotes, 300 ), m_sma15min( &m_quotes, 1800 ),
  m_stateTrade( ETradeOut ), m_dtEnd( date( 2011, 9, 23 ), time_duration( 17, 58, 0 ) ),
  m_nTransitions( 0 )
{
  ou::tf::CProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

  m_sim->OnConnected.Add( MakeDelegate( this, &Strategy::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleSimulatorDisConnected ) );

  m_pExecutionProvider = m_sim;
  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &Strategy::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleOnExecDisconnected ) );

  m_pDataProvider = m_sim;
  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &Strategy::HandleOnData1Connected ) );
  m_pDataProvider->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleOnData1Disconnected ) );

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pTestInstrument = mgr.Exists( "+GCZ11" ) ? mgr.Get( "+GCZ11" ) : mgr.ConstructFuture( "+GCZ11", "SMART", 2011, 12 );
  m_pTestInstrument->SetMultiplier( 100 );

  m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497" );
  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );
  
  m_sim->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );
  m_sim->SetOnSimulationComplete( MakeDelegate( this, &Strategy::HandleSimulationComplete ) );

  pPosition.reset( new ou::tf::CPosition( m_pTestInstrument, m_sim, m_sim ) );
  pPosition->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  pPosition->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );
}

Strategy::~Strategy(void) {

  m_sim->SetOnSimulationComplete( 0 );
  m_sim->RemoveQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->RemoveTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );

  m_pDataProvider->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleOnData1Connected ) );
  m_pDataProvider->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleOnData1Disconnected ) );

  m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleOnExecDisconnected ) );

  m_sim->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleSimulatorDisConnected ) );

  ou::tf::CProviderManager::Instance().Release( "sim01" );

}

void Strategy::HandleSimulatorConnected( int ) {
  m_sim->Run();
}

void Strategy::HandleSimulatorDisConnected( int ) {
}

void Strategy::HandleOnExecConnected( int ) {
}

void Strategy::HandleOnExecDisconnected( int ) {
}

void Strategy::HandleOnData1Connected( int ) {
}

void Strategy::HandleOnData1Disconnected( int ) {
}

void Strategy::Start( const std::string& sSymbolPath ) {
  m_sim->Connect();
}

void Strategy::HandleQuote( const ou::tf::CQuote& quote ) {

//  m_ss.str( "" );
//  m_ss << quote.DateTime();

  m_quotes.Append( quote );
  m_sma15min.Update();

  if ( 1000 < m_quotes.Size() ) {

    switch ( m_stateTrade ) {
    case ETradeOut:
      if ( 0 != m_sma15min.Slope() ) {
        if ( 0 < m_sma15min.Slope() ) { //rising
          m_ss.str( "" );
          m_ss << "Ordered: " << quote.DateTime() << "; ";
          pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
          m_stateTrade = ETradeLong;
        }
        else { // falling
          m_ss.str( "" );
          m_ss << "Ordered: " << quote.DateTime() << "; ";
          pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
          m_stateTrade = ETradeShort;
        }
      }
      break;
    case ETradeLong:
      if ( pPosition->OrdersPending() ) {
      }
      else {
        if ( quote.DateTime() > m_dtEnd ) {
          pPosition->ClosePosition();
          m_stateTrade = ETradeDone;
        }
        else {
          if ( 0 > m_sma15min.Slope() ) {
            //pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            ++m_nTransitions;
            m_ss.str( "" );
            m_ss << "Ordered: " << quote.DateTime() << "; ";
            pPosition->ClosePosition();
            pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            m_stateTrade = ETradeShort;
          }
        }
      }
      break;
    case ETradeShort:
      if ( pPosition->OrdersPending() ) {
      }
      else {
        if ( quote.DateTime() > m_dtEnd ) {
          pPosition->ClosePosition();
          m_stateTrade = ETradeDone;
        }
        else {
          if ( 0 < m_sma15min.Slope() ) {
            //pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
            ++m_nTransitions;
            m_ss.str( "" );
            m_ss << "Ordered: " << quote.DateTime() << "; ";
            pPosition->ClosePosition();
            pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
            m_stateTrade = ETradeLong;
          }
        }
      }
      break;
    case ETradeDone:
      break;
    }
  }
}

void Strategy::HandleTrade( const ou::tf::CTrade& trade ) {
  m_trades.Append( trade );
}

void Strategy::HandleSimulationComplete( void ) {
  m_ss.str( "" );
  m_ss << m_nTransitions << " changes, ";
  pPosition->EmitStatus( m_ss );
  m_ss << ". ";
  m_sim->EmitStats( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleExecution( ou::tf::CPosition::execution_delegate_t del ) {
  m_ss << "Exec: " << del.second.GetTimeStamp() << ": ";
  pPosition->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleCommission( const ou::tf::CPosition* pPosition ) {
  m_ss.str( "" );
  pPosition->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}