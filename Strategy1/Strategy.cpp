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
  m_sma1( &m_quotes, 60 ), // 1 min
  m_sma2( &m_quotes, 120 ),  // 2 min
  m_sma3( &m_quotes, 180 ),  // 3 min
  m_sma4( &m_quotes, 300 ),  // 5 min
  m_sma5( &m_quotes, 600 ),  // 10 min
  m_sma6( &m_quotes, 1800 ), // 30 min
  m_sma7( &m_quotes, 3600 ), // 60 min
//  m_stateTrade( ETradeOut ), m_dtEnd( date( 2011, 9, 23 ), time_duration( 17, 58, 0 ) ),
  m_stateTrade( ETradeOut ), m_dtEnd( date( 2011, 11, 9 ), time_duration( 17, 45, 0 ) ),  // put in time start
  m_nTransitions( 0 ),
  m_barFactory( 180 ),
  m_dvChart( "Strategy1", "GC" ), 
  m_ceShorts( ou::ChartEntryShape::ESell, ou::Colour::Orange ),
  m_ceLongs( ou::ChartEntryShape::EBuy, ou::Colour::Blue ),
  m_tsswSlopeOfSlopeOfSMA( &m_tradesSlopeOfSlopeOfSMA, 90 ), 
  m_tsswSpreads( &m_spreads, 120 )
{

  ou::tf::CProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

  m_dvChart.Add( 0, m_ceShorts );
  m_dvChart.Add( 0, m_ceLongs );
  m_dvChart.Add( 0, m_ceBars );
  m_dvChart.Add( 0, m_ceSMA1 );
  m_dvChart.Add( 0, m_ceSMA2 );
  m_dvChart.Add( 0, m_ceUpperBollinger1 );
  m_dvChart.Add( 0, m_ceLowerBollinger1 );
  m_dvChart.Add( 0, m_ceUpperBollinger2 );
  m_dvChart.Add( 0, m_ceLowerBollinger2 );
  m_dvChart.Add( 1, m_ceVolume );
  m_dvChart.Add( 2, m_ceSlopeOfSMA1 );
  m_dvChart.Add( 2, m_ceSlopeOfSMA2 );
  m_dvChart.Add( 2, m_ceSlopeOfSlopeOfSMA );
  m_dvChart.Add( 3, m_ceBollinger1Width );
  m_dvChart.Add( 3, m_ceBollinger2Width );
  m_dvChart.Add( 4, m_ceOutstandingLong );
  m_dvChart.Add( 4, m_ceOutstandingShort );
//  m_dvChart.Add( 3, m_ceRR );
  m_dvChart.Add( 5, m_cePLLong );
  m_dvChart.Add( 5, m_cePLShort );
  m_dvChart.Add( 5, m_cePLNet );
//  m_dvChart.Add( 5, m_ceSpread );

  m_ceSMA1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceSMA2.SetColour( ou::Colour::Turquoise );
  m_cePLLong.SetColour( ou::Colour::Blue );
  m_cePLShort.SetColour( ou::Colour::Orange );
  m_cePLNet.SetColour( ou::Colour::Green );
  m_ceSlopeOfSMA1.SetColour( ou::Colour::DeepSkyBlue );
  m_ceSlopeOfSMA2.SetColour( ou::Colour::Turquoise );
  m_ceSlopeOfSlopeOfSMA.SetColour( ou::Colour::ForestGreen );
  m_ceOutstandingLong.SetColour( ou::Colour::Blue );
  m_ceOutstandingShort.SetColour( ou::Colour::Red );

  m_ceUpperBollinger1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceLowerBollinger1.SetColour( ou::Colour::DarkOliveGreen );
  m_ceUpperBollinger2.SetColour( ou::Colour::Turquoise );
  m_ceLowerBollinger2.SetColour( ou::Colour::Turquoise );

  m_ceBollinger1Width.SetColour( ou::Colour::DarkOliveGreen );
  m_ceBollinger2Width.SetColour( ou::Colour::Turquoise );

  m_barFactory.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarCompletion ) );

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

//  m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-06 18:54:22.184889" );
  //m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-07 18:53:31.016760" );
  m_sim->SetGroupDirectory( "/app/semiauto/2011-Nov-08 18:58:29.396624" );
  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );
  
  m_sim->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );
  m_sim->SetOnSimulationComplete( MakeDelegate( this, &Strategy::HandleSimulationComplete ) );

  m_pPositionLong.reset( new ou::tf::CPosition( m_pTestInstrument, m_sim, m_sim ) );
  m_pPositionLong->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionLong->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingLongs = new OrdersOutstandingLongs( m_pPositionLong );

  m_pPositionShort.reset( new ou::tf::CPosition( m_pTestInstrument, m_sim, m_sim ) );
  m_pPositionShort->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionShort->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingShorts = new OrdersOutstandingShorts( m_pPositionShort );
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

  m_barFactory.SetOnBarComplete( 0 );

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

  if ( ( 0 == quote.Ask() ) || ( 0 == quote.Bid() ) || ( 0 == quote.AskSize() ) || ( 0 == quote.BidSize() ) ) {
    return;
  }
  // should also check that a price within 2 - 3 sigma of last

  // problems occur when long trend happens and can't get out of opposing position.

  ptime dt( quote.DateTime() );

  m_quotes.Append( quote );
  ou::tf::TSSWStatsMidQuote& sma( m_sma5 );
  sma.Update();
  m_sma7.Update();

  double spread = quote.Ask() - quote.Bid();
  m_spreads.Append( ou::tf::CPrice( quote.DateTime(), spread ) );
  m_tsswSpreads.Update();

  m_tradesSlopeOfSlopeOfSMA.Append( ou::tf::CPrice( quote.DateTime(), sma.Slope() ) );
  m_tsswSlopeOfSlopeOfSMA.Update();

  if ( 500 < m_quotes.Size() ) {

    m_pOrdersOutstandingLongs->HandleQuote( quote );
    m_pOrdersOutstandingShorts->HandleQuote( quote );

    m_ceSpread.Add( dt, m_tsswSpreads.MeanY() );

    unsigned int cntLongs = m_pOrdersOutstandingLongs->GetCountOfOutstandingMatches();
    m_ceOutstandingLong.Add( dt, cntLongs );
    unsigned int cntShorts = m_pOrdersOutstandingShorts->GetCountOfOutstandingMatches();
    m_ceOutstandingShort.Add( dt, cntShorts );
    unsigned int dif = ( cntLongs > cntShorts ) ? cntLongs - cntShorts : cntShorts - cntLongs;

    m_ceSMA1.Add( dt, sma.MeanY() );
    m_ceSlopeOfSMA1.Add( dt, sma.Slope() );
    double direction = m_tsswSlopeOfSlopeOfSMA.Slope();
    if ( ( 0.00005 < direction ) || ( -0.00005 > direction ) ) {
      direction = 0.0;
    }
    else {
      m_ceSlopeOfSlopeOfSMA.Add( dt, direction * 200.0 );
    }

    m_ceUpperBollinger1.Add( dt, sma.BBUpper() );
    m_ceLowerBollinger1.Add( dt, sma.BBLower() );
    m_ceBollinger1Width.Add( dt, sma.BBUpper() - sma.BBLower() );

    m_ceSMA2.Add( dt, m_sma7.MeanY() );
    m_ceSlopeOfSMA2.Add( dt, m_sma7.Slope() * 10.0 );
    m_ceUpperBollinger2.Add( dt, m_sma7.BBUpper() );
    m_ceLowerBollinger2.Add( dt, m_sma7.BBLower() );
    m_ceBollinger2Width.Add( dt, m_sma7.BBUpper() - m_sma7.BBLower() );

    //m_ceRR.Add( quote.DateTime(), m_sma5min.RR() );
    double dblPLLong = m_pPositionLong->GetRealizedPL() + m_pPositionLong->GetUnRealizedPL() - m_pPositionLong->GetCommissionPaid();
    double dblPLShort = m_pPositionShort->GetRealizedPL() + m_pPositionShort->GetUnRealizedPL() - m_pPositionShort->GetCommissionPaid();
    m_cePLLong.Add( dt, dblPLLong );
    m_cePLShort.Add( dt, dblPLShort );
    m_cePLNet.Add( dt, dblPLLong + dblPLShort );

    switch ( m_stateTrade ) {
    case ETradeOut:
      //if ( 0 != sma.Slope() ) {
      if ( 0 != direction ) {
        if ( 0.00001 < direction ) { //rising
          m_ss.str( "" );
          m_ss << "Ordered: " << quote.DateTime() << "; ";
          m_pOrder = m_pPositionLong->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
          m_ceLongs.AddLabel( quote.DateTime(), quote.Ask(), "Long a" );
          m_stateTrade = ETradeLong;
        }
        if ( -0.00001 > direction ) {
        //else { // falling
          m_ss.str( "" );
          m_ss << "Ordered: " << quote.DateTime() << "; ";
          m_pOrder = m_pPositionShort->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
          m_ceShorts.AddLabel( quote.DateTime(), quote.Bid(), "Short a" );
          m_stateTrade = ETradeShort;
        }
      }
      break;
    case ETradeLong:
      //if ( m_pPositionLong->OrdersPending() ) {
      if ( false ) {
      }
      else {
        if ( dt > m_dtEnd ) {
          m_pOrder.reset();
          m_pOrdersOutstandingLongs->CancelAll();
          m_pOrdersOutstandingShorts->CancelAll();
          m_pPositionLong->ClosePosition();
          m_pPositionShort->ClosePosition();
          m_stateTrade = ETradeDone;
        }
        else {
          //if ( 0 > sma.Slope() ) {
          if ( -0.00001 > direction ) {
            //pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            ++m_nTransitions;
            //m_ss.str( "" );
            //m_ss << "Ordered: " << quote.DateTime() << "; ";
            //m_pPositionLongOpen->ClosePosition();
            m_pOrdersOutstandingLongs->AddOrderFilling( m_pOrder );
            m_pOrder.reset();
            m_pOrder = m_pPositionShort->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            m_ceShorts.AddLabel( quote.DateTime(), quote.Bid(), "Short b" );
            m_stateTrade = ETradeShort;
          }
        }
      }
      break;
    case ETradeShort:
      //if ( m_pPositionShort->OrdersPending() ) {
      if ( false ) {
      }
      else {
        if ( dt > m_dtEnd ) {
          m_pOrder.reset();
          m_pOrdersOutstandingLongs->CancelAll();
          m_pOrdersOutstandingShorts->CancelAll();
          m_pPositionLong->ClosePosition();
          m_pPositionShort->ClosePosition();
          m_stateTrade = ETradeDone;
        }
        else {
          //if ( 0 < sma.Slope() ) {
          if ( 0.00001 < direction ) {
            //pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
            ++m_nTransitions;
            //m_ss.str( "" );
            //m_ss << "Ordered: " << quote.DateTime() << "; ";
            //m_pPositionShortOpen->ClosePosition();
            m_pOrdersOutstandingShorts->AddOrderFilling( m_pOrder );
            m_pOrder.reset();
            m_pOrder = m_pPositionLong->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
            m_ceLongs.AddLabel( quote.DateTime(), quote.Ask(), "Long b" );
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
  m_barFactory.Add( trade );
}

void Strategy::HandleSimulationComplete( void ) {
  m_ss.str( "" );
  m_ss << m_nTransitions << " changes, ";
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  m_ss << ". ";
  m_sim->EmitStats( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleExecution( ou::tf::CPosition::execution_delegate_t del ) {
  m_ss << "Exec: " << del.second.GetTimeStamp() << ": ";
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleCommission( const ou::tf::CPosition* pPosition ) {
  m_ss.str( "" );
  m_pPositionLong->EmitStatus( m_ss );
  m_ss << ", ";
  m_pPositionShort->EmitStatus( m_ss );
  std::cout << m_ss << std::endl;
}

void Strategy::HandleBarCompletion( const ou::tf::CBar& bar ) {
  m_ceBars.AddBar( bar );
  m_ceVolume.Add( bar.DateTime(), bar.Volume() );
}

