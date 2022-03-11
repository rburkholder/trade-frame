/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    TradeLifeTime.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 9, 2022 16:38
 */

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFVuTrading/PanelOrderButtons_structs.h>

#include "TradeLifeTime.h"

TradeLifeTime::TradeLifeTime( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& selectors, Indicators& indicators )
: m_statePosition( EPositionState::InitializeEntry )
, m_pPosition( pPosition )
, m_bWatching( false )
, m_bWatchStop( false )
, m_dblStopTrailDelta( 0.0 )
, m_ceBuySubmit(  indicators.ceBuySubmit )
, m_ceBuyFill(    indicators.ceBuyFill )
, m_ceSellSubmit( indicators.ceSellSubmit )
, m_ceSellFill(   indicators.ceSellFill )
{}

TradeLifeTime::~TradeLifeTime() {
  StopWatch();
  m_pPosition.reset();
}

void TradeLifeTime::HandleOrderCancelled( const ou::tf::Order& order ) {
  std::cout << "order " << order.GetOrderId() << " cancelled" << std::endl;
}

void TradeLifeTime::HandleOrderFilled( const ou::tf::Order& order ) {
  std::cout << "order " << order.GetOrderId() << " filled" << std::endl;
  m_statePosition = EPositionState::EnteredPosition;
}

void TradeLifeTime::StartWatch() {
  if ( !m_bWatching ) {
    m_bWatching = true;
    ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Add( MakeDelegate( this, &TradeLifeTime::HandleQuote ) );
    pWatch->StartWatch();
  }
}

void TradeLifeTime::StopWatch() {
  if ( m_bWatching ) {
    m_bWatching = false;
    ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->StopWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &TradeLifeTime::HandleQuote ) );
  }
}

void TradeLifeTime::ClearOrders() {
}

// see TFTrading/MonitorOrder.cpp
double TradeLifeTime::PriceInterval( double price ) const {
  auto idRule = m_pPosition->GetInstrument()->GetExchangeRule();
  double interval = boost::dynamic_pointer_cast<ou::tf::ib::TWS>( m_pPosition->GetExecutionProvider() )->GetInterval( price, idRule );
  return interval;
}

// see TFTrading/MonitorOrder.cpp
double TradeLifeTime::NormalizePrice( double price ) const {
  double interval = PriceInterval( price );
  return m_pPosition->GetInstrument()->NormalizeOrderPrice( price, interval );
}

void TradeLifeTime::HandleQuote( const ou::tf::Quote& quote ) {
  m_quote = quote;
}

// =====

TradeWithABuy::TradeWithABuy( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& selectors, Indicators& indicators )
: TradeLifeTime( pPosition, selectors, indicators )
{
  std::cout << pPosition->GetInstrument()->GetInstrumentName() << " buying" << std::endl;

  ou::tf::Quote quote( m_pPosition->GetWatch()->LastQuote() ); // probably no quotes yet

  assert( selectors.m_bPositionEntryEnable );
  {
    switch ( selectors.m_ePositionEntryMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Market:
        m_pOrderEntry = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
        m_ceBuySubmit.AddLabel( quote.DateTime(), quote.Midpoint(), "Buy Submit" );
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Limit:
        {
          double price( NormalizePrice( selectors.PositionEntryValue() ) );
          m_pOrderEntry = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, price );
          m_ceBuySubmit.AddLabel( quote.DateTime(), price, "Buy Submit" );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Stoch:
        assert( false ); // need code for this
        break;
    }

    m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithABuy::HandleEntryOrderCancelled ) );
    m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &TradeWithABuy::HandleEntryOrderFilled ) );

  }

  if ( selectors.m_bPositionExitProfitEnable ) {
    switch ( selectors.m_ePositionExitProfitMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute:
        {
          double value = NormalizePrice( selectors.PositionExitProfitValue() );
          m_pOrderProfit = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, value );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Relative:
        {
          double value = NormalizePrice( quote.Midpoint() + selectors.PositionExitProfitValue() );
          m_pOrderProfit = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, value );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch:
        assert( false ); // need code for this
        break;
    }

    m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithABuy::HandleProfitOrderCancelled ) );
    m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &TradeWithABuy::HandleProfitOrderFilled ) );

  }

  if ( selectors.m_bPositionExitStopEnable ) {
    switch ( selectors.m_ePositionExitStopMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::TrailingAbsolute:
        {
          m_dblStopTrailDelta = selectors.PositionExitStopValue();
          m_dblStopCurrent = NormalizePrice( quote.Midpoint() - selectors.PositionExitStopValue() );
          m_pOrderStop = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Stop, ou::tf::OrderSide::Sell, 1, m_dblStopCurrent );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::TrailingPercent:
        {
          assert( false ); // need code for this
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::Stop:
        {
          m_dblStopCurrent = NormalizePrice( selectors.PositionExitStopValue() );
          m_pOrderStop = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Stop, ou::tf::OrderSide::Sell, 1, m_dblStopCurrent );
        }
        break;
    }

    m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithABuy::HandleStopOrderCancelled ) );
    m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &TradeWithABuy::HandleStopOrderFilled ) );

  }

  m_statePosition = EPositionState::EnteringPosition;
  m_pPosition->PlaceOrder( m_pOrderEntry );
  std::cout << "order " << m_pOrderEntry->GetOrderId() << " placed (buy entry)" << std::endl;
  if ( m_bWatchStop ) {
    StartWatch();
  }
}

TradeWithABuy::~TradeWithABuy() {
  if ( m_pOrderEntry ) {
    m_pOrderEntry->OnOrderCancelled.Remove( MakeDelegate( this, &TradeWithABuy::HandleEntryOrderCancelled ) );
    m_pOrderEntry->OnOrderFilled.Remove( MakeDelegate( this, &TradeWithABuy::HandleEntryOrderFilled ) );
    m_pOrderEntry.reset();
  }
  if ( m_pOrderStop ) {
    m_pOrderStop->OnOrderCancelled.Remove( MakeDelegate( this, &TradeWithABuy::HandleStopOrderCancelled ) );
    m_pOrderStop->OnOrderFilled.Remove( MakeDelegate( this, &TradeWithABuy::HandleStopOrderFilled ) );
    m_pOrderStop.reset();
  }
  if ( m_pOrderProfit ) {
    m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithABuy::HandleProfitOrderCancelled ) );
    m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &TradeWithABuy::HandleProfitOrderFilled ) );
    m_pOrderProfit.reset();
  }
}

void TradeWithABuy::HandleQuote( const ou::tf::Quote& quote ) {
  TradeLifeTime::HandleQuote( quote );
  if ( m_bWatchStop ) {
    double stop = NormalizePrice( quote.Midpoint() - m_dblStopTrailDelta );
    if ( stop > m_dblStopCurrent ) {
      m_pOrderStop->SetPrice1( stop );
      m_dblStopCurrent = stop;
      m_pPosition->UpdateOrder( m_pOrderStop );
      m_ceSellSubmit.AddLabel( quote.DateTime(), stop,  "Stop Update" );
    }
  }
}

void TradeWithABuy::HandleEntryOrderCancelled( const ou::tf::Order& order ) {
  StopWatch();
  m_statePosition = EPositionState::Done;
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithABuy::HandleEntryOrderFilled( const ou::tf::Order& order ) {
  m_ceBuyFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Buy Fill" );
  if ( m_pOrderStop ) {
    m_pPosition->PlaceOrder( m_pOrderStop );
    if ( 0.0 < m_dblStopTrailDelta ) m_bWatchStop = true;
    m_ceSellSubmit.AddLabel( m_quote.DateTime(), m_pOrderStop->GetPrice1(), "Stop Submit" );
    std::cout << "order " << m_pOrderStop->GetOrderId() << " placed (buy stop)" << std::endl;
  }
  if ( m_pOrderProfit ) {
    m_pPosition->PlaceOrder( m_pOrderProfit );
    m_ceSellSubmit.AddLabel( m_quote.DateTime(), m_pOrderProfit->GetPrice1(), "Profit Submit" );
    std::cout << "order " << m_pOrderProfit->GetOrderId() << " placed (buy profit)" << std::endl;
  }
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithABuy::HandleProfitOrderCancelled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithABuy::HandleProfitOrderFilled( const ou::tf::Order& order ) {
  if ( m_pOrderStop ) {
    m_pPosition->CancelOrder( m_pOrderStop->GetOrderId() );
  }
  m_ceSellFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Profit Fill" );
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithABuy::HandleStopOrderCancelled( const ou::tf::Order& order ) {
  m_bWatchStop = false;
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithABuy::HandleStopOrderFilled( const ou::tf::Order& order ) {
  m_bWatchStop = false;
  if ( m_pOrderProfit ) {
    m_pPosition->CancelOrder( m_pOrderProfit->GetOrderId() );
  }
  m_ceSellFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Stop Fill" );
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithABuy::Cancel() {
}

void TradeWithABuy::Close() {
}

// =====

TradeWithASell::TradeWithASell( pPosition_t pPosition, const ou::tf::PanelOrderButtons_Order& selectors, Indicators& indicators )
: TradeLifeTime( pPosition, selectors, indicators )
{
  std::cout << pPosition->GetInstrument()->GetInstrumentName() << " selling" << std::endl;

  ou::tf::Quote quote( m_pPosition->GetWatch()->LastQuote() ); // probably no quotes yet

  assert( selectors.m_bPositionEntryEnable );
  {
    switch ( selectors.m_ePositionEntryMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Market:
        m_pOrderEntry = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        m_ceBuySubmit.AddLabel( quote.DateTime(), quote.Midpoint(), "Sell Submit" );
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Limit:
        {
          double price( NormalizePrice( selectors.PositionEntryValue() ) );
          m_pOrderEntry = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, price );
          m_ceBuySubmit.AddLabel( quote.DateTime(), price, "Sell Submit" );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionEntryMethod::Stoch:
        assert( false ); // need code for this
        break;
    }

    m_pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithASell::HandleEntryOrderCancelled ) );
    m_pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &TradeWithASell::HandleEntryOrderFilled ) );

  }

  if ( selectors.m_bPositionExitProfitEnable ) {
    switch ( selectors.m_ePositionExitProfitMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute:
        {
          double value = NormalizePrice( selectors.PositionExitProfitValue() );
          m_pOrderProfit = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, value );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Relative:
        {
          double value = NormalizePrice( quote.Midpoint() - selectors.PositionExitProfitValue() );
          m_pOrderProfit = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, value );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch:
        assert( false ); // need code for this
        break;
    }

    m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithASell::HandleProfitOrderCancelled ) );
    m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &TradeWithASell::HandleProfitOrderFilled ) );

  }

  if ( selectors.m_bPositionExitStopEnable ) {
    switch ( selectors.m_ePositionExitStopMethod ) {
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::TrailingAbsolute:
        {
          m_dblStopTrailDelta = selectors.PositionExitStopValue();
          m_dblStopCurrent = NormalizePrice( quote.Midpoint() + selectors.PositionExitStopValue() );
          m_pOrderStop = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Stop, ou::tf::OrderSide::Buy, 1, m_dblStopCurrent );
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::TrailingPercent:
        {
          assert( false ); // need code for this
        }
        break;
      case ou::tf::PanelOrderButtons_Order::EPositionExitStopMethod::Stop:
        {
          m_dblStopCurrent = NormalizePrice( selectors.PositionExitStopValue() );
          m_pOrderStop = m_pPosition->ConstructOrder(
            ou::tf::OrderType::Stop, ou::tf::OrderSide::Buy, 1, m_dblStopCurrent );
        }
        break;
    }

    m_pOrderStop->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithASell::HandleStopOrderCancelled ) );
    m_pOrderStop->OnOrderFilled.Add( MakeDelegate( this, &TradeWithASell::HandleStopOrderFilled ) );

  }

  m_statePosition = EPositionState::EnteringPosition;
  m_pPosition->PlaceOrder( m_pOrderEntry );
  std::cout << "order " << m_pOrderEntry->GetOrderId() << " placed (sell entry)" << std::endl;
}

TradeWithASell::~TradeWithASell() {
  if ( m_pOrderEntry ) {
    m_pOrderEntry->OnOrderCancelled.Remove( MakeDelegate( this, &TradeWithASell::HandleEntryOrderCancelled ) );
    m_pOrderEntry->OnOrderFilled.Remove( MakeDelegate( this, &TradeWithASell::HandleEntryOrderFilled ) );
    m_pOrderEntry.reset();
  }
  if ( m_pOrderStop ) {
    m_pOrderStop->OnOrderCancelled.Remove( MakeDelegate( this, &TradeWithASell::HandleStopOrderCancelled ) );
    m_pOrderStop->OnOrderFilled.Remove( MakeDelegate( this, &TradeWithASell::HandleStopOrderFilled ) );
    m_pOrderStop.reset();
  }
  if ( m_pOrderProfit ) {
    m_pOrderProfit->OnOrderCancelled.Add( MakeDelegate( this, &TradeWithASell::HandleProfitOrderCancelled ) );
    m_pOrderProfit->OnOrderFilled.Add( MakeDelegate( this, &TradeWithASell::HandleProfitOrderFilled ) );
    m_pOrderProfit.reset();
  }
}

void TradeWithASell::HandleQuote( const ou::tf::Quote& quote ) {
  TradeLifeTime::HandleQuote( quote );
  if ( m_bWatchStop ) {
    double stop = NormalizePrice( quote.Midpoint() + m_dblStopTrailDelta );
    if ( stop < m_dblStopCurrent ) {
      m_pOrderStop->SetPrice1( stop );
      m_dblStopCurrent = stop;
      m_pPosition->UpdateOrder( m_pOrderStop );
      m_ceSellSubmit.AddLabel( quote.DateTime(), stop,  "Stop Update" );
    }
  }
}

void TradeWithASell::HandleEntryOrderCancelled( const ou::tf::Order& order ) {
  StopWatch();
  m_statePosition = EPositionState::Done;
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithASell::HandleEntryOrderFilled( const ou::tf::Order& order ) {
  m_ceBuyFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Sell Fill" );
  if ( m_pOrderStop ) {
    m_pPosition->PlaceOrder( m_pOrderStop );
    if ( 0.0 < m_dblStopTrailDelta ) m_bWatchStop = true;
    m_ceSellSubmit.AddLabel( m_quote.DateTime(), m_pOrderStop->GetPrice1(), "Stop Submit" );
    std::cout << "order " << m_pOrderStop->GetOrderId() << " placed (sell stop)" << std::endl;
  }
  if ( m_pOrderProfit ) {
    m_pPosition->PlaceOrder( m_pOrderProfit );
    m_ceSellSubmit.AddLabel( m_quote.DateTime(), m_pOrderProfit->GetPrice1(), "Profit Submit" );
    std::cout << "order " << m_pOrderProfit->GetOrderId() << " placed (sell profit)" << std::endl;
  }
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithASell::HandleProfitOrderCancelled( const ou::tf::Order& order ) {
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithASell::HandleProfitOrderFilled( const ou::tf::Order& order ) {
  if ( m_pOrderStop ) {
    m_pPosition->CancelOrder( m_pOrderStop->GetOrderId() );
  }
  m_ceSellFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Profit Fill" );
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithASell::HandleStopOrderCancelled( const ou::tf::Order& order ) {
  m_bWatchStop = false;
  TradeLifeTime::HandleOrderCancelled( order );
}

void TradeWithASell::HandleStopOrderFilled( const ou::tf::Order& order ) {
  m_bWatchStop = false;
  if ( m_pOrderProfit ) {
    m_pPosition->CancelOrder( m_pOrderProfit->GetOrderId() );
  }
  m_ceSellFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Stop Fill" );
  TradeLifeTime::HandleOrderFilled( order );
}

void TradeWithASell::Cancel() {
}

void TradeWithASell::Close() {
}

