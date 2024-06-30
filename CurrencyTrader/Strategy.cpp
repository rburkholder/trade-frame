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

#include <boost/log/trivial.hpp>

#include "Strategy.hpp"

Strategy::Strategy()
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
  Init();
}

void Strategy::Init() {

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

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_plUp.Init( m_cdv, EChartSlot::PL_Up );
  m_plTtl.Init( m_cdv, EChartSlot::PL_Ttl );
  m_plDn.Init( m_cdv, EChartSlot::PL_Dn );

  m_ceTradingRangeRising.SetColour( ou::Colour::Green );
  m_ceTradingRangeRising.SetName( "TR Rise" );
  m_ceTradingRangeFalling.SetColour( ou::Colour::Red );
  m_ceTradingRangeFalling.SetName( "TR Fall" );

  m_cdv.Add( EChartSlot::TR_EMA, &m_ceTradingRangeRising );
  m_cdv.Add( EChartSlot::TR_EMA, &m_ceTradingRangeFalling );

  // supplied by 1 second mid-quote
  m_pEmaCurrency1 = std::make_unique<EMA>( 90, m_cdv, EChartSlot::Price );
  m_pEmaCurrency1->Set( ou::Colour::Purple, "Price EMA1" );

  m_pEmaCurrency2 = std::make_unique<EMA>( 5 * 60, m_cdv, EChartSlot::Price );
  m_pEmaCurrency2->Set( ou::Colour::DarkBlue, "Price EMA2" );

  // supplied by 1 minute trade bar
  m_pATRFast = std::make_unique<EMA>( 3, m_cdv, EChartSlot::ATR );
  m_pATRFast->Set( ou::Colour::Blue, "ATR Fast" );

  // supplied by 1 minute trade bar
  m_pATRSlow = std::make_unique<EMA>( 14, m_cdv, EChartSlot::ATR );
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
  m_pEmaCurrency1.reset();
  m_pEmaCurrency2.reset();

  m_cdv.Clear();
}

void Strategy::SetTransaction( ou::tf::Order::quantity_t quantity, fTransferFunds_t&& f ) {

  m_quantityToOrder = quantity;
  m_to_up.Set( quantity, f ); // make a copy of f
  m_to_dn.Set( quantity, f ); // make a copy of f

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
    pPosition_t pPosition = f( pWatch, pWatch->GetInstrumentName() +":up" );
    assert( pPosition );
    m_to_up.Set( pPosition, m_cdv, EChartSlot::Price );
  }

  {
    pPosition_t pPosition = f( pWatch, pWatch->GetInstrumentName() +":dn" );
    assert( pPosition );
    m_to_dn.Set( pPosition, m_cdv, EChartSlot::Price );
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
  m_tick = m_to_up.PriceInterval( exch_rate );
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
           << "pip"
    << ',' << m_pWatch->GetInstrumentName()
    << ',' << "exch rate=" << exch_rate
    << ',' << "quan=" << m_quantityToOrder
    << ',' << "interval=" << m_tick
    << '.' << "quan*tick=" << quan_x_tick
    << ',' << "first=" << first
    << ',' << "second=" << second // redundant?
    << ',' << "usd=" << m_base_currency_pip
    ;
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  const auto dt( bar.DateTime() );

  double unrealized, realized, commission, total;

  m_to_up.QueryStats( unrealized, realized, commission, total );
  m_plUp.m_ceUnRealized.Append( dt, unrealized );
  m_plUp.m_ceRealized.Append( dt, realized );
  m_plUp.m_ceProfitLoss.Append( dt, commission );
  m_plUp.m_ceCommission.Append( dt, total );

  m_pPortfolio->QueryStats( unrealized, realized, commission, total );
  m_plTtl.m_ceUnRealized.Append( dt, unrealized );
  m_plTtl.m_ceRealized.Append( dt, realized );
  m_plTtl.m_ceProfitLoss.Append( dt, commission );
  m_plTtl.m_ceCommission.Append( dt, total );

  m_to_dn.QueryStats( unrealized, realized, commission, total );
  m_plDn.m_ceUnRealized.Append( dt, unrealized );
  m_plDn.m_ceRealized.Append( dt, realized );
  m_plDn.m_ceProfitLoss.Append( dt, commission );
  m_plDn.m_ceCommission.Append( dt, total );

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

  RunStateUp( m_to_up );
  RunStateDn( m_to_dn );

  m_state.swing = State::Swing::none;
}

bool Strategy::SwingBarState( const Swing::EBarState eBarState ) const {
  // unused, eliminated too many signals
  return
       ( eBarState == m_rSwing[4].eBarState )
    && ( eBarState == m_rSwing[3].eBarState )
    && ( eBarState == m_rSwing[2].eBarState )
    && ( eBarState == m_rSwing[1].eBarState )
    && ( eBarState == m_rSwing[0].eBarState )
    ;
}

void Strategy::RunStateUp( TrackOrder& to ) {
  switch ( to.m_stateTrade ) {
    case TrackOrder::ETradeState::Init: // Strategy starts in this state
      to.m_stateTrade = TrackOrder::ETradeState::Search;
      break;
    case TrackOrder::ETradeState::Search:
      switch ( m_state.swing ) {
        case State::Swing::up:
          {
            const double bid( m_quote.Bid() );
            const double swing_lo = m_rSwing[ 2 ].lo;
            const double diff1 = bid - swing_lo;
            const double diff2 = 3.0 * m_tick;
            m_stopUp.diff = diff1 > diff2 ? diff1 : diff2;
            m_stopUp.trail = bid - m_stopUp.diff; // run a parabolic stop?
            m_stopUp.start = m_stopUp.trail;
            to.Set(
              [this,bid]( double fill_price, double commission ){
                m_nCount++;
                m_dblCommission += commission;
                if ( fill_price < bid ) {
                  m_stopUp.trail = m_stopUp.start = ( fill_price - m_stopUp.diff );
                }
              });
            BOOST_LOG_TRIVIAL(info)
              << m_pWatch->GetInstrumentName() << ','
              << "up" << ','
              << "b=" << m_quote.Bid() << ','
              << "a=" << m_quote.Ask() << ','
              << "trf=" << m_TRFast.true_range << ','
              << "trs=" << m_TRSlow.true_range << ','
              << "sw=" << m_rSwing[0].lo << ',' << m_rSwing[1].lo << ',' << m_rSwing[2].lo << ',' << m_rSwing[3].lo << ',' << m_rSwing[4].lo << ','
              << "st=" << m_stopUp.start << ','
              << "df=" << m_stopUp.diff << ','
              << "trl=" << m_stopUp.trail
              ;
            assert( 0.0 < m_stopUp.diff );
            to.EnterLongLmt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Ask(), m_quote.Bid(), 57 ) );
          }
          break;
        case State::Swing::none:
          break;
        case State::Swing::down:
          break;
      }
      break;
    case TrackOrder::ETradeState::EntrySubmitted:
      // wait for exectuion
      break;
    case TrackOrder::ETradeState::ExitSignal:
      switch ( m_state.swing ) {
        case State::Swing::up:
          break;
        case State::Swing::none:
          {
            const double bid = m_quote.Bid();
            if ( bid <= m_stopUp.trail ) {
              // exit with stop
              BOOST_LOG_TRIVIAL(info)
                << m_pWatch->GetInstrumentName() << ','
                << "up exit on none"
                ;
              to.ExitLongMkt( TrackOrder::OrderArgs( m_quote.DateTime(), bid ) );
            }
            else {
              // update trailing stop
              const double diff = bid - m_stopUp.trail;
              if ( diff > m_stopUp.diff ) {
                m_stopUp.trail = bid - m_stopUp.diff;
              }
            }
          }
          break;
        case State::Swing::down:
          // should be exiting earlier than this
          BOOST_LOG_TRIVIAL(info)
            << m_pWatch->GetInstrumentName() << ','
            << "up exit on down"
            ;
          to.ExitLongMkt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Midpoint() ) );
          break;
      }
      break;
    case TrackOrder::ETradeState::ExitSubmitted:
      // wait for execution
      break;
    case TrackOrder::ETradeState::NoTrade:
      break;
    case TrackOrder::ETradeState::EndOfDayCancel:
      break;
    case TrackOrder::ETradeState::EndOfDayNeutral:
      break;
    case TrackOrder::ETradeState::Done:
      break;
    default:
      assert( false );
  }
}

void Strategy::RunStateDn( TrackOrder& to ) {
  switch ( to.m_stateTrade ) {
    case TrackOrder::ETradeState::Init: // Strategy starts in this state
      to.m_stateTrade = TrackOrder::ETradeState::Search;
      break;
    case TrackOrder::ETradeState::Search:
      switch ( m_state.swing ) {
        case State::Swing::up:
          break;
        case State::Swing::none:
          break;
        case State::Swing::down:
          {
            const double ask( m_quote.Ask() );
            const double swing_hi = m_rSwing[ 2 ].hi;
            const double diff1 = swing_hi - ask;
            const double diff2 = 3.0 * m_tick;
            m_stopDn.diff = diff1 > diff2 ? diff1 : diff2;
            m_stopDn.trail = ask + m_stopDn.diff; // run a parabolic stop?
            m_stopDn.start = m_stopDn.trail;
            to.Set(
              [this,ask]( double fill_price, double commission ){
                m_nCount++;
                m_dblCommission += commission;
                if ( fill_price > ask ) {
                  m_stopUp.trail = m_stopUp.start = ( fill_price + m_stopDn.diff );
                }
              });
            BOOST_LOG_TRIVIAL(info)
              << m_pWatch->GetInstrumentName() << ','
              << "dn" << ','
              << "b=" << m_quote.Bid() << ','
              << "a=" << m_quote.Ask() << ','
              << "trf=" << m_TRFast.true_range << ','
              << "trs=" << m_TRSlow.true_range << ','
              << "sw=" << m_rSwing[0].hi << ',' << m_rSwing[1].hi << ',' << m_rSwing[2].hi << ',' << m_rSwing[3].hi << ',' << m_rSwing[4].hi << ','
              << "st=" << m_stopDn.start << ','
              << "df=" << m_stopDn.diff << ','
              << "trl=" << m_stopDn.trail
              ;
            assert( 0.0 < m_stopDn.diff );
            to.EnterShortLmt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Bid(), m_quote.Ask(), 57 ) );
          }
          break;
      }
      break;
    case TrackOrder::ETradeState::EntrySubmitted:
      // wait for exectuion
      break;
    case TrackOrder::ETradeState::ExitSignal:
      switch ( m_state.swing ) {
        case State::Swing::up:
          // should be exiting earlier than this
          BOOST_LOG_TRIVIAL(info)
            << m_pWatch->GetInstrumentName() << ','
            << "dn exit up"
            ;
          to.ExitShortMkt( TrackOrder::OrderArgs( m_quote.DateTime(), m_quote.Midpoint() ) );
          break;
        case State::Swing::none:
          {
            const double ask = m_quote.Ask();
            if ( ask >= m_stopDn.trail ) {
              // exit with stop
              BOOST_LOG_TRIVIAL(info)
                << m_pWatch->GetInstrumentName() << ','
                << "dn exit none"
                ;
              to.ExitShortMkt( TrackOrder::OrderArgs( m_quote.DateTime(), ask ) );
            }
            else {
              // update trailing stop
              const double diff = m_stopDn.trail - ask;
              if ( diff > m_stopDn.diff ) {
                m_stopDn.trail = m_stopDn.diff - ask;
              }
            }
          }
          break;
        case State::Swing::down:
          break;
      }
      break;
    case TrackOrder::ETradeState::ExitSubmitted:
      // wait for execution
      break;
    case TrackOrder::ETradeState::NoTrade:
      break;
    case TrackOrder::ETradeState::EndOfDayCancel:
      break;
    case TrackOrder::ETradeState::EndOfDayNeutral:
      break;
    case TrackOrder::ETradeState::Done:
      break;
    default:
      assert( false );
  }
}

void Strategy::HandleCancel( boost::gregorian::date date, boost::posix_time::time_duration td ) { // one shot
  m_to_up.HandleCancel( date, td );
  m_to_dn.HandleCancel( date, td );
}

void Strategy::HandleGoNeutral( boost::gregorian::date date, boost::posix_time::time_duration td ) { // one shot
  m_to_up.HandleGoNeutral( date, td );
  m_to_dn.HandleGoNeutral( date, td );
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
  assert( m_pEmaCurrency1 );
  assert( m_pEmaCurrency2 );
  m_pEmaCurrency1->Update( bar.DateTime(), bar.Close() );
  m_pEmaCurrency2->Update( bar.DateTime(), bar.Close() );
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

  a = b; b = c; c = d; d = e; e.Update( bar, m_pEmaCurrency1->dblEmaLatest );

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
      m_state.swing = State::Swing::down;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "hi"
      //  << ',' << c.hi << ',' << e.hi
      //  << std::endl;
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
      m_state.swing = State::Swing::up;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "lo"
      //  << ',' << c.lo << ',' << e.lo
      //  << std::endl;
    }
  }

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

    m_plUp.Clear();
    m_plTtl.Clear();
    m_plDn.Clear();

    m_ceSwingHi.Clear();
    m_ceSwingLo.Clear();

    m_ceTradingRangeRising.Clear();
    m_ceTradingRangeFalling.Clear();
  }
}

void Strategy::CloseAndDone() {
  // fake out the state machine, by dynamically adjusting the trigger times?
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
