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
    std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " PreMarket A -> Open" << std::endl;
    return transit<StateMarketOpen>();  // late but transit anyway
  }
  else { // test
    std::stringstream s;
    s << quote.Quote().DateTime().time_of_day() << " " << is.tdMarketOpen;
    std::string ss( s.str() );
    if ( quote.Quote().DateTime().time_of_day() >= is.tdMarketOpen ) {
      is.dtPreTradingStop = quote.Quote().DateTime() + is.tdMarketOpenIdle;
      is.dblMidQuoteAtOpen = ( quote.Quote().Ask() + quote.Quote().Bid() ) / 2.0;
      std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " PreMarket B -> Open" << std::endl;
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
  std::cout << trade.Trade().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " Open " << is.dblOpeningTrade << std::endl;
  return transit<StatePreTrading>();
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

  is.vZeroMarks.push_back( mid );
  std::sort( is.vZeroMarks.begin(), is.vZeroMarks.end() );
  is.iterZeroMark = is.vZeroMarks.begin();
  while ( mid != *is.iterZeroMark ) {
    is.iterZeroMark++;
    if ( is.vZeroMarks.end() == is.iterZeroMark ) 
      throw std::runtime_error( "can't find our zero mark" );
  }
  is.iterNextMark = is.iterZeroMark;  // set next same as zero as don't know which direction next will be

  std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() 
    << "Opening Order mid@" << mid << ", slope@" << is.statsMed.Slope() << " ";

  if ( ( 0 < is.statsMed.Slope() ) /*&& ( mid > is.dblOpeningTrade )*/ /*&& ( 0 < is.statsMed.Accel() ) */) {
  //if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    if ( is.vZeroMarks.end() == is.iterNextMark ) is.iterNextMark--;  // how does this effect issues?
    std::cout << "going long" << std::endl;
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<StateLong>();
  }
  else {
    if ( ( 0 > is.statsMed.Slope() ) /*&& ( mid < is.dblOpeningTrade )*/ /* && ( 0 > is.statsMed.Accel() )*/ ) {
    //if ( quote.Quote().Ask() < *is.iterZeroMark ) {
      if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      std::cout << "going short" << std::endl;
      is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
      return transit<StateShort>();
    }
  }

  std::cout << " -- no order, quiescent" << std::endl;
  
  return transit<StateAfterMarket>();
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

  if ( ( 0 > is.statsMed.Slope() ) && ( mid < *is.iterZeroMark ) /*&& ( 0 > is.statsMed.Accel() )*/ ) {
  //if ( quote.Quote().Ask() < *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
    std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " long going short" << std::endl;
    // go short
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, is.nSharesToTrade );
    return transit<StateShort>();
  }
  else {
    if ( quote.Quote().Ask() >= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " long crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
        if ( is.vZeroMarks.end() == is.iterNextMark ) is.iterNextMark--;  // how does this effect issues?
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

  if ( ( 0 < is.statsMed.Slope() ) && ( mid > *is.iterZeroMark ) /*&& ( 0 < is.statsMed.Accel() )*/ ) {
  //if ( quote.Quote().Bid() > *is.iterZeroMark ) {
    is.iterNextMark = is.iterZeroMark;
    if ( is.vZeroMarks.end() != is.iterZeroMark ) is.iterNextMark++;
    if ( is.vZeroMarks.end() == is.iterNextMark ) is.iterNextMark--;  // how does this effect issues?
    std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " short going long" << std::endl;
    // go long
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    is.pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, is.nSharesToTrade );
    return transit<StateLong>();
  }
  else {
    if ( quote.Quote().Bid() <= *is.iterNextMark ) {
      if ( is.iterZeroMark != is.iterNextMark ) {
        is.iterZeroMark = is.iterNextMark;
        std::cout << quote.Quote().DateTime() << ": " << is.pPosition->GetInstrument()->GetInstrumentName() << " short crossing next zero: " << *is.iterZeroMark << std::endl;
        if ( is.vZeroMarks.begin() != is.iterZeroMark ) is.iterNextMark--;
      }
    }
  }

  return discard_event();
}

