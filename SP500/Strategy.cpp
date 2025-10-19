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

#include <cmath>

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
  static const ou::Colour::EColour c_colourPrdct(  ou::Colour::Blue );

  static const double c_tickHi( +1.0 );
  static const double c_tickLo( -1.0 );

  static const size_t c_window( 60 );

  static const double c_ImbalanceMarker( 0.7 );
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
, m_dblPrvPrice {}
, m_dblPrvSD {}
, m_dblEma013 {}, m_dblEma029 {}, m_dblEma050 {}, m_dblEma200 {}
, m_statsPrices( m_prices, boost::posix_time::time_duration( 0, 0, c_window ) )
, m_statsReturns( m_returns, boost::posix_time::time_duration( 0, 0, c_window ) )
, m_statsReturns_mean( m_returns_mean, boost::posix_time::time_duration( 0, 0, c_window ) )
, m_statsReturns_slope( m_returns_slope, boost::posix_time::time_duration( 0, 0, c_window ) )
, m_stopInitial {}, m_stopDelta {}, m_stopTrail {}
, m_dblQuoteImbalance {}
, m_ixcurCrossing( 0 ), m_ixprvCrossing( 1 )
, m_dblPrice_hi {}, m_dblPrice_start {}, m_dblPrice_lo {}
, m_dblPrice_sum_max_profit {}, m_dblPrice_sum_win {}, m_dblPrice_sum_loss {}, m_dblPrice_sum_max_loss {}
, m_nMaxProfit {}, m_nWin {}, m_nLoss {}, m_nMaxLoss {}
, m_dblZigHi {}, m_dblZigLo {}
, m_nZigZags {}, m_dblSumZigZags {}, m_eZigZag( EZigZag::init )
, m_cntQuotePriceChanged {}, m_cntQuotePriceUnchanged {}
, m_cntOffsetUp {}, m_cntOffsetDn {}
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

     m_cemPosOne.AddMark( +1.0, ou::Colour::DarkSlateBlue, "+1" );
  m_cemPointFive.AddMark(  0.5, ou::Colour::Purple, "0.5" );
       m_cemZero.AddMark(  0.0, ou::Colour::Black, "zero" );
     m_cemNegOne.AddMark( -1.0, ou::Colour::DarkSlateBlue, "-1" );

  m_ceTradePrice.SetName( "Trade" );
  m_ceTradePrice.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Price, &m_ceTradePrice );

  m_ceTradeBBU.SetName( "BBU" );
  m_cdv.Add( EChartSlot::Price, &m_ceTradeBBU );

  m_ceTradeBBL.SetName( "BBL" );
  m_cdv.Add( EChartSlot::Price, &m_ceTradeBBL );

  m_ceTradeZigZag.SetName( "ZigZag" );
  m_ceTradeZigZag.SetColour( ou::Colour::Brown );
  m_cdv.Add( EChartSlot::Price, &m_ceTradeZigZag );

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

  m_cdv.Add( EChartSlot::Ratio, &m_cemZero );

  m_cdv.Add( EChartSlot::Ratio, &m_cemPosOne );
  //m_cdv.Add( EChartSlot::Ratio, &m_cemZero );
  m_cdv.Add( EChartSlot::Ratio, &m_cemNegOne );
  m_ceTradePrice_bb_ratio.SetName( "price / bb" );
  m_ceTradePrice_bb_ratio.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTradePrice_bb_ratio );

  m_ceTradePrice_ema_bb_ratio.SetName( "price ema13 / bb " );
  m_ceTradePrice_ema_bb_ratio.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTradePrice_ema_bb_ratio );

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

  m_cdv.Add( EChartSlot::rtnPriceSDa, &m_cemZero );

  m_ceTradeBBDiff_val.SetName( "price sd direction" );
  m_ceTradeBBDiff_val.SetColour( ou::Colour::Purple );
  m_cdv.Add( EChartSlot::rtnPriceSDa, &m_ceTradeBBDiff_val );

  m_cdv.Add( EChartSlot::rtnPriceSDo, &m_cemZero );

  m_ceTradeBBDiff_vol.SetName( "price sd direction" );
  m_ceTradeBBDiff_vol.SetColour( ou::Colour::Purple );
  m_cdv.Add( EChartSlot::rtnPriceSDo, &m_ceTradeBBDiff_vol );

  m_ceRtnPrice_mean.SetColour( ou::Colour::Blue );
  m_ceRtnPrice_mean.SetName( "Returns - Mean" );
  m_cdv.Add( EChartSlot::Ratio, &m_ceRtnPrice_mean );

  m_ceRtnPrice_slope.SetColour( ou::Colour::Red );
  m_ceRtnPrice_slope.SetName( "Returns - Slope" );
  m_cdv.Add( EChartSlot::Ratio, &m_ceRtnPrice_slope );

  if ( m_flags.bEnablePrediction ) {
    m_cdv.Add( EChartSlot::Predict, &m_cemPointFive );
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

  if ( ( 0 == quote.AskSize() ) || ( 0 == quote.BidSize() ) ) {} // only a couple are zero
  else {

    const auto dt( quote.DateTime() );

    const double ask( quote.Ask() );
    const double bid( quote.Bid() );

    if ( ( ask == m_quote.Ask() ) && ( bid == m_quote.Bid() ) ) {
      m_cntQuotePriceUnchanged++;
    }
    else {
      m_cntQuotePriceChanged ++;

      m_quote = quote;

  //  BOOST_LOG_TRIVIAL(debug)
  //    << "quote"
  //    << ',' << quote.BidSize()
  //    << '@' << quote.Bid()
  //    << ',' << quote.AskSize()
  //    << '@' << quote.Ask()
  //    ;

      // track maximum excursions for profit potential
      switch ( m_pTrackOrder->State()() ) {
        case ETradeState::Init:
          break;
        case ETradeState::ExitSignalUp:
          {
            // sell to close position (uses bid)
            if ( bid > m_dblPrice_hi ) {
              m_dblPrice_hi = bid;
            }
            else {
              if ( bid < m_dblPrice_lo ) {
                m_dblPrice_lo = bid;
              }
            }
          }
          break;
        case ETradeState::ExitSignalDn:
          {
            // buy to close position (uses ask)
            if ( ask > m_dblPrice_hi ) {
              m_dblPrice_hi = ask;
            }
            else {
              if ( ask < m_dblPrice_lo ) {
                m_dblPrice_lo = ask;
              }
            }
          }
          break;
        default:
          break;
      }

      // track zig-zag against bollinger boundaries for profit potential
      const auto upper( m_statsPrices.BBUpper() );
      const auto lower( m_statsPrices.BBLower() );
      switch ( m_eZigZag ) {
        case EZigZag::init:
          {
            if ( 0.5 < ( upper - lower ) ) {
              if ( upper < ask ) {
                m_dtZigZag = dt;
                m_dblZigHi = upper;
                m_dblZigLo = lower;
                m_eZigZag = EZigZag::trackupper;
              }
              else {
                if ( lower > bid ) {
                  m_dtZigZag = dt;
                  m_dblZigHi = upper;
                  m_dblZigLo = lower;
                  m_eZigZag = EZigZag::tracklower;
                }
              }
            }
          }
          break;
        case EZigZag::trackupper:
          if ( lower > bid ) {
            // summarize & switch sides
            m_nZigZags++;
            m_dblSumZigZags += ( m_dblZigHi - m_dblZigLo );
            m_ceTradeZigZag.Append( m_dtZigZag, m_dblZigHi );
            m_dtZigZag = dt;
            m_dblZigLo = bid; // new benchmark
            m_eZigZag = EZigZag::tracklower;
          }
          else {
            // extend upper
            if ( ask > m_dblZigHi ) {
              m_dtZigZag = dt;
              m_dblZigHi = ask;
            }
          }
          break;
        case EZigZag::tracklower:
          if ( upper < ask ) {
            // summarize & switch sides
            m_nZigZags++;
            m_dblSumZigZags += ( m_dblZigHi - m_dblZigLo );
            m_ceTradeZigZag.Append( m_dtZigZag, m_dblZigLo );
            m_dtZigZag = dt;
            m_dblZigHi = ask; // new benchmark
            m_eZigZag = EZigZag::trackupper;
          }
          else {
            // extend lower
            if ( bid < m_dblZigLo ) {
              m_dtZigZag = dt;
              m_dblZigLo = bid;
            }
          }
          break;
      }

      // optional visual indicators
      switch ( CurrentTimeFrame() ) {
        case TimeFrame::RHTrading:
        case TimeFrame::Cancelling:
        case TimeFrame::GoingNeutral:
        case TimeFrame::WaitForRHClose:
          if ( m_flags.bEnableBidAskPrice ) {
            m_ceAskPrice.Append( dt, ask );
            m_ceBidPrice.Append( dt, bid );
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
}

void Strategy::UpdateECross( ECross& ec, const double mark, const double value ) const {
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

    if ( std::isnan( mean ) || std::isinf( mean ) ) {}
    else {
      m_returns_mean.Append( ou::tf::Price( dt, mean ) );
      const double bb_mean(   m_statsReturns_mean.MeanY() );
      const double bb_offset( m_statsReturns_mean.BBOffset() );
      const double normalized( ( mean - bb_mean ) / bb_offset );
      m_ceRtnPrice_mean.Append( dt, normalized );
      m_features.dblReturnsMean = normalized;

      CrossState& cs( rcs[ rtn_mean ] );
      ECross& ec( cs.cross );
      UpdateECross( ec, 1.0, normalized );
      cs.value = normalized;
    }

    if ( std::isnan( slope ) || std::isinf( slope ) ) {}
    else {
      m_returns_slope.Append( ou::tf::Price( dt, slope ) );
      const double bb_mean(   m_statsReturns_slope.MeanY() );
      const double bb_offset( m_statsReturns_slope.BBOffset() );
      const double normalized( ( slope - bb_mean ) / bb_offset );
      m_ceRtnPrice_slope.Append( dt, normalized );
      m_features.dblReturnsSlope = normalized;

      CrossState& cs( rcs[ rtn_slope ] );
      ECross& ec( cs.cross );
      UpdateECross( ec, 1.0, normalized );
      cs.value = normalized;
    }

    rcs[ rtn_sd ].value = sd;

    // relate price, mean, slope, sd to momentum/trend?
    // predict the volatility (via garch?) might be the solution

    // rather than an ema, use a linear regression, or a second order polynomial to get some curve?
    // then use this near the bollinger band with some idea of jitter
    // to determine when the envelope has been broken for change in direction
    // but will probably still get faked out?  but how often?

  }
  m_dblPrvPrice = price;
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  const ou::tf::Price::dt_t         dt( trade.DateTime() );
  const ou::tf::Price::price_t   price( trade.Price() );
  const ou::tf::Price::volume_t volume( trade.Volume() );

  const ou::tf::Price dt_price( dt, price );

  // needs to be constructed prior to the m_trade assignment
  // const bool direction( m_quote.LeeReady( m_trade.Price(), trade.Price() ) );

  m_trade = trade;

  m_vwp.Add( price, volume );
  m_ceTradePrice.Append( dt_price );

  // more information might be available if this can be matched to walking the quotes/order book
  //if ( direction ) {
  //  m_ceTradeVolumeUp.Append( dt, volume );
  //}
  //else {
  //  m_ceTradeVolumeDn.Append( dt, -volume );
  //}
  m_ceTradeVolume.Append( dt, volume );

  mapVolumeAtPrice_t::iterator iterVolumeAtPrice = m_mapVolumeAtPrice.find( price );
  if ( m_mapVolumeAtPrice.end() == iterVolumeAtPrice ) {
    m_mapVolumeAtPrice.emplace( price, volume );
  }
  else {
    iterVolumeAtPrice->second += volume;
  }

  UpdatePriceReturn( dt, price );  // updates m_crossing, m_ixprvCrossing, m_ixcurCrossing

  m_prices.Append( dt_price );

  const double bb_upper( m_statsPrices.BBUpper() );
  m_ceTradeBBU.Append( dt, bb_upper );

  const double bb_lower( m_statsPrices.BBLower() );
  m_ceTradeBBL.Append( dt, bb_lower );

  // todo: consider using a 2 or 3 degree polynomial to smooth (similar to CurrencyTrader/CubicRegression.cpp)
  const double bb_offset( m_statsPrices.BBOffset() );
  if ( bb_offset >= m_dblPrvSD ) {  // track rise/fall rather than value
    m_ceTradeBBDiff_vol.Append( dt, +1.0 );
    m_cntOffsetUp++;
    m_cntOffsetDn = 0;
    m_features.dblSDDirection = +1.0;
  }
  else {
    m_ceTradeBBDiff_vol.Append( dt, -1.0 );
    m_cntOffsetUp = 0;
    m_cntOffsetDn++;
    m_features.dblSDDirection = -1.0;
  }

  //m_ceTradeBBDiff.Append( dt, bb_offset - m_dblPrvSD ); // track rise/fall rather than value

  m_ceTradeBBDiff_val.Append( dt, bb_offset );
  m_dblPrvSD = bb_offset;

  const double bb_mean( m_statsPrices.MeanY() );
  const double price_normalized( ( price - bb_mean ) / bb_offset );
  m_ceTradePrice_bb_ratio.Append( dt, price_normalized );

  rCross_t& rcs( m_crossing[ m_ixcurCrossing ] );
  CrossState& cs( rcs[ prc_norm ] );
  ECross& ec( cs.cross );
  UpdateECross( ec, 1.0, price_normalized );
  cs.value = price_normalized;

  TimeTick( trade );
}

void Strategy::HandleTickJ( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const ou::tf::Price::dt_t       dt( tick.DateTime() );
    const ou::tf::Price::price_t price( tick.Price() );

    m_features.dblTickJ = price / 100.0;  // approx normalization
    m_ceTickJ.Append( dt, m_features.dblTickJ );
  }
}

void Strategy::HandleTickL( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const ou::tf::Price::dt_t       dt( tick.DateTime() );
    const ou::tf::Price::price_t price( tick.Price() );

    m_features.dblTickL = price / 200.0;  // approx normalization
    m_ceTickL.Append( dt, m_features.dblTickL );
  }
}

void Strategy::HandleAdv( const ou::tf::Trade& tick ) {
  if ( m_flags.bEnableAdvDec ) {
    if ( RHTrading() ) {
      //CalcAdvDec( tick.DateTime() );
    }
  }
}

void Strategy::HandleDec( const ou::tf::Trade& tick ) {
  if ( m_flags.bEnableAdvDec ) {
    if ( RHTrading() ) {
      //CalcAdvDec( tick.DateTime() );
    }
  }
}

void Strategy::CalcAdvDec( boost::posix_time::ptime dt ) {
  //m_ceAdvDec.Append( dt, diff - m_dblPrvAdvDec );
  //if ( ( 0.0 == m_dblPrvAdvDec ) ) {}
  //else {
  //  m_ceAdvDec.Append( dt, ratio );
  //}
  //m_dblPrvAdvDec = diff;
  //m_dblPrvAdvDec = ratio;
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  if ( RHTrading() ) {
    Calc01SecIndicators( bar );
  }
  TimeTick( bar );
}

void Strategy::HandleRHTrading( const ou::tf::Trade& trade ) {

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
/*
  switch ( m_pTrackOrder->State()() ) {
    case ETradeState::Search:
      switch ( Search( trade ) ) {
        case ESearchResult::buy:
          EnterLong( trade );
          break;
        case ESearchResult::none:
          break;
        case ESearchResult::sell:
          EnterShort( trade );
          break;
      }
      break;
    case ETradeState::EntrySubmittedUp:
      break;
    case ETradeState::EntrySubmittedDn:
      break;
    case ETradeState::ExitSignalUp:
      UpdatePositionProgressUp( trade );
      break;
    case ETradeState::ExitSignalDn:
      UpdatePositionProgressDn( trade );
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
*/
}

Strategy::ESearchResult Strategy::Search( const ou::tf::Trade& trade ) const {

  ESearchResult sr( ESearchResult::none );

  const rCross_t& rcp( m_crossing[ m_ixprvCrossing ] );
  const ECross prvCross( rcp[ prc_norm ].cross );

  const rCross_t& rcc( m_crossing[ m_ixcurCrossing ] );
  const ECross curCross( rcc[ prc_norm ].cross );

  switch ( curCross ) {
    case ECross::upperlo:
      if ( ( ECross::uppermk == prvCross ) || ( ECross::upperhi == prvCross ) ) {
        sr = ESearchResult::sell;
      }
      break;
    case ECross::lowerhi:
      if ( ( ECross::lowermk == prvCross ) || ( ECross::lowerlo == prvCross ) ) {
        sr = ESearchResult::buy;
      }
      break;
    default:
      break;
  }
  return sr;
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  // might be better here, the trade will provide bollinger bands, but we need to watch
  // the progression of quotes.  bid/ask will set the range of 'current' trades/returns.
  // if the desire is to provide liquidity, then bid/ask will be need to lead or track
  // to current top of bid/ask order book
  // based upon mean/slope, can the a prediction be made on how far this will go?
  // how much is predicted by order book imbalance/change?

  /*
  static const double c_nd( 2.0 );

  const rCross_t& rcp( m_crossing[ m_ixprvCrossing ] );
  const rCross_t& rcc( m_crossing[ m_ixcurCrossing ] );
*/

  const auto dt( quote.DateTime() );

  const auto ask( quote.Ask() );
  const auto bid( quote.Bid() );

  const auto ema13( m_dblEma013 );
  const auto ema29( m_dblEma029 );

  switch ( m_pTrackOrder->State()() ) {
    case ETradeState::Search:
      if ( ( 0 < m_cntOffsetUp )
        && ( ema13 > ema29 )
        && ( ema13 < bid )
      ) {
        ou::tf::OrderArgs oa( dt, 100, bid );
        m_pTrackOrder->EnterLongMkt( oa );
      }
      else {
        if ( ( 0 < m_cntOffsetUp )
          && ( ema13 < ema29 )
          && ( ema13 > ask )
        ) {
          ou::tf::OrderArgs oa( dt, 100, ask );
          m_pTrackOrder->EnterShortMkt( oa );
        }
      }
      break;
    case ETradeState::EntrySubmittedUp:
      break;
    case ETradeState::EntrySubmittedDn:
      break;
    case ETradeState::ExitSignalUp:
      //UpdatePositionProgressUp( quote );
      if ( ( 0 < m_cntOffsetDn ) && ( ema13 > ask ) ) {
        ou::tf::OrderArgs oa( dt, 100, ask );
        m_pTrackOrder->ExitLongMkt( oa );
      }
      break;
    case ETradeState::ExitSignalDn:
      //UpdatePositionProgressDn( quote );
      if ( ( 0 < m_cntOffsetDn ) && ( ema13 < bid ) ) {
        ou::tf::OrderArgs oa( dt, 100, bid );
        m_pTrackOrder->ExitShortMkt( oa );
      }
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

Strategy::ESearchResult Strategy::Search( const ou::tf::Quote& quote ) const {

  ESearchResult sr( ESearchResult::none );

  /*
  try market orders to get the ball rolling
  later try limit orders to test probability of duration
  later try quote volume analysis to track direction probability
  use the paper to tie trades with quote changes
  sometime try RL with LSTM or Fuzzy with LSTM
  use fuzzy to define trending or sideways markets based upon ema distance?
    specifically look at sideways rather than direction change, drift, even if crossing should remain in-trade
  accumulate distance metrics to define relative fuzziness as session progresses
    session beginning may not be valid as few values are available
    may need to use previous day's values as a guide
    how much time is spent in each 4 or 5 percentiles

  entry:
    hi side:
      if bid > ema13 && ema13 > ema29 && 0 < m_cntOffsetUp then buy
    lo side:
      if ask < ema13 && ema13 < ema29 && 0 < m_cntOffsetDn then sell

  exit:
    hi side:
      track if ask < ema13 && 0 < m_cntOffsetDn
    lo side:
      track if bid > ema13 && 0 < m_cntOffsetUp

  note:
    track ema13 on entry to test how much it moves, and use for stop/exit
      rather than full retracing prior to exit
    flat ema13 means counter trading / no movement, maybe use limits for entry/exit
    successful trades will require an ema19 slope of such and such - can this be tracked?

  */

  return sr;
}

void Strategy::EnterLong( const ou::tf::Trade& trade ) {
  const auto nd( m_statsPrices.GetBBMultiplier() );
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  //BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
  //m_ceLongEntry.AddLabel( dt, price, "long" );
  ++m_nEnterLong;
  //m_to.State().Set( ETradeState::EntrySubmittedUp );
  m_stopDelta = nd * m_statsPrices.SD();;
  m_stopTrail = m_stopInitial = m_quote.Bid() - m_stopDelta;
  ou::tf::OrderArgs oa( dt, 100, price, m_quote.Ask(), 5 );
  oa.Set(
    nullptr // fOrderCancelled_t
  , [this]( ou::tf::OrderArgs::quantity_t quantity, double price, double commission ){ // fOrderFilled_t
      m_dblPrice_hi = m_dblPrice_start = m_dblPrice_lo = price;
    } );
  m_pTrackOrder->EnterLongLmt( oa );
}

void Strategy::EnterShort( const ou::tf::Trade& trade ) {
  const auto nd( m_statsPrices.GetBBMultiplier() );
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  //BOOST_LOG_TRIVIAL(trace) << "ETickHi::DnOvr enter";
  //m_ceShortEntry.AddLabel( dt, price, "short" );
  ++m_nEnterShort;
  //m_to.State().Set( ETradeState::EntrySubmittedDn );
  m_stopDelta = nd * m_statsPrices.SD();;
  m_stopTrail = m_stopInitial = m_quote.Ask() + m_stopDelta;
  ou::tf::OrderArgs oa( dt, 100, price, m_quote.Bid(), 5 );
  oa.Set(
    nullptr // fOrderCancelled_t
  , [this]( ou::tf::OrderArgs::quantity_t quantity, double price, double commission ){ // fOrderFilled_t
      m_dblPrice_hi = m_dblPrice_start = m_dblPrice_lo = price;
    } );
  m_pTrackOrder->EnterShortLmt( oa );
}

void Strategy::UpdatePositionProgressUp( const ou::tf::Trade& trade ) {
  bool bTestStop( true );

    switch ( Search( trade ) ) {
    case ESearchResult::buy:
      // ignore, still long
      break;
    case ESearchResult::none:
      break;
    case ESearchResult::sell:
      {
        // exit then enter short
        const auto dt( trade.DateTime() );
        const auto price( trade.Price() );
        ou::tf::OrderArgs oa_mkt( dt, 100, price );
        UpdatePositionProgressUp_order( oa_mkt, true );
        bTestStop = false;
      }
      break;
  }

  if ( bTestStop ) {
    const auto price( trade.Price() ); // todo: test the stops with quotes?
    if ( m_stopTrail > price ) { // if stopped out, don't enter, wait for signal, worse though, paradoxically
      const auto dt( trade.DateTime() );
      ou::tf::OrderArgs oa_mkt( dt, 100, m_stopTrail );
      UpdatePositionProgressUp_order( oa_mkt, false );
    }
    else {
      const double stop( price - m_stopDelta );
      if ( m_stopTrail < stop ) {
        m_stopTrail = stop;
      }
    }
  }
}

void Strategy::UpdatePositionProgressUp_order( ou::tf::OrderArgs& oa, bool reenter ) {
  using quantity_t = ou::tf::Order::quantity_t;
  oa.Set(
    [](){ // fOrderCancelled_t
      // go back to search
    }
  , [this,reenter]( quantity_t quantity, double price, double commission ){ // fOrderFilled_t, long closed
      if ( m_dblPrice_start < price ) {
        m_nWin++;
        m_dblPrice_sum_win += ( price - m_dblPrice_start );
      }
      else {
        m_nLoss++;
        m_dblPrice_sum_loss += ( m_dblPrice_start - price );
      }

      if ( m_dblPrice_start < m_dblPrice_hi ) {
        m_nMaxProfit++;
        m_dblPrice_sum_max_profit += ( m_dblPrice_hi - m_dblPrice_start );
      }

      if ( m_dblPrice_start > m_dblPrice_lo ) {
        m_nMaxLoss++;
        m_dblPrice_sum_max_loss += ( m_dblPrice_start - m_dblPrice_lo );
      }

      if ( reenter ) {
        EnterShort( m_trade );
      }

    } );
  m_pTrackOrder->ExitLongMkt( oa );
}

void Strategy::UpdatePositionProgressDn( const ou::tf::Trade& trade ) {
  bool bTestStop( true );

  switch ( Search( trade ) ) {
    case ESearchResult::buy:
      {
        // exit then enter long
        const auto dt( trade.DateTime() );
        const auto price( trade.Price() );
        ou::tf::OrderArgs oa_mkt( dt, 100, price );
        UpdatePositionProgressDn_order( oa_mkt, true );
        bTestStop = false;
      }
      break;
    case ESearchResult::none:
      break;
    case ESearchResult::sell:
      // ignore, still short
      break;
  }

  if ( bTestStop ) {
    const auto price( trade.Price() ); // todo: test the stops with quotes?
    if ( m_stopTrail < price ) { // if stopped out, don't enter, wait for signal, worse though, paradoxically
      const auto dt( trade.DateTime() );
      ou::tf::OrderArgs oa_mkt( dt, 100, m_stopTrail );
      UpdatePositionProgressDn_order( oa_mkt, false );
    }
    else {
      const double stop( price + m_stopDelta );
      if ( m_stopTrail > stop ) {
        m_stopTrail = stop;
      }
    }
  }
}

void Strategy::UpdatePositionProgressDn_order( ou::tf::OrderArgs& oa, bool reenter ) {
  using quantity_t = ou::tf::Order::quantity_t;
  oa.Set(
    [](){ // fOrderCancelled_t
      // go back to search
    }
  , [this,reenter]( quantity_t quantity, double price, double commission ){ // fOrderFilled_t, short closed
      if ( m_dblPrice_start > price ) {
        m_nWin++;
        m_dblPrice_sum_win += ( m_dblPrice_start - price );
      }
      else {
        m_nLoss++;
        m_dblPrice_sum_loss += ( price - m_dblPrice_start );
      }

      if ( m_dblPrice_start < m_dblPrice_hi ) {
        m_nMaxLoss++;
        m_dblPrice_sum_max_loss += ( m_dblPrice_hi - m_dblPrice_start );
      }

      if ( m_dblPrice_start > m_dblPrice_lo ) {
        m_nMaxProfit++;
        m_dblPrice_sum_max_profit += ( m_dblPrice_start - m_dblPrice_lo );
      }

      if ( reenter ) {
        EnterLong( m_trade );
      }

    } );
  m_pTrackOrder->ExitShortMkt( oa );
}

void Strategy::UpdatePositionProgressUp( const ou::tf::Quote& quote ) {
  const auto ask( quote.Ask() );
  if ( m_stopTrail > ask ) {
    const auto dt( quote.DateTime() );
    ou::tf::OrderArgs oa( dt, 100, m_stopTrail );
    // todo: set fCancelled, fFilled
    m_pTrackOrder->ExitLongMkt( oa );
  }
  else {
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
    ou::tf::OrderArgs oa( dt, 100, m_stopTrail );
    m_pTrackOrder->ExitShortMkt( oa );
  }
  else {
    const double stop( bid + m_stopDelta );
    if ( m_stopTrail > stop ) {
      m_stopTrail = stop;
    }
  }
}

// TODO: migrate to 0.10 second interval, or for each return instead?
void Strategy::Calc01SecIndicators( const ou::tf::Bar& bar ) {

  const auto dt( bar.DateTime() );

  m_features.dt = dt;

  const double vwp( m_vwp() );
  const double price( 0.0 == vwp ? bar.Close() : vwp );
  const ou::tf::Price price_( bar.DateTime(), price );

  UpdateEma< 13>( price_, m_dblEma013, m_ceEma013 );
  UpdateEma< 29>( price_, m_dblEma029, m_ceEma029 );
  UpdateEma< 50>( price_, m_dblEma050, m_ceEma050 );
  UpdateEma<200>( price_, m_dblEma200, m_ceEma200 );

  const double bb_offset( m_statsPrices.BBOffset() );
  const double bb_mean(   m_statsPrices.MeanY() );

  if ( 0.0 == bb_offset ) {}
  else {

    m_features.dblPrice = ( price - bb_mean ) / bb_offset;

    const double ema13_normalized( ( m_dblEma013 - bb_mean ) / bb_offset );
    m_ceTradePrice_ema_bb_ratio.Append( dt, ema13_normalized );
    m_features.dblEma013 = ema13_normalized;

    const double ema29_normalized( ( m_dblEma029 - bb_mean ) / bb_offset );
    m_features.dblEma029 = ema29_normalized;
  }

  Features_scaled scaled; // receives scaled data
  const ou::tf::Price result = m_fForward( m_features, scaled ); // may call PredictionVector for a live strategy
  if ( m_flags.bEnablePrediction ) {
    m_cePrediction_scaled.Append( result );
    //m_cePrediction_descaled.Append( dtPrediction, scaled.predicted.dbl );
  }

  //BOOST_LOG_TRIVIAL(trace) << "Calc01SecIndicators " << dt << ',' << prediction.DateTime();

  m_ceTrade_ratio.Append( dt, scaled.price.dbl );

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
  //BOOST_LOG_TRIVIAL(trace) << "HandleCancel," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_pTrackOrder->State().Set( ETradeState::EndOfDayCancel );
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

// review interaction with TradeState, TrackOrder
void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  //BOOST_LOG_TRIVIAL(trace) << "HandleGoNeutral," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_pTrackOrder->State().Set( ETradeState::EndOfDayNeutral );
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

void Strategy::HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration td ) {
  BOOST_LOG_TRIVIAL(trace) << "HandleAtRHClose," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  const double actual_profit( m_dblPrice_sum_win / m_nWin );
  BOOST_LOG_TRIVIAL(info) << "profit: " << m_dblPrice_sum_win << '/' << m_nWin << '=' << actual_profit;
  const double acutal_loss( m_dblPrice_sum_loss / m_nLoss );
  BOOST_LOG_TRIVIAL(info) << "  loss: " << m_dblPrice_sum_loss << '/' << m_nLoss << '=' << acutal_loss;
  BOOST_LOG_TRIVIAL(info) << "   net: " << m_dblPrice_sum_win - m_dblPrice_sum_loss;
  BOOST_LOG_TRIVIAL(info) << "factor: " << m_dblPrice_sum_win / m_dblPrice_sum_loss << " (> 1.0 is good)";
  const double possible_profit( m_dblPrice_sum_max_profit / m_nMaxProfit );
  BOOST_LOG_TRIVIAL(info) << "max missed profit: " << m_dblPrice_sum_max_profit << '/' << m_nMaxProfit << '=' << possible_profit;
  const double possible_loss( m_dblPrice_sum_max_loss / m_nMaxLoss );
  BOOST_LOG_TRIVIAL(info) << "max missed   loss: " << m_dblPrice_sum_max_loss << '/' << m_nMaxLoss << '=' << possible_loss;
  BOOST_LOG_TRIVIAL(info) << "              net: " << m_dblPrice_sum_max_profit - m_dblPrice_sum_max_loss;
  BOOST_LOG_TRIVIAL(info) << "zigzag: " << m_dblSumZigZags << '/' << m_nZigZags << '=' << m_dblSumZigZags / m_nZigZags;
  BOOST_LOG_TRIVIAL(info) << "quote price: " << m_cntQuotePriceChanged << " changed, " << m_cntQuotePriceUnchanged << " unchanged";
  BOOST_LOG_TRIVIAL(info) << "trade count: " << m_ceTradePrice.Size();
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