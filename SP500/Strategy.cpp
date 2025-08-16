/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: April 14, 2025 20:32:29
 */

#include <boost/log/trivial.hpp>

#include "Strategy.hpp"

namespace {
  static const ou::Colour::EColour c_colourEma200( ou::Colour::OrangeRed );
  static const ou::Colour::EColour c_colourEma50(  ou::Colour::DarkMagenta );
  static const ou::Colour::EColour c_colourEma29(  ou::Colour::DarkTurquoise );
  static const ou::Colour::EColour c_colourEma13(  ou::Colour::Purple );
  static const ou::Colour::EColour c_colourPrice(  ou::Colour::DarkGreen );
  static const ou::Colour::EColour c_colourAsk(    ou::Colour::Blue );
  static const ou::Colour::EColour c_colourBid(    ou::Colour::Red );
  static const ou::Colour::EColour c_colourTickJ(  ou::Colour::Chocolate );
  static const ou::Colour::EColour c_colourTickL(  ou::Colour::MediumPurple );
  static const ou::Colour::EColour c_colourTickRegime( ou::Colour::DarkOrchid );
  static const ou::Colour::EColour c_colourAdvDec( ou::Colour::Maroon );
  static const ou::Colour::EColour c_colourPrdct(  ou::Colour::Blue );

  static const double c_tickHi( +1.0 );
  static const double c_tickLo( -1.0 );
}

Strategy::Strategy(
  ou::ChartDataView& cdv
, fConstructWatch_t&& fConstructWatch
, fConstructPosition_t&& fConstructPosition
, fStart_t&& fStart
, fStop_t&& fStop
, fForward_t&& fForward
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_stateTrade( ETradeState::Init )
, m_fConstructWatch( std::move( fConstructWatch ) )
, m_fConstructPosition( std::move( fConstructPosition ) )
, m_fStart( std::move( fStart ) )
, m_fStop( std::move( fStop ) )
, m_fForward( std::move( fForward ) )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_bfQuotes01Sec(  1 )
, m_dblMid {}
, m_nEnterLong {}, m_nEnterShort {}
, m_dblTickRegime {}, m_bTickRegimeIncreased( false )
, m_TickRegime( ETickRegime::congestion )
, m_dblPrvPrice {}, m_dblPrvAdvDec {}
{
  SetupChart();

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

}

Strategy::~Strategy() {
  if ( m_pDec ) {
    m_pDec->StopWatch();
    m_pDec->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
  }
  if ( m_pAdv ) {
    m_pAdv->StopWatch();
    m_pAdv->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
  }
  if ( m_pTickJ ) {
    m_pTickJ->StopWatch();
    m_pTickJ->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
  }
  if ( m_pTickL ) {
    m_pTickL->StopWatch();
    m_pTickL->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
  }
  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->StopWatch();
    pWatch->OnQuote.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
  }
  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_cdv.Clear();
}

void Strategy::Start() {

  m_fConstructPosition(
    "SPY",
    [this]( pPosition_t pPosition ){
      m_pPosition = pPosition;
      pWatch_t pWatch = pPosition->GetWatch();
      pWatch->OnQuote.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
      pWatch->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
      pWatch->StartWatch();
      ValidateAndStart();
    } );

  m_fConstructWatch(
    "JT6T.Z",
    [this]( pWatch_t pWatch ){
      m_pTickJ = pWatch;
      m_pTickJ->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
      m_pTickJ->StartWatch();
      ValidateAndStart();
    } );

  m_fConstructWatch(
    "LI6N.Z",
    [this]( pWatch_t pWatch ){
      m_pTickL = pWatch;
      m_pTickL->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
      m_pTickL->StartWatch();
      ValidateAndStart();
    } );

  m_fConstructWatch(
    "II6A.Z", // advancers
    [this]( pWatch_t pWatch ){
      m_pAdv = pWatch;
      m_pAdv->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
      m_pAdv->StartWatch();
      ValidateAndStart();
    } );

  m_fConstructWatch(
    "II6D.Z", // decliners
    [this]( pWatch_t pWatch ){
      m_pDec = pWatch;
      m_pDec->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
      m_pDec->StartWatch();
      ValidateAndStart();
    } );

}

void Strategy::ValidateAndStart() {
  bool bOkToStart( true );
  bOkToStart &= nullptr != m_pPosition.get();
  bOkToStart &= nullptr != m_pTickJ.get();
  bOkToStart &= nullptr != m_pTickL.get();
  bOkToStart &= nullptr != m_pAdv.get();
  bOkToStart &= nullptr != m_pDec.get();
  if ( bOkToStart ) {
    m_fStart();
  }
}

void Strategy::SetupChart() {

  m_cemPosOne.AddMark( +1.0, ou::Colour::Black,   "+1" );
    m_cemZero.AddMark(  0.0, ou::Colour::Black, "zero" );
  m_cemNegOne.AddMark( -1.0, ou::Colour::Black,   "-1" );
  m_cemZeroPtOne.AddMark( 0.1, ou::Colour::Red,   "0.1" );

  m_ceTrade.SetName( "Trade" );
  m_ceTrade.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );

  //m_ceAsk.SetName( "Ask" );
  //m_ceAsk.SetColour( c_colourAsk );
  //m_cdv.Add( EChartSlot::Price, &m_ceAsk );

  //m_ceBid.SetName( "Bid" );
  //m_ceBid.SetColour( c_colourBid );
  //m_cdv.Add( EChartSlot::Price, &m_ceBid );

  m_ceEma013.SetName( "13s ema" );
  m_ceEma013.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma013 );

  m_ceEma029.SetName( "29s ema" );
  m_ceEma029.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma029 );

  m_ceEma050.SetName( "50s ema" );
  m_ceEma050.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma050 );

  m_ceEma200.SetName( "200s ema" );
  m_ceEma200.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma200 );

  m_cdv.Add( EChartSlot::Ratio, &m_cemZero );

  m_ceTrade_ratio.SetName( "Trade" );
  m_ceTrade_ratio.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTrade_ratio );

  m_ceEma013_ratio.SetName( "13s ema" );
  m_ceEma013_ratio.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma013_ratio );

  m_ceEma029_ratio.SetName( "29s ema" );
  m_ceEma029_ratio.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma029_ratio );

  m_ceEma050_ratio.SetName( "50s ema" );
  m_ceEma050_ratio.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma050_ratio );

  m_ceEma200_ratio.SetName( "200s ema" );
  m_ceEma200_ratio.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma200_ratio );

  m_ceTickJ_sigmoid.SetName( "TickJ" );
  m_ceTickJ_sigmoid.SetColour( c_colourTickJ );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTickJ_sigmoid );

  m_ceTickL_sigmoid.SetName( "TickL" );
  m_ceTickL_sigmoid.SetColour( c_colourTickL );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTickL_sigmoid );

  m_ceAdvDec_ratio.SetName( "AdvDec" );
  m_ceAdvDec_ratio.SetColour( c_colourAdvDec );
  m_cdv.Add( EChartSlot::Ratio, &m_ceAdvDec_ratio );

  m_ceVolume.SetName( "Volume" );
  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Tick, &m_cemZero );

  m_ceTickJ.SetName( "TickJ" );
  m_ceTickJ.SetColour( c_colourTickJ );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickJ );

  m_ceTickL.SetName( "TickL" );
  m_ceTickL.SetColour( c_colourTickL );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickL );

  m_ceAdvDec.SetName( "AdvDec" );
  m_ceAdvDec.SetColour( c_colourAdvDec );
  m_cdv.Add( EChartSlot::AdvDec, &m_cemZero );
  m_cdv.Add( EChartSlot::AdvDec, &m_ceAdvDec );

  m_ceRtnPrice.SetName( "Price Returns" );
  m_cdv.Add( EChartSlot::rtnPrice, &m_ceRtnPrice );

  m_cdv.Add( EChartSlot::TickRegime, &m_cemZeroPtOne );
  m_cdv.Add( EChartSlot::TickRegime, &m_cemZero );

  m_ceTickRegime.SetName( "Tick Regime" );
  m_ceTickRegime.SetColour( c_colourTickRegime );
  m_cdv.Add( EChartSlot::TickRegime, &m_ceTickRegime );

  //m_ceRtnAdvDec.SetName( "AdvDec Returns" );
  //m_cdv.Add( EChartSlot::rtnAdvDec, &m_ceRtnAdvDec );

  //m_cePrediction_scaled.SetName( "Predict" );
  //m_cePrediction_scaled.SetColour( c_colourPrdct );
  //m_cdv.Add( EChartSlot::Predict, &m_ceTrade_ratio );
  //m_cdv.Add( EChartSlot::Predict, &m_cePrediction_scaled );

  //m_cePrediction_descaled.SetName( "Predict" );
  //m_cePrediction_descaled.SetColour( c_colourPrdct );
  //m_cdv.Add( EChartSlot::Price, &m_cePrediction_descaled );

  m_ceProfitLoss.SetName( "P/L" );
  //m_cdv.Add( EChartSlot::PL, &m_cemZero );
  //m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  //m_cdv.Add( EChartSlot::PredVec, &m_cePrediction_vector );
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  m_quote = quote;
  const auto dt( quote.DateTime() );
  //m_ceAsk.Append( dt, quote.Ask() );
  //m_ceBid.Append( dt, quote.Bid() );
  m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {
  m_trade = trade;
  const ou::tf::Price::dt_t         dt( trade.DateTime() );
  const ou::tf::Price::price_t   price( trade.Price() );
  const ou::tf::Price::volume_t volume( trade.Volume() );
  m_vwp.Add( price, volume );
  m_ceTrade.Append(  dt, price );
  m_ceVolume.Append( dt, volume );

  if ( ( 0.0 == m_dblPrvPrice ) ) {}
  else {
    const double rtn = std::log( price / m_dblPrvPrice ); // natural log, ie ln
    m_ceRtnPrice.Append( dt, rtn );
  }
  m_dblPrvPrice = price;

  TimeTick( trade );
}

void Strategy::HandleTickJ( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const ou::tf::Price::dt_t       dt( tick.DateTime() );
    const ou::tf::Price::price_t price( tick.Price() );

    m_features.dblTickJ = price / 100.0;  // approx normalization
    m_ceTickJ.Append( dt, m_features.dblTickJ );

    m_ceTickRegime.Append( dt, CalcTickRegime() );
  }
}

void Strategy::HandleTickL( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const ou::tf::Price::dt_t       dt( tick.DateTime() );
    const ou::tf::Price::price_t price( tick.Price() );

    m_features.dblTickL = price / 200.0;  // approx normalization
    m_ceTickL.Append( dt, m_features.dblTickL );

    m_ceTickRegime.Append( dt, CalcTickRegime() );
  }
}

double Strategy::CalcTickRegime() {
  const double dblTickRegime = m_features.dblTickJ * m_features.dblTickL;
  m_bTickRegimeIncreased = m_dblTickRegime < dblTickRegime;
  if (  0.1 >= dblTickRegime ) {
    m_TickRegime = ETickRegime::congestion;
  }
  else {
    if ( 0.0 <= m_features.dblTickJ ) {
      if ( 0.0 <= m_features.dblTickL ) {
        m_TickRegime = ETickRegime::advance;
      }
      else { // 0.0 > m_features.dblTickL
        m_TickRegime = ETickRegime::diverge;
      }
    }
    else { // 0.0 > m_features.dblTickJ
      if ( 0.0 <= m_features.dblTickL ) {
        m_TickRegime = ETickRegime::diverge;
      }
      else { // 0.0 > m_features.dblTickL
        m_TickRegime = ETickRegime::decline;
      }
    }
  }
  m_dblTickRegime = dblTickRegime;
  return dblTickRegime;
}

void Strategy::HandleAdv( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    m_features.dblAdv = tick.Price();
    CalcAdvDec( tick.DateTime() );
  }
}

void Strategy::HandleDec( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    m_features.dblDec = tick.Price();
    CalcAdvDec( tick.DateTime() );
  }
}

void Strategy::CalcAdvDec( boost::posix_time::ptime dt ) {
  const double sum( m_features.dblAdv + m_features.dblDec );
  const double diff( m_features.dblAdv - m_features.dblDec );
  const double ratio( diff / sum );
  m_features.dblAdvDecRatio = ratio;

  //if ( ( 0.0 == m_dblPrvAdvDec ) ) {}
  //else {
  //  const double rtn = std::log( ratio / m_dblPrvAdvDec ); // natural log, ie ln
  //  m_ceRtnAdvDec.Append( dt, rtn );
  //}
  //m_dblPrvAdvDec = ratio;

  m_ceAdvDec.Append( dt, ratio );
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  if ( RHTrading() ) {
    Calc01SecIndicators( bar );
  }
  TimeTick( bar );
}

void Strategy::HandleRHTrading( const ou::tf::Trade& trade ) {
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  switch ( m_stateTrade ) {
    case ETradeState::Search:
      switch ( m_TickRegime ) {
        case ETickRegime::advance:
          if ( m_bTickRegimeIncreased ) {
            //BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
            m_ceLongEntry.AddLabel( dt, price, "long" );
            ++m_nEnterLong;
            m_stateTrade = ETradeState::LongSubmitted;
          }
          break;
        case ETickRegime::decline:
          if ( m_bTickRegimeIncreased ) {
            //BOOST_LOG_TRIVIAL(trace) << "ETickHi::DnOvr enter";
            m_ceShortEntry.AddLabel( dt, price, "short" );
            ++m_nEnterShort;
            m_stateTrade = ETradeState::ShortSubmitted;
          }
          break;
        case ETickRegime::congestion:
          break;
        case ETickRegime::diverge:
          break;
      }
      break;
    case ETradeState::LongSubmitted:
      if ( !m_bTickRegimeIncreased ) {
        //BOOST_LOG_TRIVIAL(trace) << "ETickLo::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::ShortSubmitted:
      if ( !m_bTickRegimeIncreased ) {
        //BOOST_LOG_TRIVIAL(trace) << "ETickHi::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::LongExit:
      break;
    case ETradeState::ShortExit:
      break;
    case ETradeState::LongExitSubmitted:
      break;
    case ETradeState::ShortExitSubmitted:
      break;
    case ETradeState::Neutral:
      BOOST_LOG_TRIVIAL(trace) << "neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayCancel:
      BOOST_LOG_TRIVIAL(trace) << "eod cancel," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(trace) << "eod neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::Done:
      break;
    case ETradeState::Init:
      m_stateTrade = ETradeState::Search;
      break;
  }
}

void Strategy::Calc01SecIndicators( const ou::tf::Bar& bar ) {

  const boost::posix_time::ptime dt( bar.DateTime() );
  m_features.dt = dt;

  const double vwp( m_vwp() );
  const double price( 0.0 == vwp ? bar.Close() : vwp );
  const ou::tf::Price price_( bar.DateTime(), price );

  m_features.dblPrice = price;

  UpdateEma< 13>( price_, m_features.dblEma013, m_ceEma013  );
  UpdateEma< 29>( price_, m_features.dblEma029, m_ceEma029  );
  UpdateEma< 50>( price_, m_features.dblEma050, m_ceEma050  );
  UpdateEma<200>( price_, m_features.dblEma200, m_ceEma200 );

  Features_scaled scaled; // receives scaled data
  m_fForward( m_features, scaled ); // may call PredictionVector for a live strategy
  const boost::posix_time::ptime dtPrediction( dt + boost::posix_time::time_duration( 0, 0, scaled.distance ) );
  //m_cePrediction_scaled.Append( dtPrediction, scaled.predicted.dbl );
  //m_cePrediction_descaled.Append( dtPrediction, scaled.predicted.dbl * scaled.range + scaled.min );

  //BOOST_LOG_TRIVIAL(trace) << "Calc01SecIndicators " << dt << ',' << prediction.DateTime();

  m_ceTrade_ratio.Append( dt, scaled.price.dbl );

  m_ceEma200_ratio.Append( dt, scaled.ema200.dbl );
  m_ceEma050_ratio.Append( dt, scaled.ema050.dbl );
  m_ceEma029_ratio.Append( dt, scaled.ema029.dbl );
  m_ceEma013_ratio.Append( dt, scaled.ema013.dbl );

  m_ceTickJ_sigmoid.Append( dt, scaled.tickJ.dbl );
  m_ceTickL_sigmoid.Append( dt, scaled.tickL.dbl );

  m_ceAdvDec_ratio.Append( dt, scaled.AdvDec.dbl );

}

void Strategy::PredictionVector( const size_t distance, const size_t size, const float* r  ) {
  m_cePrediction_vector.Clear(); // will this clear prior to appending?
  static const boost::posix_time::time_duration one_sec( 0, 0, 1 );
  boost::posix_time::ptime dt( m_features.dt - boost::posix_time::time_duration( 0, 0, size - distance + 60 ) ); // 60 provides offset to see whole prediction
  size_t counter( size );
  while ( 0 < counter ) {
    m_cePrediction_vector.Append( dt, *r );
    ++r;
    dt += one_sec;
    --counter;
  }
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  //m_ceProfitLoss.Append( bar.DateTime(), dblTotal );
}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(warning) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      break;
    default:
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
  }
  m_pOrder.reset();
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::LongSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Fill" );
      m_stateTrade = ETradeState::LongExit;
      break;
    case ETradeState::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Fill" );
      m_stateTrade = ETradeState::ShortExit;
      break;
    case ETradeState::LongExitSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Exit Fill" );
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      // figure out what labels to apply
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrder.reset();
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleCancel," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleGoNeutral," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayNeutral;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

