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

//#include <cmath>

#include <boost/log/trivial.hpp>

// this define is required to prevent interference with libtorch
//   but may have been fixed now with https://github.com/pytorch/kineto/pull/1106/commits
#define FMT_HEADER_ONLY 1
#include <fmt/core.h>

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

  static const double c_regimeMinimum( 0.05 );

  static const size_t c_window( 60 );

  static const double c_ImbalanceMarker( 0.7 );

  static const double c_ReturnsMeanMarker( 0.045 );
  static const double c_ReturnsSlopeMarker( 0.0000045 );

}

Strategy::Strategy(
  ou::ChartDataView& cdv
, const Flags& flags
, fConstructWatch_t&& fConstructWatch
, fConstructPosition_t&& fConstructPosition
, fStart_t&& fStart
, fStop_t&& fStop
, fForward_t&& fForward
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_flags( flags )
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
, m_dblPrvSD {}
, m_statsReturns( m_returns, boost::posix_time::time_duration( 0, 0, c_window ) )
, m_minmaxPrices( m_prices,  boost::posix_time::time_duration( 0, 0, c_window ) )
, m_statsPrices(  m_prices,  boost::posix_time::time_duration( 0, 0, c_window ) )
//, m_atr {}
, m_stopInitial {}, m_stopDelta {}, m_stopTrail {}
, m_dblQuoteImbalance {}
, m_ixcurCrossing( 0 ), m_ixprvCrossing( 1 )
//, m_dblRtnPrice_slope_ema {}
{
  SetupChart();

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

}

Strategy::~Strategy() {
  m_cdv.SetNotifyCursorDateTime( nullptr );
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
  m_pTrackOrder.reset();
}

void Strategy::Start() {

  m_fConstructPosition(
    "SPY",
    [this]( pPosition_t pPosition ){
      m_pPosition = pPosition;
      m_pTrackOrder = std::make_unique<ou::tf::TrackOrder>( m_pPosition, m_cdv, EChartSlot::Price );
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
    m_cdv.SetNotifyCursorDateTime(
      // change this to be set from caller when a graphical user element is available
      // for displaying 'time & sales' datum from datetime supplied by cursor
      []( const boost::posix_time::ptime dt ){
        // arrives in gui thread
      } );
    m_fStart();
  }
}

void Strategy::SetupChart() {

  m_cemPosOne.AddMark( +1.0, ou::Colour::Black, "+1" );
    m_cemZero.AddMark(  0.0, ou::Colour::Black, "zero" );
  m_cemNegOne.AddMark( -1.0, ou::Colour::Black, "-1" );

  m_cemRegimMin.AddMark( c_regimeMinimum, ou::Colour::Red,   "min" );

  m_ceTradePrice.SetName( "Trade" );
  m_ceTradePrice.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Price, &m_ceTradePrice );

  m_ceTradeBBU.SetName( "BBU" );
  m_cdv.Add( EChartSlot::Price, &m_ceTradeBBU );

  m_ceTradeBBL.SetName( "BBL" );
  m_cdv.Add( EChartSlot::Price, &m_ceTradeBBL );

  if ( m_flags.bEnableBidAskPrice ) {
    m_ceAskPrice.SetName( "Ask" );
    m_ceAskPrice.SetColour( c_colourAsk );
    m_cdv.Add( EChartSlot::Price, &m_ceAskPrice );

    m_ceBidPrice.SetName( "Bid" );
    m_ceBidPrice.SetColour( c_colourBid );
    m_cdv.Add( EChartSlot::Price, &m_ceBidPrice );
  }

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

  if ( m_flags.bEnablePrediction ) {
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

    if ( m_flags.bEnableAdvDec ) {
      m_ceAdvDec_ratio.SetName( "AdvDec" );
      m_ceAdvDec_ratio.SetColour( c_colourAdvDec );
      m_cdv.Add( EChartSlot::Ratio, &m_ceAdvDec_ratio );
    }
  }

  m_ceTradeVolume.SetName( "Volume" );
  m_ceTradeVolume.SetColour( ou::Colour::Green );
  m_cdv.Add( EChartSlot::TickVolume, &m_ceTradeVolume );

  //m_ceTradeVolumeUp.SetName( "Volume - Buy" );
  //m_ceTradeVolumeUp.SetColour( ou::Colour::Green );
  //m_cdv.Add( EChartSlot::TickVolume, &m_ceTradeVolumeUp );
  //m_ceTradeVolumeDn.SetName( "Volume - Sell" );
  //m_ceTradeVolumeDn.SetColour( ou::Colour::Red );
  //m_cdv.Add( EChartSlot::TickVolume, &m_ceTradeVolumeDn );

  if ( m_flags.bEnableBidAskVolume ) {
    m_ceAskVolume.SetName( "Ask" );
    m_ceAskVolume.SetColour( ou::Colour::Red );
    m_cdv.Add( EChartSlot::QuoteVolume, &m_ceAskVolume );

    m_ceBidVolume.SetName( "Bid" );
    m_ceBidVolume.SetColour( ou::Colour::Blue );
    m_cdv.Add( EChartSlot::QuoteVolume, &m_ceBidVolume );
  }

  m_cdv.Add( EChartSlot::TickStat, &m_cemZero );

  m_ceTickJ.SetName( "TickJ" );
  m_ceTickJ.SetColour( c_colourTickJ );
  m_cdv.Add( EChartSlot::TickStat, &m_ceTickJ );

  m_ceTickL.SetName( "TickL" );
  m_ceTickL.SetColour( c_colourTickL );
  m_cdv.Add( EChartSlot::TickStat, &m_ceTickL );

  if ( m_flags.bEnableAdvDec ) {
    m_ceAdvDec.SetName( "AdvDec" );
    m_ceAdvDec.SetColour( c_colourAdvDec );
    m_cdv.Add( EChartSlot::AdvDec, &m_cemZero );
    m_cdv.Add( EChartSlot::AdvDec, &m_ceAdvDec );
  }

  //m_ceRtnPrice.SetName( "Price Returns" );
  //m_cdv.Add( EChartSlot::Price, &m_ceRtnPrice_bbu );
  //m_cdv.Add( EChartSlot::rtnPrice, &m_ceRtnPrice );
  //m_cdv.Add( EChartSlot::Price, &m_ceRtnPrice_bbl );

  if ( m_flags.bEnableImbalance ) {

    static const std::string sMarker( fmt::format( "{:.{}f}", c_ImbalanceMarker, 1 ) );

    m_cemImbalanceMarker.AddMark( +c_ImbalanceMarker, ou::Colour::Black, '+' + sMarker );
    m_cemImbalanceMarker.AddMark( -c_ImbalanceMarker, ou::Colour::Black, '-' + sMarker );

    m_cdv.Add( EChartSlot::Imbalance, &m_cemImbalanceMarker );
    m_cdv.Add( EChartSlot::Imbalance, &m_cemZero );

    m_ceImbalance.SetName( "Imbalance" );
    m_ceImbalance.SetColour( ou::Colour::Purple );
    m_cdv.Add( EChartSlot::Imbalance, &m_ceImbalance );
  }

  m_cdv.Add( EChartSlot::rtnPriceSD, &m_cemZero );

  m_ceTradeBBDiff.SetName( "price sd direction" );
  m_ceTradeBBDiff.SetColour( ou::Colour::Purple );
  m_cdv.Add( EChartSlot::rtnPriceSD, &m_ceTradeBBDiff );

  {
    static const std::string sMarker( fmt::format( "{:.{}f}", c_ReturnsMeanMarker, 3 ) );

    m_cemRtnPriceMarkers_mean.AddMark( +c_ReturnsMeanMarker, ou::Colour::Green, '+' + sMarker );
    m_cemRtnPriceMarkers_mean.AddMark( -c_ReturnsMeanMarker, ou::Colour::Red,   '-' + sMarker );

    m_cdv.Add( EChartSlot::rtnPriceMean, &m_cemRtnPriceMarkers_mean );
  }

  m_cdv.Add( EChartSlot::rtnPriceMean, &m_cemZero );
  m_ceRtnPrice_mean.SetName( "Returns - Average" );
  m_cdv.Add( EChartSlot::rtnPriceMean, &m_ceRtnPrice_mean );

  {
    static const std::string sMarker( fmt::format( "{:.{}f}", c_ReturnsSlopeMarker, 7 ) );

    m_cemRtnPriceMarkers_slope.AddMark( +c_ReturnsSlopeMarker, ou::Colour::Green, '+' + sMarker );
    m_cemRtnPriceMarkers_slope.AddMark( -c_ReturnsSlopeMarker, ou::Colour::Red,   '-' + sMarker );

    m_cdv.Add( EChartSlot::rtnPriceSlope, &m_cemRtnPriceMarkers_slope );
  }

  m_cdv.Add( EChartSlot::rtnPriceSlope, &m_cemZero );
  m_ceRtnPrice_slope.SetName( "Returns - Slope" );
  m_cdv.Add( EChartSlot::rtnPriceSlope, &m_ceRtnPrice_slope );

  //m_ceRtnPrice_slope_ema.SetName( "Returns - ema" );
  //m_ceRtnPrice_slope_ema.SetColour( ou::Colour::Purple );
  //m_cdv.Add( EChartSlot::rtnPriceSlp, &m_ceRtnPrice_slope_ema );

  m_cdv.Add( EChartSlot::TickRegime, &m_cemRegimMin );
  m_cdv.Add( EChartSlot::TickRegime, &m_cemZero );

  m_ceTickRegime.SetName( "Tick Regime" );
  m_ceTickRegime.SetColour( c_colourTickRegime );
  m_cdv.Add( EChartSlot::TickRegime, &m_ceTickRegime );

  if ( m_flags.bEnablePrediction ) {
    m_cePrediction_scaled.SetName( "Predict" );
    m_cePrediction_scaled.SetColour( c_colourPrdct );
    m_cdv.Add( EChartSlot::Predict, &m_ceTrade_ratio );
    m_cdv.Add( EChartSlot::Predict, &m_cePrediction_scaled );

    m_cePrediction_descaled.SetName( "Predict" );
    m_cePrediction_descaled.SetColour( c_colourPrdct );
    m_cdv.Add( EChartSlot::Price, &m_cePrediction_descaled );
  }

  m_ceProfitLoss.SetName( "P/L" );
  m_cdv.Add( EChartSlot::PL, &m_cemZero );
  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

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

  if ( ( 0 == quote.AskSize() ) || ( 0 == quote.BidSize() ) ) {} // only a couple are zero
  else {
  //  BOOST_LOG_TRIVIAL(debug)
  //    << "quote"
  //    << ',' << quote.BidSize()
  //    << '@' << quote.Bid()
  //    << ',' << quote.AskSize()
  //    << '@' << quote.Ask()
  //    ;

    switch ( CurrentTimeFrame() ) {
      case TimeFrame::RHTrading:
      case TimeFrame::Cancelling:
      case TimeFrame::GoingNeutral:
      case TimeFrame::WaitForRHClose:
        if ( m_flags.bEnableBidAskPrice ) {
          m_ceAskPrice.Append( dt, quote.Ask() );
          m_ceBidPrice.Append( dt, quote.Bid() );
        }
        if ( m_flags.bEnableBidAskVolume ) {
          m_ceAskVolume.Append( dt, quote.AskSize() );
          m_ceBidVolume.Append( dt, -quote.BidSize() );
        }
        if ( m_flags.bEnableImbalance ) {
          m_dblQuoteImbalance = quote.Imbalance();
          UpdateECross( m_ECross_imbalance, c_ImbalanceMarker, m_dblQuoteImbalance );
          m_ceImbalance.Append( dt, m_dblQuoteImbalance );
        }
        break;
      default:
        break;
    }

    TimeTick( quote );

    // provides a 1 sec pulse for checking the algorithm after tick activities
    m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 );

  }
}

void Strategy::UpdateECross( ECross& ec, const double mark, const double value ) {
  if ( 0.0 == value ) {
    ec = ECross::zero;
  }
  else {
    if ( 0.0 < value ) {
      if ( +mark == value ) {
        ec = ECross::uppermk;
      }
      else {
        if ( +mark > value ) {
          ec = ECross::upperlo;
        }
        else { // +mark < value
          ec = ECross::upperhi;
        }
      }
    }
    else { // 0.0 > mean
      if ( -mark == value ) {
        ec = ECross::lowermk;
      }
      else {
        if ( -mark > value ) {
          ec = ECross::lowerlo;
        }
        else { // -mark < value
          ec = ECross::lowerhi;
        }
      }
    }
  }
}

void Strategy::UpdatePriceReturn( ou::tf::Price::dt_t dt, ou::tf::Price::price_t price ) {

  if ( ( 0.0 == m_dblPrvPrice ) ) {}
  else {

    const double rtn = std::log( price / m_dblPrvPrice ); // natural log, ie ln
    m_returns.Append( ou::tf::Price( dt, rtn ) );

    const double sd(    m_statsReturns.SD() );
    const double mean(  m_statsReturns.MeanY() / sd  );
    const double slope( m_statsReturns.Slope() / sd );
    // todo: consider multi-time frame sd estimation

    m_ixprvCrossing = ( m_ixprvCrossing + 1 ) % 2;
    m_ixcurCrossing = ( m_ixcurCrossing + 1 ) % 2;

    rCross_t& rcs( m_crossing[ m_ixcurCrossing ] );

    {
      CrossState& cs( rcs[ rtn_mean ] );
      ECross& ec( cs.cross );
      UpdateECross( ec, c_ReturnsMeanMarker, mean );
      cs.value = mean;

    }
    //m_ceRtnPrice_mean.Append( dt, mean / c_ReturnsMeanMarker ); // possible normalization, try sigmoid
    m_ceRtnPrice_mean.Append( dt, mean );

    {
      CrossState& cs( rcs[ rtn_slope ] );
      ECross& ec( cs.cross );
      UpdateECross( ec, c_ReturnsSlopeMarker, slope );
      cs.value = slope;
    }
    //m_ceRtnPrice_slope.Append( dt, slope / c_ReturnsSlopeMarker ); // possible normalization, try sigmoid
    m_ceRtnPrice_slope.Append( dt, slope );

    rcs[ rtn_sd ].value = sd;

    // relate price, mean, slope, sd to momentum/trend?
    // predict the volatility (via garch?) might be the solution

    // rather than an ema, use a linear regression, or a second order polynomial to get some curve?
    // then use this near the bollinger band with some idea of jitter
    // to determine when the envelope has been broken for change in direction
    // but will probably still get faked out?  but how often?

    //static const double multiplier( 3.0 );
    //static const double multiplier_prv( ( multiplier - 1.0 ) / multiplier );
    //static const double multiplier_cur( 1.0 - multiplier_prv );
    //if ( !std::isnan( slope ) ) {
    //  m_dblRtnPrice_slope_ema = ( multiplier_prv * m_dblRtnPrice_slope_ema ) + ( multiplier_cur * slope );
    //  m_ceRtnPrice_slope_ema.Append( dt, m_dblRtnPrice_slope_ema );
    //}

  }
  m_dblPrvPrice = price;
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  const bool direction( m_quote.LeeReady( m_trade.Price(), trade.Price() ) );

  m_trade = trade;

  const ou::tf::Price::dt_t         dt( trade.DateTime() );
  const ou::tf::Price::price_t   price( trade.Price() );
  const ou::tf::Price::volume_t volume( trade.Volume() );

  m_vwp.Add( price, volume );
  m_ceTradePrice.Append(  dt, price );

  // more information might be available if this can be matched to walking the quotes/order book
  //if ( direction ) {
  //  m_ceTradeVolumeUp.Append( dt, volume );
  //}
  //else {
  //  m_ceTradeVolumeDn.Append( dt, -volume );
  //}
  m_ceTradeVolume.Append( dt, volume );

  UpdatePriceReturn( dt, price );

  m_prices.Append( ou::tf::Price( dt, price ) );
  //m_atr = m_minmaxPrices.Diff();
  //m_ceVisualize.Append( dt, m_atr );

  m_ceTradeBBU.Append( dt, m_statsPrices.BBUpper() );
  m_ceTradeBBL.Append( dt, m_statsPrices.BBLower() );

  const double bboffset( m_statsPrices.BBOffset() );
  //m_ceTradeBBDiff.Append( dt, bboffset );
  m_ceTradeBBDiff.Append( dt, ( bboffset >= m_dblPrvSD ) ? 1.0 : -1.0 ); // track rise/fall rather than value
  m_dblPrvSD = bboffset;

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
  if ( c_regimeMinimum >= dblTickRegime ) {
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
  if ( m_flags.bEnableAdvDec ) {
    if ( RHTrading() ) {
      m_features.dblAdv = tick.Price();
      CalcAdvDec( tick.DateTime() );
    }
  }
}

void Strategy::HandleDec( const ou::tf::Trade& tick ) {
  if ( m_flags.bEnableAdvDec ) {
    if ( RHTrading() ) {
      m_features.dblDec = tick.Price();
      CalcAdvDec( tick.DateTime() );
    }
  }
}

void Strategy::CalcAdvDec( boost::posix_time::ptime dt ) {
  const double diff( m_features.dblAdv - m_features.dblDec );
  const double sum( m_features.dblAdv + m_features.dblDec );
  const double ratio( diff / sum );
  m_features.dblAdvDecRatio = ratio;

  m_ceAdvDec.Append( dt, diff - m_dblPrvAdvDec );
  //if ( ( 0.0 == m_dblPrvAdvDec ) ) {}
  //else {
  //  m_ceAdvDec.Append( dt, ratio );
  //}
  m_dblPrvAdvDec = diff;
  //m_dblPrvAdvDec = ratio;

}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  if ( RHTrading() ) {
    Calc01SecIndicators( bar );
  }
  TimeTick( bar );
}

/*
      switch ( m_TickRegime ) {
        case ETickRegime::advance:
          //if ( m_bTickRegimeIncreased ) {
          //if ( ( 0.0 < m_statsReturns.MeanY() ) && ( 0.0 < m_statsReturns.Slope() ) ) {
          if ( ( 0.0 < m_statsReturns.Slope() ) ) {
            //BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
            m_ceLongEntry.AddLabel( dt, price, "long" );
            ++m_nEnterLong;
            m_stateTrade = ETradeState::LongSubmitted;
          }
          break;
        case ETickRegime::decline:
          //if ( m_bTickRegimeIncreased ) {
          //if ( ( 0.0 > m_statsReturns.MeanY() ) && ( 0.0 > m_statsReturns.Slope() ) ) {
          if ( ( 0.0 > m_statsReturns.Slope() ) ) {
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
*/

void Strategy::HandleRHTrading( const ou::tf::Trade& trade ) {
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  // run state machine here as the bollinger bands are updated only with incoming trades?
  // using limit orders at the bollinger band is independent of quotes
  // however once quotes get near the edge, then important to rapidly update
  // or make decisions based upon movement and nbbo changes
  // might be able to see order book being walked
  // which is the intent of the imbalance if it could be made sense of
  // for the time being, just put in limit orders to track as a simulated bracket order
  // make use of the sd direction to confirm direction
  // break bb width into thirds for hysteresis
  // possibly use the 'bollinger outside of long ema' as a signal

  // todo split bb into three parts for hysteresis when supplying liquidity
  //   to be used by quote section

  switch ( m_pTrackOrder->State()() ) {
    case ETradeState::Search:
      break;
    case ETradeState::EntrySubmittedUp:
      break;
    case ETradeState::EntrySubmittedDn:
      break;
    case ETradeState::ExitSignalUp:
      break;
    case ETradeState::ExitSignalDn:
      break;
    case ETradeState::ExitSubmitted:
      break;
    case ETradeState::EndOfDayCancel: // not in HandleRHTrading, set in one shot HandleCancel
      break;
    case ETradeState::EndOfDayNeutral: // not in HandleRHTrading, set in one shot HandleGoNeutral
      break;
    case ETradeState::Done:
      break;
    case ETradeState::Init:
      m_pTrackOrder->State().Set( ETradeState::Search );
      break;
    default:
      // todo: track unused states
      break;
  }
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  const auto dt( quote.DateTime() );
  // might be better here, the trade will provide bollinger bands, but we need to watch
  // the progression of quotes.  bid/ask will set the range of 'current' trades/returns.
  // if the desire is to provide liquidity, then bid/ask will be need to lead or track
  // to current top of bid/ask order book
  // based upon mean/slope, can the a prediction be made on how far this will go?
  // how much is predicted by order book imbalance/change?

  static const double c_nd( 2.0 );

  const rCross_t& rcp( m_crossing[ m_ixprvCrossing ] );
  const rCross_t& rcc( m_crossing[ m_ixcurCrossing ] );

  switch ( m_pTrackOrder->State()() ) {
    case ETradeState::Search:
      switch ( rcc[ rtn_slope ].cross ) {
        case ECross::upperhi:
          break;
        case ECross::uppermk:
          break;
        case ECross::upperlo:
          if ( ( ECross::upperhi == rcp[ rtn_slope ].cross )
            && ( ECross::upperhi == rcp[ rtn_mean ].cross )
          ) {
            //BOOST_LOG_TRIVIAL(trace) << "ETickHi::DnOvr enter";
            //m_ceShortEntry.AddLabel( dt, price, "short" );
            ++m_nEnterShort;
            //m_to.State().Set( ETradeState::EntrySubmittedDn );
            m_stopDelta = c_nd * m_statsPrices.SD();;
            m_stopTrail = m_stopInitial = m_quote.Ask() + m_stopDelta;
            ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_trade.Price(), m_quote.Bid(), 5 );
            // todo: set fCancelled, fFilled
            m_pTrackOrder->EnterShortLmt( oa );
          }
          break;
        case ECross::zero:
          break;
        case ECross::lowerhi:
          if ( ( ECross::lowerlo == rcp[ rtn_slope ].cross )
            && ( ECross::lowerlo == rcp[ rtn_mean ].cross )
          ) {
            //BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
            //m_ceLongEntry.AddLabel( dt, price, "long" );
            ++m_nEnterLong;
            //m_to.State().Set( ETradeState::EntrySubmittedUp );
            m_stopDelta = c_nd * m_statsPrices.SD();;
            m_stopTrail = m_stopInitial = m_quote.Bid() - m_stopDelta;
            ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_trade.Price(), m_quote.Ask(), 5 );
            // todo: set fCancelled, fFilled
            m_pTrackOrder->EnterLongLmt( oa );
          }
          break;
        case ECross::lowermk:
          break;
        case ECross::lowerlo:
          break;
      }
      break;
    case ETradeState::EntrySubmittedUp:
      if ( !m_bTickRegimeIncreased ) {
        //BOOST_LOG_TRIVIAL(trace) << "ETickLo::Neutral exit";
        //m_pTrackOrder->State().Set( ETradeState::Search );
      }
      break;
    case ETradeState::EntrySubmittedDn:
      if ( !m_bTickRegimeIncreased ) {
        //BOOST_LOG_TRIVIAL(trace) << "ETickHi::Neutral exit";
        //m_pTrackOrder->State().Set( ETradeState::Search );
      }
      break;
    case ETradeState::ExitSignalUp:
      UpdatePositionProgressUp( quote );
      break;
    case ETradeState::ExitSignalDn:
      UpdatePositionProgressDn( quote );
      break;
    case ETradeState::ExitSubmitted:
      break;
    case ETradeState::EndOfDayCancel: // not in HandleRHTrading, set in one shot HandleCancel
      //BOOST_LOG_TRIVIAL(trace) << "eod cancel," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayNeutral: // not in HandleRHTrading, set in one shot HandleGoNeutral
      //BOOST_LOG_TRIVIAL(trace) << "eod neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::Done:
      break;
    case ETradeState::Init:
      m_pTrackOrder->State().Set( ETradeState::Search );
      break;
    default:
      // todo: track unused states
      break;
  }
}

void Strategy::UpdatePositionProgressUp( const ou::tf::Trade& trade ) {
  const auto price( trade.Price() );
  if ( m_stopTrail > price ) {
    const auto dt( trade.DateTime() );
    ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_stopTrail );
    // todo: set fCancelled, fFilled
    m_pTrackOrder->ExitLongMkt( oa );
  }
  else {
    //if ( m_atr < m_stopDelta ) m_stopDelta = m_atr;
    const double stop( price - m_stopDelta );
    if ( m_stopTrail < stop ) {
      m_stopTrail = stop;
    }
  }
}

void Strategy::UpdatePositionProgressDn( const ou::tf::Trade& trade ) {
  const auto price( trade.Price() );
  if ( m_stopTrail < price ) {
    const auto dt( trade.DateTime() );
    ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_stopTrail );
    // todo: set fCancelled, fFilled
    m_pTrackOrder->ExitShortMkt( oa );
  }
  else {
    //if ( m_atr < m_stopDelta ) m_stopDelta = m_atr;
    const double stop( price + m_stopDelta );
    if ( m_stopTrail > stop ) {
      m_stopTrail = stop;
    }
  }
}

void Strategy::UpdatePositionProgressUp( const ou::tf::Quote& quote ) {
  const auto ask( quote.Ask() );
  if ( m_stopTrail > ask ) {
    const auto dt( quote.DateTime() );
    ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_stopTrail );
    // todo: set fCancelled, fFilled
    m_pTrackOrder->ExitLongMkt( oa );
  }
  else {
    //if ( m_atr < m_stopDelta ) m_stopDelta = m_atr;
    const double stop( ask - m_stopDelta );
    if ( m_stopTrail < stop ) {
      m_stopTrail = stop;
    }
  }
}

void Strategy::UpdatePositionProgressDn( const ou::tf::Quote& quote ) {
  const auto bid( quote.Bid() );
  if ( m_stopTrail < bid ) {
    const auto dt( quote.DateTime() );
    ou::tf::TrackOrder::OrderArgs oa( dt, 100, m_stopTrail );
    //if ( m_atr < m_stopDelta ) m_stopDelta = m_atr;
    m_pTrackOrder->ExitShortMkt( oa );
  }
  else {
    //if ( m_atr < m_stopDelta ) m_stopDelta = m_atr;
    const double stop( bid + m_stopDelta );
    if ( m_stopTrail > stop ) {
      m_stopTrail = stop;
    }
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
  if ( m_flags.bEnablePrediction ) {
    m_cePrediction_scaled.Append( dtPrediction, scaled.predicted.dbl );
    m_cePrediction_descaled.Append( dtPrediction, scaled.predicted.dbl * scaled.range + scaled.min );
  }

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

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second, update statistics
  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );
}

// review interaction with TradeState, TrackOrder
void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "HandleCancel," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_pTrackOrder->State().Set( ETradeState::EndOfDayCancel );
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

// review interaction with TradeState, TrackOrder
void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "HandleGoNeutral," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_pTrackOrder->State().Set( ETradeState::EndOfDayNeutral );
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

/*
https://medium.com/@jatinnavani/predicting-stock-prices-using-log-returns-and-exponential-moving-averages-147634412a59
Log returns are preferred over simple percentage returns for several reasons:

  * Statistical Properties: Log returns are more statistically stable than simple returns. They can be modeled
    using a normal distribution, making them suitable for various statistical techniques.
  * Time Additivity: Log returns are time-additive, meaning that the total return over multiple periods
    can be calculated by simply summing the log returns of each period. This property simplifies the
    mathematical modeling of returns over time.
  * Handling Small Values: For small percentage changes, log returns provide a more accurate representation
    of the growth rate than simple returns, as they do not exaggerate the effects of small fluctuations.
  * Volatility Analysis: Log returns provide a better basis for volatility analysis, allowing us to
    understand how much the price is expected to fluctuate based on historical data.
*/