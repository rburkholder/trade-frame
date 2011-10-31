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

#include "StatesOfTrading.h"

sc::result StatePreMarket::Handle( const EvQuote& quote ) {  // requires quotes to come before trades.
  InstrumentState& is( context<MachineMarketStates>().data );
  if ( is.bMarketHoursCrossMidnight && is.bDaySession ) { // transit
    is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
    is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
    return transit<StateMarketOpen>();  // late but transit anyway
  }
  else { // test
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketOpen ) {
      is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
      is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
      return transit<StateMarketOpen>();
    }
  }
  return discard_event();
}

sc::result StatePreMarket::Handle( const EvTrade& trade ) {
  InstrumentState& is( context<MachineMarketStates>().data ); 
  if ( is.bMarketHoursCrossMidnight && is.bDaySession ) { // transit
    //return transit<App::StateMarketOpen>();  // late but transit anyway
    return discard_event();  // see if we get a pre-market trade for GC futures, possibly when starting mid market
  }
  else { // test
    if ( trade.Trade().DateTime().time_of_day() >= is.tdMarketOpen ) {
      return discard_event();  // see if we get a pre-market trade for GC futures, possibly when starting mid market
    }
  }
  return discard_event();
}

sc::result StateMarketOpen::Handle( const EvTrade& trade ) {
  InstrumentState& is( context<MachineMarketStates>().data );
  is.dblOpeningTrade = trade.Trade().Trade();
  is.vZeroMarks.push_back( is.dblOpeningTrade );
  std::sort( is.vZeroMarks.begin(), is.vZeroMarks.end() );
  is.iterZeroMark = is.vZeroMarks.begin();
  while ( is.dblOpeningTrade != *is.iterZeroMark ) {
    is.iterZeroMark++;
    if ( is.vZeroMarks.end() == is.iterZeroMark ) 
      throw std::runtime_error( "can't find our zero mark" );
  }
  is.iterNextMark = is.iterZeroMark;  // set next same as zero as don't know which direction next will be
  //return transit<App::StatePreTrading>();
  return transit<StateTrading>();
}

sc::result StatePreTrading::Handle( const EvQuote& quote ) {  // not currently used

  InstrumentState& is( context<MachineMarketStates>().data );

  if ( quote.Quote().DateTime() >= is.dtPreTradingStop ) {
    return transit<StateTrading>();
  }

  return discard_event();
}

sc::result StateCancelOrders::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );
  is.pPosition->CancelOrders();
  return transit<StateCancelOrdersIdle>();
}

sc::result StateCancelOrdersIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdClosePositions ) {
      return transit<StateClosePositions>();
    }
  }
  return discard_event();
}

sc::result StateClosePositions::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );
  is.pPosition->ClosePosition();
  return transit<StateClosePositionsIdle>();
}

sc::result StateClosePositionsIdle::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdAfterMarket ) {
      return transit<StateAfterMarket>();
    }
  }
  return discard_event();
}

sc::result StateAfterMarket::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketClosed ) {
      return transit<StateMarketClosed>();
    }
  }
  return discard_event();
}

sc::result StateMarketClosed::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );
  return discard_event();
}

sc::result StateZeroPosition::Handle( const EvQuote& quote ) {

  InstrumentState& is( context<MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<StateCancelOrders>();
    }
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
  if ( ( 0 < is.statsMed.Slope() ) && ( mid > is.dblOpeningTrade ) && ( 0 < is.statsMed.Accel() ) ) {
  //if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    std::cout << "Zero Position going long" << std::endl;
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<StateLong>();
  }
  else {
    if ( ( 0 > is.statsFast.Slope() ) && ( mid < is.dblOpeningTrade ) && ( 0 > is.statsMed.Accel() ) ) {
    //if ( quote.Quote().Ask() < *is.iterZeroMark ) {
      if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      std::cout << "Zero Position going short" << std::endl;
      // go short
      is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
      return transit<StateShort>();
    }
  }

  return discard_event();
}

sc::result StateLong::Handle( const EvQuote& quote ) {
  InstrumentState& is( context<MachineMarketStates>().data );
  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<StateCancelOrders>();
    }
  }

  if ( is.pPosition->OrdersPending() ) {
    return discard_event();
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  if ( ( 0 > is.statsFast.Slope() ) && ( mid < *is.iterZeroMark ) && ( 0 > is.statsMed.Accel() ) ) {
  //if ( quote.Quote().Ask() < *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
    std::cout << "long going short" << std::endl;
    // go short
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
    return transit<StateShort>();
  }
  else {
    if ( quote.Quote().Ask() >= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << "long crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
      }
    }
  }

  return discard_event();
}

sc::result StateShort::Handle( const EvQuote& quote ) {

  InstrumentState& is( context<MachineMarketStates>().data );

  if ( is.bDaySession ) { // transit
    if ( quote.Quote().DateTime().time_of_day() >= is.tdCancelOrders ) {
      return transit<StateCancelOrders>();
    }
  }

  if ( is.pPosition->OrdersPending() ) {
    return discard_event();
  }

  double mid = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;

  if ( ( 0 < is.statsMed.Slope() ) && ( mid > *is.iterZeroMark ) && ( 0 < is.statsMed.Accel() ) ) {
  //if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    std::cout << "short going long" << std::endl;
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<StateLong>();
  }
  else {
    if ( quote.Quote().Bid() <= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << "short crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      }
    }
  }

  return discard_event();
}

