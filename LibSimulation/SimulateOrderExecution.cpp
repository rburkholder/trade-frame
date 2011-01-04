/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <LibCommon/TimeSource.h>

#include "SimulateOrderExecution.h"

int CSimulateOrderExecution::m_nExecId( 1000 );

CSimulateOrderExecution::CSimulateOrderExecution(void)
: m_dtQueueDelay( milliseconds( 800 ) ), m_dblCommission( 1.00 ), 
  m_ea( EAQuotes ),
  m_bOrdersQueued( false ),
  m_bCancelsQueued( false ), m_nOrderQuanRemaining( 0 )
{
}

CSimulateOrderExecution::~CSimulateOrderExecution(void) {
}

void CSimulateOrderExecution::NewTrade( const CTrade& trade ) {
  if ( EATrades == m_ea ) {
    CQuote quote( trade.DateTime(), trade.Trade(), trade.Volume(), trade.Trade(), trade.Volume() );
    if ( m_bOrdersQueued || m_bCancelsQueued ) {
      ProcessDelayQueues( quote );
    }
  }
}

void CSimulateOrderExecution::NewQuote( const CQuote& quote ) {
  if ( EAQuotes == m_ea ) {
    if ( m_bOrdersQueued || m_bCancelsQueued ) {
      ProcessDelayQueues( quote );
    }
  }
}

void CSimulateOrderExecution::SubmitOrder( pOrder_t pOrder ) {
  m_lDelayOrder.push_back( pOrder );
  m_bOrdersQueued = true;
}

void CSimulateOrderExecution::CancelOrder( COrder::idOrder_t nOrderId ) {
  structCancelOrder co( ou::CTimeSource::Instance().Internal(), nOrderId );
  m_lDelayCancel.push_back( co );
  m_bCancelsQueued = true;
}

void CSimulateOrderExecution::CalculateCommission( COrder* pOrder, CTrade::tradesize_t quan ) {
  if ( 0 != quan ) {
    if ( NULL != OnCommission ) {
      double dblCommission( 0 );
      switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
        case InstrumentType::ETF:
        case InstrumentType::Stock:
          dblCommission = 0.005 * (double) quan;
          if ( 1.00 > dblCommission ) dblCommission = 1.00;
          break;
        case InstrumentType::Option:
          dblCommission = 0.95 * (double) quan;
          break;
        case InstrumentType::Future:
          break;
        case InstrumentType::Currency:
          break;
      }
      OnCommission( pOrder->GetOrderId(), dblCommission );
    }
  }
}

void CSimulateOrderExecution::ProcessDelayQueues( const CQuote &quote ) {

  // process cancels list
  bool bNoMore = false;
  while ( !bNoMore && !m_lDelayCancel.empty() ) {
    structCancelOrder &co = m_lDelayCancel.front();
    if ( ( co.dtCancellation + m_dtQueueDelay ) < quote.DateTime() ) {
      bNoMore = true;  // havn't waited long enough to simulate cancel submission
    }
    else {
      m_lDelayCancel.pop_front();
      m_bCancelsQueued = !m_lDelayCancel.empty();
      bool bOrderFound = false;
      for ( lDelayOrder_iter_t iter = m_lDelayOrder.begin(); iter != m_lDelayOrder.end(); ++iter ) {
        if ( co.nOrderId == (*iter)->GetOrderId() ) {
          // perform cancellation on in-process order
          if ( NULL != m_pCurrentOrder ) {
            if ( co.nOrderId == m_pCurrentOrder->GetOrderId() ) {
              m_pCurrentOrder.reset();
            }
          }
          if ( NULL != OnOrderCancelled ) {
            CalculateCommission( m_pCurrentOrder.get(), m_nOrderQuanProcessed );
            OnOrderCancelled( co.nOrderId );
          }
          m_lDelayOrder.erase( iter );
          bOrderFound = true;
          break;
        }
      }
      if ( !bOrderFound ) {  // need an event for this, as it could be legitimate crossing execution prior to cancel
        std::cout << "no order found to cancel: " << co.nOrderId << std::endl;
        if ( NULL != OnNoOrderFound ) OnNoOrderFound( co.nOrderId );
      }
    }
  }

  // process orders list
  // only handles first in queue
  // need to build and maintain order book, particularily for handling limit orders
  if ( NULL == m_pCurrentOrder ) {
    m_pCurrentOrder = m_lDelayOrder.front();
    if ( ( m_pCurrentOrder->GetDateTimeOrderSubmitted() + m_dtQueueDelay ) < quote.DateTime() ) {
      m_lDelayOrder.pop_front();
      m_nOrderQuanRemaining = m_pCurrentOrder->GetQuanOrdered();
      m_nOrderQuanProcessed = 0;
      assert( 0 != m_nOrderQuanRemaining );
    }
    else {
      m_pCurrentOrder.reset();
      m_nOrderQuanRemaining = 0;
    }
  }
  if ( NULL != m_pCurrentOrder ) {
    assert( 0 != m_nOrderQuanRemaining );
    if ( ( 0 != quote.AskSize() ) && ( 0 != quote.BidSize() ) ) {
      CTrade::tradesize_t quanAvail;
      double dblPrice;
      OrderSide::enumOrderSide orderSide = m_pCurrentOrder->GetOrderSide();
      switch ( orderSide ) {
        case OrderSide::Buy:
          quanAvail = std::min<CTrade::tradesize_t>( m_nOrderQuanRemaining, quote.AskSize() );
          dblPrice = quote.Ask();
          break;
        case OrderSide::Sell:
          quanAvail = std::min<CTrade::tradesize_t>( m_nOrderQuanRemaining, quote.BidSize() );
          dblPrice = quote.Bid();
          break;
        default:
          throw std::runtime_error( "CSimulateOrderExecution::ProcessDelayQueues unknown order side" );
          break;
      }

      switch ( m_pCurrentOrder->GetOrderType() ) {
        case OrderType::Market: 
          {
          std::string id;
          GetExecId( &id );
          CExecution exec( dblPrice, quanAvail, orderSide, "SIMMkt", id );
          if ( NULL != OnOrderFill ) 
            OnOrderFill( m_pCurrentOrder->GetOrderId(), exec );
          m_nOrderQuanRemaining -= quanAvail;
          m_nOrderQuanProcessed += quanAvail;
          }
          break;
        case OrderType::Limit: {
          // need to handle order book
          double dblLimitOrderPrice = m_pCurrentOrder->GetPrice1();
          assert( 0 < dblLimitOrderPrice );
          switch ( orderSide ) {
            case OrderSide::Buy:
              if ( quote.Ask() <= dblLimitOrderPrice ) {
                std::string id;
                GetExecId( &id );
                CExecution exec( quote.Ask(), quanAvail, orderSide, "SIMLmtBuy", id );
                if ( NULL != OnOrderFill ) 
                  OnOrderFill( m_pCurrentOrder->GetOrderId(), exec );
                m_nOrderQuanRemaining -= quanAvail;
                m_nOrderQuanProcessed += quanAvail;
              }
              break;
            case OrderSide::Sell:
              if ( quote.Bid() >= dblLimitOrderPrice ) {
                std::string id;
                GetExecId( &id );
                CExecution exec( quote.Bid(), quanAvail, orderSide, "SIMLmtSell", id );
                if ( NULL != OnOrderFill ) 
                  OnOrderFill( m_pCurrentOrder->GetOrderId(), exec );
                m_nOrderQuanRemaining -= quanAvail;
                m_nOrderQuanProcessed += quanAvail;
              }
              break;
            default:
              break;
          }
          break;
        }
      }
      if ( 0 == m_nOrderQuanRemaining ) {
        CalculateCommission( m_pCurrentOrder.get(), m_nOrderQuanProcessed );
        m_pCurrentOrder.reset();
        m_bOrdersQueued = !m_lDelayOrder.empty();
      }
    }
  }
}
