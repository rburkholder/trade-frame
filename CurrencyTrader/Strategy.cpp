/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * Project: CurrencyTrader
 * Created: March 14, 2024 21:32:07
 */

// see ATR trading notes at bottom
// TODO: load daily bars to calculate TR over last number of days
// TODO: change some of the screeners to use TR rather than bar ranges

// TODO: how many swings are on the proper side of ema?

// https://www.forex.com/en-us/forex-trading/how-to-trade-forex/ talks about the over-night roll-over fees

// "Pip" is an acronym for Percentage in Point or Price Interest Point.

// 2024/07/15 try spline or polynomial envelope on swings for contrarian trades

#include <boost/log/trivial.hpp>

#include <fmt/core.h>

#include "Strategy.hpp"

Strategy::Strategy( const config::Strategy& config )
: DailyTradeTimeFrame<Strategy>()
, m_eBaseCurrency( EBase::Unknown )
, m_quantityToOrder {}
, m_tick {}
, m_base_currency_pip {}
, m_bfQuotes01Sec( 1 )
, m_bfTrading( 60 )
, m_ceSwingHi( ou::ChartEntryShape::EShape::Long,  ou::Colour::Purple )
, m_ceSwingLo( ou::ChartEntryShape::EShape::Short, ou::Colour::HotPink )
, m_nLo {}, m_nNet {}, m_nHi {}
, m_nCount {}
, m_dblCommission {}
, m_fResetSoftware( nullptr )
{
  Init( config );
}

void Strategy::Init( const config::Strategy& config ) {

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName(    "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceBarsTrade.SetName( "Trades" );

  m_ceVolume.SetName(   "Volume" );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );

  m_cdv.Add( EChartSlot::Price, &m_ceSwingLo );
  m_cdv.Add( EChartSlot::Price, &m_ceSwingHi );

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceBarsTrade );

  m_cdv.Add( EChartSlot::Price, &m_ceCubicSwingDn );
  m_ceCubicSwingDn.SetColour( ou::Colour::Orange );
  m_cdv.Add( EChartSlot::Price, &m_ceCubicSwingUp );
  m_ceCubicSwingUp.SetColour( ou::Colour::Orange );

  m_cdv.Add( EChartSlot::Price, &m_cubicSwingDn.ChartEntry() );
  m_cubicSwingDn.ChartEntry().SetColour( ou::Colour::DarkTurquoise );
  m_cdv.Add( EChartSlot::Price, &m_cubicSwingUp.ChartEntry() );
  m_cubicSwingUp.ChartEntry().SetColour( ou::Colour::DarkSeaGreen );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_plTo.Init( m_cdv, EChartSlot::PL_To );
  m_plTtl.Init( m_cdv, EChartSlot::PL_Ttl );

  m_ceTradingRangeRising.SetColour( ou::Colour::Green );
  m_ceTradingRangeRising.SetName( "TR Rise" );
  m_ceTradingRangeFalling.SetColour( ou::Colour::Red );
  m_ceTradingRangeFalling.SetName( "TR Fall" );

  m_cdv.Add( EChartSlot::TR_EMA, &m_ceTradingRangeRising );
  m_cdv.Add( EChartSlot::TR_EMA, &m_ceTradingRangeFalling );

  static const ou::Colour::EColour colour[] = {
    ou::Colour::Purple
  , ou::Colour::HotPink
  , ou::Colour::DarkOrange
  , ou::Colour::DarkCyan
  };

  // supplied by 1 second mid-quote
  size_t ix {};
  for ( const auto seconds: config.m_vSmootherSeconds ) {
    pSmoother_t p = std::make_unique<ou::tf::indicator::Ema>( seconds, m_cdv, EChartSlot::Price );
    assert( ix < sizeof( colour ) );
    p->Set( colour[ ix ], "Smoother" + fmt::format( "{}", ix ) );
    m_vSmootherCurrency.emplace_back( std::move( p ) );
    ix++;
  }

  // supplied by 1 minute trade bar
  m_pATRFast = std::make_unique<ou::tf::indicator::Ema>( 3, m_cdv, EChartSlot::ATR );
  m_pATRFast->Set( ou::Colour::Blue, "ATR Fast" );

  // supplied by 1 minute trade bar
  m_pATRSlow = std::make_unique<ou::tf::indicator::Ema>( 14, m_cdv, EChartSlot::ATR );
  m_pATRSlow->Set( ou::Colour::Crimson, "ATR Slow" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );
  m_bfTrading.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleMinuteBar ) );
}

Strategy::~Strategy() {

  if ( m_pWatch ) {
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
    //m_pWatch->RecordSeries( true );
  }

  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_bfTrading.SetOnBarComplete( nullptr );

  m_pATRFast.reset();
  m_pATRSlow.reset();

  m_vSmootherCurrency.clear();

  m_cdv.Clear();
}

void Strategy::SetTransaction( ou::tf::Order::quantity_t quantity, fTransferFunds_t&& f ) {

  m_quantityToOrder = quantity;
  m_to.Set( f ); // make a copy of f

}

void Strategy::SetWatch( EBase eBaseCurrency, pWatch_t pWatch, pPortfolio_t pPortfolio, fConstructPosition_t&& f ) {

  assert( pWatch );
  assert( pPortfolio );
  assert( EBase::Unknown != eBaseCurrency );

  m_eBaseCurrency = eBaseCurrency;

  m_pWatch = pWatch;
  m_pPortfolio = pPortfolio; // TODO: update portfolio metrics to chart

  m_cdv.SetNames( "Currency Trader", m_pWatch->GetInstrumentName() );

  {
    pPosition_t pPosition = f( pWatch, pWatch->GetInstrumentName() );
    assert( pPosition );
    m_to.Set( pPosition, m_cdv, EChartSlot::Price );
  }

  //m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

Strategy::latest_t Strategy::Latest() const {
  return latest_t( m_quote.Bid(), m_quote.Ask(), m_nCount, m_dblCommission );
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {

  m_quote = quote;

  const ptime dt( m_quote.DateTime() );
  const double ask( m_quote.Ask() );
  const double bid( m_quote.Bid() );

  m_ceQuoteAsk.Append( dt, ask );
  m_ceQuoteBid.Append( dt, bid );

 if ( ask < m_trFalling.dblExtension ) m_trFalling.dblExtension = ask;
 if ( bid > m_trRising.dblExtension ) m_trRising.dblExtension = bid;

  //m_quotes.Append( quote );

  //TimeTick( quote );

  m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  const ptime dt( trade.DateTime() );
  //const ou::tf::Trade::price_t  price(  trade.Price() );
  const ou::tf::Trade::volume_t volume( trade.Volume() );

  // m_ceTrade.Append( dt, price ); // for iqfeed, is same as bid price for fxcm
  const double fake_price( m_quote.Midpoint() );
  m_ceTrade.Append( dt, fake_price );
  //m_ceVolume.Append( dt, volume ); // iqfeed shows as 1 for fxcm

  m_bfTrading.Add( dt, fake_price, volume );

}

void Strategy::HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration ) {

  // one time calc pip, https://www.benzinga.com/money/how-to-calculate-pips
  const double exch_rate( m_quote.Midpoint() );
  m_tick = m_to.PriceInterval( exch_rate );
  const double quan_x_tick = (double)m_quantityToOrder * m_tick;
  const double first( quan_x_tick / exch_rate );
  const double second( quan_x_tick );

  switch ( m_eBaseCurrency ) {
    case EBase::First:
      m_base_currency_pip = first;
      break;
    case EBase::Second:
      m_base_currency_pip = second;
      break;
    default:
      assert( false );
  }
  BOOST_LOG_TRIVIAL(info)
           << m_pWatch->GetInstrumentName()
    << ',' << "pip"
    << ',' << "exch_rate=" << exch_rate
    << ',' << "quan=" << m_quantityToOrder
    << ',' << "interval=" << fmt::format( "{:f}", m_tick )
    << ',' << "quan*tick=" << quan_x_tick
    << ',' << "first=" << first
    << ',' << "second=" << second // redundant?
    << ',' << "usd=" << m_base_currency_pip
    ;
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  const auto dt( bar.DateTime() );

  double unrealized, realized, commission, total;

  m_to.QueryStats( unrealized, realized, commission, total );
  m_plTo.m_ceUnRealized.Append( dt, unrealized );
  m_plTo.m_ceRealized.Append( dt, realized );
  m_plTo.m_ceProfitLoss.Append( dt, commission );
  m_plTo.m_ceCommission.Append( dt, total );

  m_pPortfolio->QueryStats( unrealized, realized, commission, total );
  m_plTtl.m_ceUnRealized.Append( dt, unrealized );
  m_plTtl.m_ceRealized.Append( dt, realized );
  m_plTtl.m_ceProfitLoss.Append( dt, commission );
  m_plTtl.m_ceCommission.Append( dt, total );

  static const double count( 4.0 );
  static const double count_base( count - 1.0 );
  static const double exist( count_base/count );
  static const double next( 1.0/count );

  const double ask( m_quote.Ask() );
  const double bid( m_quote.Bid() );

  switch ( m_state.swing ) {
    case State::Swing::up:
      m_state.sum += ( m_state.last - ask );
      m_state.last = ask;

      m_trFalling.dblEma = exist * m_trFalling.dblEma + next * ( m_trFalling.dblStart - m_trFalling.dblExtension );
      m_trFalling.dblStart = m_trFalling.dblExtension = bid;

      m_ceTradingRangeFalling.Append( dt, m_trFalling.dblEma );

      m_trRising.dblStart = m_trRising.dblExtension = ask;
      break;
    case State::Swing::none:
      break;
    case State::Swing::down:
      m_state.sum += ( bid - m_state.last );
      m_state.last = bid;

      m_trRising.dblEma = exist * m_trRising.dblEma + next * ( m_trRising.dblExtension - m_trRising.dblStart );
      m_trRising.dblStart = m_trRising.dblExtension = ask;

      m_ceTradingRangeRising.Append( dt, m_trRising.dblEma );

      m_trFalling.dblStart = m_trFalling.dblExtension = bid;
      break;
  }

  // things to check:
  //   track ATR at swingup/swingdn
  //   check distance from ema, if large correction, don't trade, if a few ATRs away, then consider trading
  //   check distance between swing points, small swings would require counter-trading
  //     use m_vSwingTrack for back-tracking

  RunState( m_to );

  m_state.swing = State::Swing::none;
}

void Strategy::RunState( ou::tf::TrackCurrencyOrder& to ) {
  using OrderArgs = ou::tf::TrackCurrencyOrder::OrderArgs;
  switch ( to.State()() ) {
    case ETradeState::Init: // Strategy starts in this state
      to.State().Set( ETradeState::Search, m_pWatch->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    case ETradeState::Search:
      switch ( m_state.swing ) {
        case State::Swing::up:
          {
            const double bid( m_quote.Bid() );
            const double swing_lo = m_rSwing[ 2 ].lo;
            const double diff1 = bid - swing_lo;
            const double diff2 = 3.0 * m_tick;
            m_stop.diff = diff1 > diff2 ? diff1 : diff2;
            m_stop.trail = bid - m_stop.diff; // run a parabolic stop?
            m_stop.start = m_stop.trail;
            to.Set( // fFillPrice_t
              [&to,this,bid]( double fill_price, double commission ){
                m_nCount++;
                m_dblCommission += commission;
                if ( fill_price < bid ) {
                  m_stop.trail = m_stop.start = ( fill_price - m_stop.diff );
                }
                to.Set( // fFillPrice_t
                  [this]( double fill_price, double commission ){
                    m_nCount++;
                    m_dblCommission += commission;
                    // cancel other stuff
                  } );
                const double limit( fill_price + 2.0 * m_tick );
                to.ExitShortLmt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, fill_price, limit ) );
                BOOST_LOG_TRIVIAL(info)
                  << m_pWatch->GetInstrumentName() << ','
                  << "up,fill=" << fill_price << ",short_limit=" << limit << ",stop.trail=" << m_stop.trail;
              });
            BOOST_LOG_TRIVIAL(info)
              << m_pWatch->GetInstrumentName() << ','
              << "up" << ','
              << "b=" << m_quote.Bid() << ','
              << "a=" << m_quote.Ask() << ','
              << "trf=" << m_TRFast.true_range << ','
              << "trs=" << m_TRSlow.true_range << ','
              << "sw=" << m_rSwing[0].lo << ',' << m_rSwing[1].lo << ',' << m_rSwing[2].lo << ',' << m_rSwing[3].lo << ',' << m_rSwing[4].lo << ','
              << "st=" << m_stop.start << ','
              << "df=" << m_stop.diff << ','
              << "trl=" << m_stop.trail
              ;
            assert( 0.0 < m_stop.diff );
            //to.EnterLongLmt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Ask(), m_quote.Bid(), 57 ) );
            to.EnterLongMkt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, m_quote.Ask() ) );
          }
          break;
        case State::Swing::none:
          break;
        case State::Swing::down:
          {
            const double ask( m_quote.Ask() );
            const double swing_hi = m_rSwing[ 2 ].hi;
            const double diff1 = swing_hi - ask;
            const double diff2 = 3.0 * m_tick;
            m_stop.diff = diff1 > diff2 ? diff1 : diff2;
            m_stop.trail = ask + m_stop.diff; // run a parabolic stop?
            m_stop.start = m_stop.trail;
            // TODO: if diff is less than expected, don't trade?
            //   need to track what typical range is?
            to.Set( // fFillPrice_t
              [&to,this,ask]( double fill_price, double commission ){
                m_nCount++;
                m_dblCommission += commission;
                if ( fill_price > ask ) {
                  m_stop.trail = m_stop.start = ( fill_price + m_stop.diff );
                }
                to.Set( // fFillPrice_t
                  [this]( double fill_price, double commission ){
                    m_nCount++;
                    m_dblCommission += commission;
                    // cancel other stuff
                  } );
                const double limit( fill_price - 2.0 * m_tick );
                to.ExitLongLmt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, fill_price, limit ) );
                BOOST_LOG_TRIVIAL(info)
                  << m_pWatch->GetInstrumentName() << ','
                  << "dn,fill=" << fill_price << ",long_limit=" << limit << ",stop.trail=" << m_stop.trail;
              });
            BOOST_LOG_TRIVIAL(info)
              << m_pWatch->GetInstrumentName() << ','
              << "dn" << ','
              << "b=" << m_quote.Bid() << ','
              << "a=" << m_quote.Ask() << ','
              << "trf=" << m_TRFast.true_range << ','
              << "trs=" << m_TRSlow.true_range << ','
              << "sw=" << m_rSwing[0].hi << ',' << m_rSwing[1].hi << ',' << m_rSwing[2].hi << ',' << m_rSwing[3].hi << ',' << m_rSwing[4].hi << ','
              << "st=" << m_stop.start << ','
              << "df=" << m_stop.diff << ','
              << "trl=" << m_stop.trail
              ;
            assert( 0.0 < m_stop.diff );
            //to.EnterShortLmt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Bid(), m_quote.Ask(), 57 ) );
            to.EnterShortMkt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, m_quote.Bid() ) );
          }
          break;
      }
      break;
    case ETradeState::EntrySubmittedUp:
    case ETradeState::EntrySubmittedDn:
      // wait for execution
      break;
    case ETradeState::ExitSignalUp: // need to move to quote
      {
        const double bid = m_quote.Bid();
        if ( bid <= m_stop.trail ) {
          to.Cancel(
            [&to,this, bid](){
              BOOST_LOG_TRIVIAL(info)
                << m_pWatch->GetInstrumentName() << ','
                << "up mkt stop on lmt cancel"
                ;
              to.ExitLongMkt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, bid ) );
            } );
        }
        else {
          // update trailing stop
          const double diff = bid - m_stop.trail;
          if ( diff > m_stop.diff ) {
            m_stop.trail = bid - m_stop.diff;
          }
        }
      }
      break;
    case ETradeState::ExitSignalDn: // need to move to quote
      {
        const double ask = m_quote.Ask();
        if ( ask >= m_stop.trail ) {
          to.Cancel(
            [&to,this, ask](){
              BOOST_LOG_TRIVIAL(info)
                << m_pWatch->GetInstrumentName() << ','
                << "dn mkt stop on lmt cancel"
                ;
              to.ExitShortMkt( OrderArgs( m_quote.DateTime(), m_quantityToOrder, ask ) );
            } );
        }
        else {
          // update trailing stop
          const double diff = m_stop.trail - ask;
          if ( diff > m_stop.diff ) {
            m_stop.trail = m_stop.diff - ask;
          }
        }
      }
      break;
    case ETradeState::ExitSubmitted:
      // wait for execution
      break;
    case ETradeState::Cancelling:
      //assert( false ); wait for state to clear, will it clear?
      break;
    case ETradeState::Cancelled:
      assert( false ); // is it legal to arrive here?
      break;
    case ETradeState::EndOfDayCancel:
      break;
    case ETradeState::EndOfDayNeutral:
      break;
    case ETradeState::NoTrade:
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false );
  }
}

void Strategy::HandleCancel( boost::gregorian::date date, boost::posix_time::time_duration td ) { // one shot
  m_to.HandleCancel( date, td );
}

void Strategy::HandleGoNeutral( boost::gregorian::date date, boost::posix_time::time_duration td ) { // one shot
  m_to.HandleGoNeutral( date, td );
}

void Strategy::HandleAtRHClose( boost::gregorian::date date, boost::posix_time::time_duration time ) { // one shot
  BOOST_LOG_TRIVIAL(info)
    << m_pWatch->GetInstrumentName()
    << " swing delta " << m_nNet
    << ", sum " << m_state.sum
    << ", " << date << 'T' << time
    ;
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  for ( pSmoother_t& p: m_vSmootherCurrency ) {
    p->Update( bar.DateTime(), bar.Close() );
  }

  TimeTick( bar );
}

void Strategy::HandleMinuteBar( const ou::tf::Bar& bar ) {

  const ptime dt( bar.DateTime() );
  m_ceBarsTrade.AppendBar( bar );

  m_ceVolume.Append( dt, bar.Volume() ); // iqfeed shows as 1 for fxcm

  m_pATRFast->Update( dt, m_TRFast.Update( bar ) );
  m_pATRSlow->Update( dt, m_TRSlow.Update( bar ) );

  // calculate swing points
  Swing& a( m_rSwing[ 0 ] );
  Swing& b( m_rSwing[ 1 ] );
  Swing& c( m_rSwing[ 2 ] );
  Swing& d( m_rSwing[ 3 ] );
  Swing& e( m_rSwing[ 4 ] );

  a = b; b = c; c = d; d = e; e.Update( bar, m_vSmootherCurrency.front()->Value() );

  { // highest point
    const double x = a.hi > b.hi ? a.hi : b.hi;
    const double y = d.hi > e.hi ? d.hi : e.hi;
    const double z = x > y ? x : y;
    if ( c.hi > z ) {
      m_ceSwingLo.AddLabel( c.dt, c.hi, "Swing Dn" );
      m_vSwingTrack.emplace_back( SwingTrack(
        bar.DateTime(),
        c.dt, c.hi,
        e.dt, e.hi ) );
      m_nHi++;
      m_nNet++;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "hi"
      //  << ',' << c.hi << ',' << e.hi
      //  << std::endl;
      m_state.swing = State::Swing::down;
      m_cubicSwingDn.Append( c.dt, c.hi );
      if ( m_cubicSwingDn.Full() ) {
        m_cubicSwingDn.CalcCoef();
        //m_state.swing = State::Swing::down;
      }
    }
  }

  { // lowest point
    const double x = a.lo < b.lo ? a.lo : b.lo;
    const double y = d.lo < e.lo ? d.lo : e.lo;
    const double z = x < y ? x : y;
    if ( c.lo < z ) {
      m_ceSwingHi.AddLabel( c.dt, c.lo, "Swing Up" );
      m_vSwingTrack.emplace_back( SwingTrack(
        bar.DateTime(),
        c.dt, c.lo,
        e.dt, e.lo ) );
      m_nLo++;
      m_nNet--;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "lo"
      //  << ',' << c.lo << ',' << e.lo
      //  << std::endl;
      m_state.swing = State::Swing::up;
      m_cubicSwingUp.Append( c.dt, c.lo );
      if ( m_cubicSwingUp.Full() ) {
        m_cubicSwingUp.CalcCoef();
        //m_state.swing = State::Swing::up;
      }
    }
  }

  // once a minute predictor
  if ( m_cubicSwingDn.Full() ) {
    const double dblCubicSwing = m_cubicSwingDn.Terpolate( ou::ChartEntryTime::Convert( dt ) );
    m_ceCubicSwingDn.Append( dt, dblCubicSwing );
  }

  // once a minute predictor
  if ( m_cubicSwingUp.Full() ) {
    const double dblCubicSwing = m_cubicSwingUp.Terpolate( ou::ChartEntryTime::Convert( dt ) );
    m_ceCubicSwingUp.Append( dt, dblCubicSwing );
  }

}

void Strategy::EmitCubicCoef() {
  BOOST_LOG_TRIVIAL(info) << "Emit " << m_pWatch->GetInstrumentName();
  m_cubicSwingDn.EmitCubicCoef();
  m_cubicSwingUp.EmitCubicCoef();
}

void Strategy::EmitSwingTrack() {
  BOOST_LOG_TRIVIAL(info)
    << m_pWatch->GetInstrumentName()
    << " swing bar state (tbd)"
  ;
}

bool Strategy::HandleSoftwareReset( boost::gregorian::date, boost::posix_time::time_duration ) {
  // validated by external call when available
  // clean up, clear out, start new statistics?
  if ( nullptr != m_fResetSoftware ) {
    return m_fResetSoftware();
  }
  else {
    return false;
  }
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pWatch ) {
    m_pWatch->SaveSeries( sPrefix );
    m_pWatch->ClearSeries();

    m_ceQuoteAsk.Clear();
    m_ceQuoteBid.Clear();
    m_ceTrade.Clear();
    m_ceVolume.Clear();

    m_plTo.Clear();
    m_plTtl.Clear();

    m_ceSwingHi.Clear();
    m_ceSwingLo.Clear();

    m_ceTradingRangeRising.Clear();
    m_ceTradingRangeFalling.Clear();
  }
}

void Strategy::CloseAndDone() {
  switch ( m_to.State()() ) {
    case ETradeState::Search:
      // nothing
      break;
    case ETradeState::EntrySubmittedUp:
    case ETradeState::EntrySubmittedDn:
    case ETradeState::ExitSubmitted:
      m_to.Cancel(
        [this](){
          m_to.Close(
            [](){
              // TrackOrder: to be implemented
            } );
        } );
      break;
    case ETradeState::ExitSignalUp:
    case ETradeState::ExitSignalDn:
      // waiting, so nothing to cancel
      break;
    default:
      // define other stuff
      break;
  }
}

  // The ATR is commonly used as an exit method that can be applied
  // no matter how the entry decision is made. One popular technique
  // is known as the "chandelier exit" and was developed by Chuck LeBeau:
  //   place a trailing stop under the highest high the stock has reached
  //   since you entered the trade. The distance between the highest high
  //   and the stop level is defined as some multiple multiplied by the ATR.

  // While the ATR doesn't tell us in which direction the breakout will occur,
  // it can be added to the closing price, and the trader can buy whenever
  // the next day's price trades above that value.

  // ATR can be used to set up entry signals.
  // ATR breakout systems are commonly used by short-term traders to time entries.
  // This system adds the ATR, or a multiple of the ATR, to the next day's open
  // and buys when prices move above that level. Short trades are the opposite;
  // the ATR or a multiple of the ATR is subtracted from the open and entries
  // occur when that level is broken.

  // maybe ATR could be connected with Darvis for timing entries.

  // To adjust for volatility you can take 10 000 (or whatever number you set as default)
  // and divide by the Average True Range over the last 100 days.
  // This way you allocate more capital to the least volatile stocks compared to the ones more volatile.

  // https://tradeciety.com/how-to-use-the-atr-indicator-guide for Kelner bands
  // ATR can be used to calculate typical PIP movement per day for trading volatility
  // use to calculate daily ATR for an idea of daily trading ranges

  // Timing Trades: ATR is crucial in deciding when to execute trades.
  // A high ATR, signaling high volatility, suggests larger price fluctuations and may be suitable
  // for breakout strategies or anticipating major market shifts.
  // A low ATR, indicating a quieter market, may favor strategies suited to a range-bound environment.

  // Setting Stop Losses: In determining stop-loss orders, ATR is invaluable.
  // It helps traders set stop losses that reflect current market volatility.
  // In a high-volatility (high ATR) scenario, a larger stop loss might be necessary
  // to avoid early exits due to market volatility. In contrast, in low-volatility (low ATR) markets,
  // tighter stop losses could be more effective, minimizing losses while
  // preventing unnecessary position exits.

  // Risk Management: ATR assists in risk management by providing insights into market volatility.
  // Traders can adjust their position sizes in response to this.
  // In volatile markets, reducing position sizes helps manage risk,
  // while in more stable conditions, larger positions might be beneficial.

  // Market Sentiment: ATR can also offer indirect indications of market sentiment.
  // A rapid increase in ATR may signal growing market tension or excitement,
  // potentially indicating upcoming significant market movements.
  // A gradual decrease in ATR can suggest a stabilizing market,
  // often leading to a consolidation phase.

  // ATR can be instrumental in weighing a limit or a market order.
  // The volatility measured by ATR can guide traders in deciding
  // whether to opt for a limit order, seeking a specific price point,
  // or a market order, executing a trade at the current market price,
  // depending on the market’s perceived stability or volatility.
