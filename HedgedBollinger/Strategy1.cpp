/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include <TFTrading/PortfolioManager.h>

#include "Strategy1.h"

// ideas
/*
* run puts on the high side, and calls on the low side
* bollinger bands need to be 2.5 x spread wide
* then on big moves, can close out pairs as profitable strangles

* count number of band hits
* enumeration of which side was last

* portfolio long
* portfolio short

* close positions when profitable
* track number of open positions

* need slope generator, so can tell if moving average is going up versuus going down
* then trade with the trend, starting on the band

* pick up three on each side then start pruning back when possible

* Books for reference:  
*  TradingTheMeasuredMove: fibo series useage (.pdf)
*  Trading With The Odds: some fibo stuff (paper)
*  ProfessionalAutomatedTrading: regime change (.pdf)
*/

Strategy::Strategy( 
  ou::tf::option::MultiExpiryBundle* meb, 
  pPortfolio_t pPortfolioLongs, pPortfolio_t pPortfolioShorts, 
  pProvider_t pExecutionProvider ) 
  : ou::ChartDVBasics(), m_pBundle( meb ), 
    m_pPortfolioLongs( pPortfolioLongs ), 
    m_pPortfolioShorts( pPortfolioShorts ), 
    m_pExecutionProvider( pExecutionProvider ),
    m_eBollinger1EmaSlope( eSlopeUnknown ),
    m_eTradingState( eTSUnknown ),
    m_bTrade( false ), m_nLongs( 0 ), m_nShorts( 0 ),
    m_nPositions( 0 ), m_eInd1( eInd1WaitForEntry )
{

  m_ceCountLongs.SetColour( ou::Colour::Red );
  m_ceCountShorts.SetColour( ou::Colour::Blue );
  m_cePL.SetColour( ou::Colour::Black );

  m_ceCountLongs.SetName( "Longs" );
  m_ceCountShorts.SetName( "Shorts" );
  m_cePL.SetName( "PL" );

  //std::stringstream ss;

  ptime dt( ou::TimeSource::Instance().External() );  // provided in utc
  boost::gregorian::date date( NormalizeDate( dt ) );
  InitForUS24HourFutures( date );
  // this may be offset incorrectly.
  //SetRegularHoursOpen( Normalize( dt.date(), dt.time_of_day(), "America/New_York" ) );  // collect some data first
  ptime dtMo( GetMarketOpen() );
  if ( dt > dtMo ) {
    SetRegularHoursOpen( dt );  // collect some data first
    // change later to 10 to collect enough data to start trading:
    //SetStartTrading( Normalize( dt.date(), dt.time_of_day() + boost::posix_time::minutes( 2 ), "America/New_York" ) );  // collect some data first
    SetStartTrading( dt + boost::posix_time::minutes( 2 ) );  // collect some data first
  }
  else {
    SetRegularHoursOpen( dtMo + boost::posix_time::minutes( 10 ) );  // collect some data first
    SetStartTrading( dtMo + boost::posix_time::minutes( 12 ) );  // collect some data first
  }

  for ( int ix = 0; ix <= 3; ++ix ) {
    m_vBollingerState.push_back( eBollingerUnknown );
  }

  if ( m_pExecutionProvider->Connected() ) { 
    m_bTrade = true;

    m_dvChart.Add( 8, &m_ceCountLongs );
    m_dvChart.Add( 8, &m_ceCountShorts );

    m_dvChart.Add( 9, &m_cePL );

  }

//  m_pBundle->Portfolio()
//    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
//      m_sNameOptionUnderlying, "aoRay", "USD", ou::tf::Portfolio::MultiLeggedPosition, ou::tf::Currency::Name[ ou::tf::Currency::USD ], m_sNameUnderlying + " Hedge" );

  m_pPositionLongs = 
      ou::tf::PortfolioManager::Instance().ConstructPosition( 
        m_pPortfolioLongs->Id(),
        "gclongs",
        "auto",
        "ib01",
        "iq01",
        m_pExecutionProvider,
        m_pBundle->GetWatchUnderlying()->GetProvider(),
        m_pBundle->GetWatchUnderlying()->GetInstrument()
      );
  m_pPositionLongs->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionLongs->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingLongs = new ou::tf::OrdersOutstandingLongs( m_pPositionLongs );

  m_pPositionShorts = 
      ou::tf::PortfolioManager::Instance().ConstructPosition( 
        m_pPortfolioShorts->Id(),
        "gcshorts",
        "auto",
        "ib01",
        "iq01",
        m_pExecutionProvider,
        m_pBundle->GetWatchUnderlying()->GetProvider(),
        m_pBundle->GetWatchUnderlying()->GetInstrument()
      );
  m_pPositionShorts->OnExecution.Add( MakeDelegate( this, &Strategy::HandleExecution ) );
  m_pPositionShorts->OnCommission.Add( MakeDelegate( this, &Strategy::HandleCommission ) );

  m_pOrdersOutstandingShorts = new ou::tf::OrdersOutstandingShorts( m_pPositionShorts );

  m_bThreadPopDatumsActive = true;
  m_pThreadPopDatums = new boost::thread( &Strategy::ThreadPopDatums, this );

//  m_pBundle->GetWatchUnderlying()->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnQuote.Add( MakeDelegate( this, &Strategy::HandleInboundQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Add( MakeDelegate( this, &Strategy::HandleInboundTradeUnderlying ) );

  m_pBundle->AddOnAtmIv( MakeDelegate( this, &Strategy::HandleCalcIv ) );

}

Strategy::~Strategy(void) {

  m_pBundle->RemoveOnAtmIv( MakeDelegate( this, &Strategy::HandleCalcIv ) );

  m_pBundle->GetWatchUnderlying()->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleInboundQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleInboundTradeUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );
  // after these are removed, may need to wait for the pushing thread to finish before we destruct Strategy.

  m_bThreadPopDatumsActive = false;
  m_cvCrossThreadDatums.notify_one();
  m_pThreadPopDatums->join();
  delete m_pThreadPopDatums;
  m_pThreadPopDatums = 0;

}

// two threads used:
//  * thread to update vectors and calculate the trade signals
//  * thread to use data from updated vectors and draw charts (a long lived process)
// biggest issue is that vectors can not be re-allocated during charting process.
// put vectors into a visitor process, which can be locked?  
// lock the trade-signals thread and the charting thread

void Strategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  // need to queue this from the originating thread.
  ou::ChartDVBasics::HandleTrade( trade );
  TimeTick( trade );
}

void Strategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  // need to queue this from the originating thread.
  if ( !quote.IsValid() ) {
    return;
  }
  ou::ChartDVBasics::HandleQuote( quote );
  TimeTick( quote );
}

// thread started in constructor
void Strategy::ThreadPopDatums( void ) {
  boost::unique_lock<boost::mutex> lock(m_mutexCrossThreadDatums);
  while ( m_bThreadPopDatumsActive ) {
    m_cvCrossThreadDatums.wait( lock );
    EDatumType type;
    while ( m_lfDatumType.pop( type ) ) {
      switch ( type ) {
      case EDatumQuote: {
        ou::tf::Quote quote;
        if ( m_lfQuote.pop( quote ) ) {
          HandleQuoteUnderlying( quote );
        }
                        }
        break;
      case EDatumTrade: {
        ou::tf::Trade trade;
        if ( m_lfTrade.pop( trade ) ) {
          HandleTradeUnderlying( trade );
        }
                        }
        break;
      }
    }
  }
}

void Strategy::HandleInboundQuoteUnderlying( const ou::tf::Quote& quote ) {
  bool bErrored( false );
  while ( !m_lfQuote.push( quote ) ) {
    if ( !bErrored ) {
      bErrored = true;
      std::cout << "m_lfQuote is full" << std::endl;
    }
  }
  while ( !m_lfDatumType.push( EDatumQuote ) );
  m_cvCrossThreadDatums.notify_one();
}

void Strategy::HandleInboundTradeUnderlying( const ou::tf::Trade& trade ) {
  bool bErrored( false );
  while ( !m_lfTrade.push( trade ) ) {
    if ( !bErrored ) {
      bErrored = true;
      std::cout << "m_lfTrade is full" << std::endl;
    }
  }
  while ( !m_lfDatumType.push( EDatumTrade ) );
  m_cvCrossThreadDatums.notify_one();
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {

  // add trades to chart
  // add based upon confirmed price

  if ( quote.IsValid() ) {

    m_dtQuote = quote.DateTime();

    double mid = quote.Midpoint();

    m_pOrdersOutstandingLongs->HandleQuote( quote );
    m_pOrdersOutstandingShorts->HandleQuote( quote );

    unsigned int cntLongs = m_pOrdersOutstandingLongs->GetCountOfOutstandingMatches();
    m_ceCountLongs.Append( m_dtQuote, cntLongs );
    unsigned int cntShorts = m_pOrdersOutstandingShorts->GetCountOfOutstandingMatches();
    m_ceCountShorts.Append( m_dtQuote, cntShorts );
    unsigned int dif = ( cntLongs > cntShorts ) ? cntLongs - cntShorts : cntShorts - cntLongs;

//    m_ceOutstandingExitsLong.Add( dt, cntLongs + m_pOrdersOutstandingLongs->GetCountOfOutstandingEntries() );
//    m_ceOutstandingExitsShort.Add( dt, cntShorts + m_pOrdersOutstandingShorts->GetCountOfOutstandingEntries() );

    infoBollinger& info( m_vInfoBollinger[ 0 ] );

    // since slope is delayed, check that data confirms slope before initiating actual trade
    // also, width of bollinger can limit when trades occur
    // track maximum, minimum, average width?

    ETradingState eTradingState( eTSUnknown );
    if ( 0.0 < info.m_statsSlope.Slope() ) {
      if ( mid > info.m_ema.Ago( 0 ).Value() ) {
        eTradingState = eTSSlopeRisingAboveMean;
      }
      else {
        if ( mid < info.m_ema.Ago( 0 ).Value() ) {
          eTradingState = eTSSlopeRisingBelowMean;
        }
      }
    }
    else {
      if ( 0.0 > info.m_statsSlope.Slope() ) {
        if ( mid < info.m_ema.Ago( 0 ).Value() ) {
          eTradingState = eTSSlopeFallingBelowMean;
        }
        else {
          if ( mid > info.m_ema.Ago( 0 ).Value() ) {
            eTradingState = eTSSlopeFallingAboveMean;
          }
        }
      }
    }

    if ( m_eTradingState != eTradingState ) {
      //std::cout << "Trading state " << eTradingState << std::endl;
      m_vTradeStateHistory.push_back( TradeStateHistory( eTradingState, quote ) );
      //m_eTradingState = eTradingState;

      size_t ix( m_vTradeStateHistory.size() );
      if ( 3 <= ix ) {
        TradeStateHistorySummary& 
          ths( m_mxTradeStateHistorySummary
                  [ m_vTradeStateHistory[ ix - 3 ].eTradingState ]
                  [ m_vTradeStateHistory[ ix - 2 ].eTradingState ]
                  [ m_vTradeStateHistory[ ix - 1 ].eTradingState ]
          );
        ou::tf::Quote& quote1( m_vTradeStateHistory[ ix - 2 ].quote );
        ou::tf::Quote& quote2( m_vTradeStateHistory[ ix - 1 ].quote );
        if ( quote2.Bid() > quote1.Ask() ) { // profitable long
          ++ths.nLongs;
          ths.dblTotalLongs += quote2.Bid() - quote1.Ask();
        }
        if ( quote1.Ask() > quote2.Bid() ) { // profitable short
          ++ths.nShorts;
          ths.dblTotalShorts += quote1.Ask() - quote2.Bid();
        }
        ths.dblSpread = quote.Spread();
        ths.dblBollingerWidth = info.m_stats.BBUpper() - info.m_stats.BBLower();
      }
    }

    // todo:
    // C:\Data\Resources\Books\Moving_Averages Kennedy
    // style 1: moving average compression - search for it, then use it set up option 
    // style 2: sma1 > sma2 > sma3 > sma4
    // 

    if ( m_bTrade ) {
      /*
      // scalping based upon acceleration crossing
      double dblNormalizedPrice;
      ou::tf::Order::pOrder_t pOrder;
      ou::tf::Instrument::pInstrument_t pInstrument;
      switch ( info.m_stateAccel.State() ) {
      case ou::tf::Crossing<double>::EGTX: 
        m_pOrdersOutstandingLongs->CancelAllButNEntryOrders( 2 );
        pInstrument = m_pPositionLongs->GetInstrument();
        dblNormalizedPrice = pInstrument->NormalizeOrderPrice( quote.Midpoint() );  // -0.10? +0.10?
        pOrder = m_pPositionLongs->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, dblNormalizedPrice );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
        pOrder->SetDescription( "Info[0].Long" );
        m_pOrdersOutstandingLongs->AddOrderFilling( 
          new ou::tf::OrdersOutstanding::structRoundTrip( 
            pOrder, 
            dblNormalizedPrice + info.m_dblBollingerWidth, // profit point, but let it accumulate with trailing stop
            dblNormalizedPrice - 0.5 * info.m_dblBollingerWidth )  // stop  ... between the two, trail while in progress, parabolic while not in progress
          );
        m_pPositionLongs->PlaceOrder( pOrder );
        //++m_nUpTransitions;
        break;
      case ou::tf::Crossing<double>::ELTX: 
        m_pOrdersOutstandingShorts->CancelAllButNEntryOrders( 2 );
        pInstrument = m_pPositionShorts->GetInstrument();
        dblNormalizedPrice = pInstrument->NormalizeOrderPrice( quote.Midpoint() );  // -0.10? +0.10?
        pOrder = m_pPositionShorts->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, dblNormalizedPrice );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
        pOrder->SetDescription( "Info[0].Short" );
        m_pOrdersOutstandingShorts->AddOrderFilling( 
          new ou::tf::OrdersOutstanding::structRoundTrip( 
            pOrder, 
            dblNormalizedPrice - info.m_dblBollingerWidth, // profit point, but let it accumulate with trailing stop
            dblNormalizedPrice + 0.5 * info.m_dblBollingerWidth )  // stop  ... between the two, trail while in progress, parabolic while not in progress
          );
        m_pPositionShorts->PlaceOrder( pOrder );
        //++m_nDnTransitions;
        break;
      }
      */

      // strong rising indicator
      bool bRising = 
           ( m_vInfoBollinger[0].m_statsSlope.MeanY() > m_vInfoBollinger[1].m_statsSlope.MeanY() ) // various slopes are greater than the next longer term
        && ( m_vInfoBollinger[1].m_statsSlope.MeanY() > m_vInfoBollinger[2].m_statsSlope.MeanY() )
        && ( m_vInfoBollinger[2].m_statsSlope.MeanY() > m_vInfoBollinger[3].m_statsSlope.MeanY() )
        && ( 0 < m_vInfoBollinger[3].m_statsSlope.MeanY() )
        ;
    
      bool bFalling = 
           ( m_vInfoBollinger[0].m_statsSlope.MeanY() < m_vInfoBollinger[1].m_statsSlope.MeanY() ) // various slopes are greater than the next longer term
        && ( m_vInfoBollinger[1].m_statsSlope.MeanY() < m_vInfoBollinger[2].m_statsSlope.MeanY() )
        && ( m_vInfoBollinger[2].m_statsSlope.MeanY() < m_vInfoBollinger[3].m_statsSlope.MeanY() )
        && ( 0 > m_vInfoBollinger[3].m_statsSlope.MeanY() )
        ;

      switch ( m_eInd1 ) {
      case eInd1WaitForEntry:
        if ( bRising ) {
          m_pOrderTrending = m_pPositionLongs->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
          m_pOrderTrending->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
          m_pOrderTrending->SetDescription( "Ind1 Long" );
          m_pPositionLongs->PlaceOrder( m_pOrderTrending );
          m_eInd1 = eInd1InRising;
        }
        else {
          if ( bFalling ) {
            m_pOrderTrending = m_pPositionShorts->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            m_pOrderTrending->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
            m_pOrderTrending->SetDescription( "Ind1 Short" );
            m_pPositionShorts->PlaceOrder( m_pOrderTrending );
            m_eInd1 = eInd1InFalling;
          }
        }
        break;
      case eInd1InRising:
        if ( !bRising ) {
          m_dblStop = ( mid - m_pOrderTrending->GetAverageFillPrice() ) / 2.0;
          m_eInd1 = eInd1FollowLongStop;
        }
        break;
      case eInd1InFalling:
        if ( !bFalling ) {
          m_dblStop = ( m_pOrderTrending->GetAverageFillPrice() - mid ) / 2.0;
          m_eInd1 = eINd1FollowShortStop;
        }
        break;
      case eInd1FollowLongStop:
        if ( bRising ) {
          m_eInd1 = eInd1InRising;
        }
        else {
          if ( mid < m_dblStop ) {
            m_pOrderTrending = m_pPositionLongs->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
            m_pOrderTrending->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
            m_pOrderTrending->SetDescription( "Ind1 Long Exit" );
            m_pPositionLongs->PlaceOrder( m_pOrderTrending );
            m_eInd1 = eInd1WaitForEntry;
          }
        }
        break;
      case eINd1FollowShortStop:
        if ( bFalling ) {
          m_eInd1 = eInd1InFalling;
        }
        else {
          if ( mid > m_dblStop ) {
            m_pOrderTrending = m_pPositionShorts->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
            m_pOrderTrending->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
            m_pOrderTrending->SetDescription( "Ind1 Short Exit" );
            m_pPositionShorts->PlaceOrder( m_pOrderTrending );
            m_eInd1 = eInd1InFalling;
          }
        }
        break;
      }

      double dblUnrealized;
      double dblRealized;
      double dblCommission;
      double dblTotal;
//      m_pPortfolio->QueryStats( dblUnrealized, dblRealized, dblCommission, dblTotal );
//      m_cePL.Append( m_dtQuote, dblTotal );

    }

    m_eTradingState = eTradingState;
/*
    if ( m_bTrade ) {
      switch ( m_eTradingState ) {
      case eTSUnknown:
        break;
      case eTSSlopeRisingAboveMean:
        switch ( eTradingState ) {
        case eTSSlopeRisingBelowMean:
          TakeLongProfits();
          GoShort();
          break;
        }
        break;
      case eTSSlopeRisingBelowMean:
        break;
      case eTSSlopeFallingAboveMean:
        break;
      case eTSSlopeFallingBelowMean:
        switch ( eTradingState ) {
        case eTSSlopeFallingAboveMean:
          TakeShortProfits();
          GoLong();
          break;
        }
        break;
      }
*/
/*
    switch ( m_eBollinger1EmaSlope ) {
    case eSlopeUnknown:
      if ( 0.0 < info.m_statsSlope.Slope() ) {
        std::cout << dt << "Starting with a long" << std::endl;
        GoLong();
        m_eBollinger1EmaSlope = eSlopePos;
      }
      else {
        if ( 0.0 > info.m_statsSlope.Slope() ) {
          std::cout << dt << "Starting with a short" << std::endl;
          GoShort();
          m_eBollinger1EmaSlope = eSlopeNeg;
        }
      }
      break;
    case eSlopeNeg:
      if ( 0.0 < info.m_statsSlope.Slope() ) {
        std::cout << dt << "Reversing Short to Long" << std::endl;
        m_pPosition->ClosePosition();
        GoLong();
        m_eBollinger1EmaSlope = eSlopePos;
      }
      break;
    case eSlopePos:
      if ( 0.0 > info.m_statsSlope.Slope() ) {
        std::cout << dt << "Reversing Long to Short" << std::endl;
        m_pPosition->ClosePosition();
        GoShort();
        m_eBollinger1EmaSlope = eSlopeNeg;
      }
      
      break;
    }
    */

  }
}

void Strategy::HandleCancel( void ) {
  static unsigned int n( 0 );
  if ( 0 == n ) {
    m_pOrdersOutstandingLongs->CancelAndCloseAllOrders();
    m_pOrdersOutstandingShorts->CancelAndCloseAllOrders();
    ++n;
  }
}

void Strategy::TakeLongProfits( void ) {
//  for ( vPosition_t::iterator iter = m_vPositionAll.begin(); m_vPositionAll.end() != iter; ++iter ) {
//    const ou::tf::Position::TableRowDef& row( iter->get()->Position()->GetRow() );
//    if ( ou::tf::OrderSide::Buy == row.eOrderSideActive ) {
//      iter->get()->ExitLong();
//    }
//  }
}

void Strategy::TakeShortProfits( void ) {
//  for ( vPosition_t::iterator iter = m_vPositionAll.begin(); m_vPositionAll.end() != iter; ++iter ) {
//    const ou::tf::Position::TableRowDef& row( iter->get()->Position()->GetRow() );
//    if ( ou::tf::OrderSide::Sell == row.eOrderSideActive ) {
//      iter->get()->ExitShort();
//    }
//  }
}

void Strategy::GoShort( void ) {
//  PositionState& ps( GetAPositionState() );
//  ps.OnPositionClosed = MakeDelegate( this, &Strategy::ReturnAPositionStateShort );
//  pPosition_t pPosition( ps.Position() );
//  pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  ++m_nShorts;
  m_ceCountShorts.Append( m_dtQuote, -m_nShorts );
}

void Strategy::GoLong( void ) {
//  PositionState& ps( GetAPositionState() );
//  ps.OnPositionClosed = MakeDelegate( this, &Strategy::ReturnAPositionStateLong );
//  pPosition_t pPosition( ps.Position() );
//  pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  ++m_nLongs;
  m_ceCountLongs.Append( m_dtQuote, m_nLongs );
}

void Strategy::EmitStats( void ) const {
  struct TradeStateHistoryEntry {
    ETradingState state1;
    ETradingState state2;
    ETradingState state3;
    TradeStateHistoryEntry( ETradingState state1_, ETradingState state2_, ETradingState state3_ )
      : state1( state1_ ), state2( state2_ ), state3( state3_ ) {};
  };
  typedef std::multimap<size_t,TradeStateHistoryEntry> mmEntry_t;
  mmEntry_t mmLongs;
  mmEntry_t mmShorts;
  for ( int ix = eTSSlopeRisingAboveMean; ix <= eTSSlopeFallingBelowMean; ++ix ) {
    for ( int iy = eTSSlopeRisingAboveMean; iy <= eTSSlopeFallingBelowMean; ++iy ) {
      for ( int iz = eTSSlopeRisingAboveMean; iz <= eTSSlopeFallingBelowMean; ++iz ) {
        const TradeStateHistorySummary& tshs( m_mxTradeStateHistorySummary[ix][iy][iz] );
        if ( 0 < tshs.nLongs ) {
          mmLongs.insert( mmEntry_t::value_type( tshs.nLongs, 
            TradeStateHistoryEntry( (ETradingState) ix, (ETradingState) iy, (ETradingState) iz ) ) );
        }
        if ( 0 < tshs.nShorts ) {
          mmShorts.insert( mmEntry_t::value_type( tshs.nShorts, 
            TradeStateHistoryEntry( (ETradingState) ix, (ETradingState) iy, (ETradingState) iz ) ) );
        }
      }
    }
  }
  std::cout << "Longs " << std::endl;
  for ( mmEntry_t::iterator iter = mmLongs.begin(); iter != mmLongs.end(); ++iter ) {
    std::cout << iter->first << "," << iter->second.state1 << "," << iter->second.state2 << "," << iter->second.state3 << std::endl;
  }
  std::cout << "Shorts " << std::endl;
  for ( mmEntry_t::iterator iter = mmShorts.begin(); iter != mmShorts.end(); ++iter ) {
    std::cout << iter->first << "," << iter->second.state1 << "," << iter->second.state2 << "," << iter->second.state3 << std::endl;
  }
}

void Strategy::HandleCommon( const ou::tf::Quote& quote ) {
}

void Strategy::HandleCommon( const ou::tf::Trade& trade ) {
}

void Strategy::HandleCalcIv( const ou::tf::PriceIV& iv ) {
  mapAtmIv_t::iterator iter = m_mapAtmIv.find( iv.Expiry() );
  if ( m_mapAtmIv.end() == iter ) {
    BundleAtmIv bai;
    switch ( m_mapAtmIv.size() ) {
    case 0: 
      bai.m_pceCallIV->SetColour( ou::Colour::RosyBrown );
      bai.m_pcePutIV->SetColour( ou::Colour::MediumOrchid );
      break;
    case 1:
      bai.m_pceCallIV->SetColour( ou::Colour::Cyan );
      bai.m_pcePutIV->SetColour( ou::Colour::PaleGreen );
      break;
    }
    std::stringstream ss;
    ss << iv.Expiry();
    bai.m_pceCallIV->SetName( ss.str() + " call" );
    bai.m_pcePutIV->SetName( ss.str() + " put" );
    m_mapAtmIv.insert( mapAtmIv_t::value_type( iv.Expiry(), bai ) );
//    m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pceCallIV.get() );
//    m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pcePutIV.get() );
    ou::ChartDVBasics::GetChartDataView().Add( 3, bai.m_pceCallIV.get() );
    ou::ChartDVBasics::GetChartDataView().Add( 3, bai.m_pcePutIV.get() );
  }
  else {
    iter->second.m_pceCallIV->Append( iv.DateTime(), iv.IVCall() );
    iter->second.m_pcePutIV->Append( iv.DateTime(), iv.IVPut() );
  }
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  switch ( order.GetOrderSide() ) {
  case ou::tf::OrderSide::Sell:
    m_ceShortFills.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "" );
    break;
  case ou::tf::OrderSide::Buy:
    m_ceLongFills.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "" );
    break;
  }
  const_cast<ou::tf::Order&>( order ).OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
}

void Strategy::HandleExecution( const PositionDelta_delegate_t& del ) {
//  m_ss << "Exec: " << del.second.GetTimeStamp() << ": ";
//  m_ss << *m_pPositionLongs;
//  m_ss << ", ";
//  m_ss << *m_pPositionShorts;
//  std::cout << m_ss << std::endl;
}

void Strategy::HandleCommission( const PositionDelta_delegate_t& del ) {
//  m_ss.str( "  " );
//  m_ss << *m_pPositionLongs;
//  m_ss << ", ";
//  m_ss << *m_pPositionShorts;
//  std::cout << m_ss << std::endl;
}

/*
PositionState& Strategy::GetAPositionState( void ) {
//  if ( 0 == m_vPositionStateEmpties.size() ) {
    std::string seq( boost::lexical_cast<std::string>( ++m_nPositions ) );
    while ( 3 > seq.length() ) seq = '0' + seq;
    pPosition_t pPosition(  
      ou::tf::PortfolioManager::Instance().ConstructPosition( 
        m_pPortfolio->Id(),
        "gc" + seq,
        "auto",
        "ib01",
        "iq01",
        m_pExecutionProvider,
        m_pBundle->GetWatchUnderlying()->GetProvider(),
        m_pBundle->GetWatchUnderlying()->GetInstrument()
        )
      );
    size_t ix( m_vPositionAll.size() );
    pPositionState_t p( new PositionState( ix, pPosition ) );
    p->OnPositionClosed = MakeDelegate( this, &Strategy::ReturnAPositionState );
    m_vPositionAll.push_back( p );
//    m_vPositionStateEmpties.push_back( m_vPositionAll.size() - 1 );
//  }

//  size_t ix( m_vPositionStateEmpties.back() );
//  m_vPositionStateEmpties.pop_back();

  return *(m_vPositionAll[ ix ].get());

}
*/
/*
void Strategy::ReturnAPositionState( const PositionState& ps ) {  // thread safe?
  size_t ix( ps.Index() );
//  for ( std::vector<size_t>::iterator iter = m_vPositionStateEmpties.begin(); m_vPositionStateEmpties.end() != iter; ++iter ) {
//    assert( ix != *iter );
//  }
//  m_vPositionStateEmpties.push_back( ix );
}

void Strategy::ReturnAPositionStateLong( const PositionState& ps ) {  // thread safe?
  --m_nLongs;
  m_ceCountLongs.Append( m_dtQuote, m_nLongs );
  ReturnAPositionState( ps );
}

void Strategy::ReturnAPositionStateShort( const PositionState& ps ) {  // thread safe?
  --m_nShorts;
  m_ceCountShorts.Append( m_dtQuote, -m_nShorts );
  ReturnAPositionState( ps );
}
*/