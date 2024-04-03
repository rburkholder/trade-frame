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

#include <boost/log/trivial.hpp>

#include "Strategy.hpp"

Strategy::Strategy()
: m_bfQuotes01Sec( 1 )
, m_bfTrading( 60 )
, m_ceSwingHi( ou::ChartEntryShape::EShape::Long,  ou::Colour::Purple )
, m_ceSwingLo( ou::ChartEntryShape::EShape::Short, ou::Colour::HotPink )
{

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName(    "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceBarsTradeRise.SetName( "Trades" );
  m_ceBarsTradeFall.SetName( "Trades" );

  m_ceVolume.SetName(   "Volume" );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );

  m_ceBarsTradeRise.SetColour( ou::Colour::Green );
  m_ceBarsTradeFall.SetColour( ou::Colour::Red );

  m_cdv.Add( EChartSlot::Price, &m_ceSwingLo );
  m_cdv.Add( EChartSlot::Price, &m_ceSwingHi );

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceBarsTradeRise );
  m_cdv.Add( EChartSlot::Price, &m_ceBarsTradeFall );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  // supplied by 1 second mid-quote
  m_pEmaCurrency = std::make_unique<EMA>( 3 * 60, m_cdv, EChartSlot::Price );
  m_pEmaCurrency->Set( ou::Colour::Purple, "Price EMA" );

  // supplied by 1 minute trade bar
  m_pATRFast = std::make_unique<EMA>( 3, m_cdv, EChartSlot::ATR );
  m_pATRFast->Set( ou::Colour::Green, "ATR Fast" );

  // supplied by 1 minute trade bar
  m_pATRSlow = std::make_unique<EMA>( 14, m_cdv, EChartSlot::ATR );
  m_pATRSlow->Set( ou::Colour::Green, "ATR Slow" );

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
  m_pEmaCurrency.reset();

  m_cdv.Clear();
}

void Strategy::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  m_pPosition = pPosition;
  m_pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "Currency Trader", m_pPosition->GetInstrument()->GetInstrumentName() );

  //m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {

  m_quote = quote;

  const ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  //m_quotes.Append( quote );

  //TimeTick( quote );

  m_bfQuotes01Sec.Add( dt, quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

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

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second
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
      BOOST_LOG_TRIVIAL(error) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      m_stateTrade = ETradeState::Done;
      break;
    default:
      m_stateTrade = ETradeState::Search;
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
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      m_stateTrade = ETradeState::Search;
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

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  switch ( m_stateTrade ) {
    case ETradeState::NoTrade:
      // do nothing
      break;
    default:
      m_stateTrade = ETradeState::EndOfDayNeutral;
      if ( m_pPosition ) {
        m_pPosition->ClosePosition();
      }
      break;
  }
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  assert( m_pEmaCurrency );
  m_pEmaCurrency->Update( bar.DateTime(), bar.Close() );
}

void Strategy::HandleMinuteBar( const ou::tf::Bar& bar ) {

  const ptime dt( bar.DateTime() );
  if ( bar.Close() >= bar.Open() ) {
    m_ceBarsTradeRise.AppendBar( bar );
  }
  else {
    m_ceBarsTradeFall.AppendBar( bar );
  }

  m_ceVolume.Append( dt, bar.Volume() ); // iqfeed shows as 1 for fxcm

  m_pATRFast->Update( dt, m_TRFast.Update( bar ) );
  m_pATRSlow->Update( dt, m_TRSlow.Update( bar ) );

  // calculate swing points

  Swing& a( m_swing[ 0 ] );
  Swing& b( m_swing[ 1 ] );
  Swing& c( m_swing[ 2 ] );
  Swing& d( m_swing[ 3 ] );
  Swing& e( m_swing[ 4 ] );

  a = b; b = c; c = d; d = e; e.Update( bar );

  {
    const double x = a.price > b.price ? a.price : b.price;
    const double y = d.price > e.price ? d.price : e.price;
    const double z = x > y ? x : y;
    if ( c.price > z ) m_ceSwingLo.AddLabel( c.dt, c.price, "Swing Lo" );
  }

  {
    const double x = a.price < b.price ? a.price : b.price;
    const double y = d.price < e.price ? d.price : e.price;
    const double z = x < y ? x : y;
    if ( c.price < z ) m_ceSwingHi.AddLabel( c.dt, c.price, "Swing Hi" );
  }
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pWatch ) {
    m_pWatch->SaveSeries( sPrefix );
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
