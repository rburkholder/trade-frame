#include "StdAfx.h"
#include "SimulateOrderExecution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CSimulateOrderExecution::m_nExecId = 1000;

CSimulateOrderExecution::CSimulateOrderExecution(void)
: m_dtQueueDelay( milliseconds( 250 ) ), m_dblCommission( 0.01 ), 
  m_pCurrentOrder( NULL ), m_bOrdersQueued( false ),
  m_bCancelsQueued( false ), m_nOrderQuanRemaining( 0 )
{
}

CSimulateOrderExecution::~CSimulateOrderExecution(void) {
}

void CSimulateOrderExecution::NewTrade( CSymbol::trade_t trade ) {
  if ( m_bOrdersQueued || m_bCancelsQueued ) ProcessDelayQueues( trade );
}

void CSimulateOrderExecution::NewQuote( CSymbol::quote_t quote ) {
  // not handled yet.  implement when implementing limit order simulation
}

void CSimulateOrderExecution::SubmitOrder( COrder *pOrder ) {
  m_lDelayOrder.push_back( pOrder );
  m_bOrdersQueued = true;
}

void CSimulateOrderExecution::CancelOrder( COrder::orderid_t nOrderId ) {
  structCancelOrder co( CTimeSource::Internal(), nOrderId );
  m_lDelayCancel.push_back( co );
  m_bCancelsQueued = true;
}

void CSimulateOrderExecution::CalculateCommission( COrder::orderid_t nOrderId, CTrade::tradesize_t quan ) {
  if ( 0 == quan ) {
    if ( NULL != OnCommission ) OnCommission( nOrderId, m_dblCommission * (double) quan );
  }
}

void CSimulateOrderExecution::ProcessDelayQueues( const CTrade &trade ) {

  // process cancels list
  bool bNoMore = false;
  while ( !bNoMore && !m_lDelayCancel.empty() ) {
    structCancelOrder &co = m_lDelayCancel.front();
    if ( ( co.dtCancellation + m_dtQueueDelay ) < trade.m_dt ) {
      bNoMore = true;  // havn't waited long enough to simulate cancel submission
    }
    else {
      m_lDelayCancel.pop_front();
      m_bCancelsQueued = !m_lDelayCancel.empty();
      bool bOrderFound = false;
      for ( std::list<COrder *>::iterator iter = m_lDelayOrder.begin(); iter != m_lDelayOrder.end(); ++iter ) {
        if ( co.nOrderId == (*iter)->GetOrderId() ) {
          // perform cancellation on in-process order
          if ( NULL != m_pCurrentOrder ) {
            if ( co.nOrderId == m_pCurrentOrder->GetOrderId() ) {
              m_pCurrentOrder = NULL;
            }
          }
          if ( NULL != OnOrderCancelled ) {
            CalculateCommission( co.nOrderId, m_nOrderQuanProcessed );
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
    if ( ( m_pCurrentOrder->GetDateTimeOrderSubmitted() + m_dtQueueDelay ) >= trade.m_dt ) {
      m_lDelayOrder.pop_front();
      m_nOrderQuanRemaining = m_pCurrentOrder->GetQuanOrdered();
      m_nOrderQuanProcessed = 0;
      assert( 0 != m_nOrderQuanRemaining );
    }
    else {
      m_pCurrentOrder = NULL;
      m_nOrderQuanRemaining = 0;
    }
  }
  if ( NULL != m_pCurrentOrder ) {
    assert( 0 != m_nOrderQuanRemaining );
    assert( 0 != trade.m_nTradeSize );
    CTrade::tradesize_t quan = min( m_nOrderQuanRemaining, trade.m_nTradeSize );
    switch ( m_pCurrentOrder->GetOrderType() ) {
      case OrderType::Market: 
        {
        std::string id( GetExecId() );
        CExecution exec( m_pCurrentOrder->GetOrderId(), trade.m_dblTrade, quan, m_pCurrentOrder->GetOrderSide(), "SIMMkt", id );
        std::cout << "Exec:  " << m_pCurrentOrder->GetInstrument()->GetSymbolName() << ", " << quan << "@" << trade.m_dblTrade
          << ", " << m_pCurrentOrder->GetOrderSide() << ", Total=" << quan * trade.m_dblTrade << std::endl;
        if ( NULL != OnOrderFill ) OnOrderFill( exec );
        m_nOrderQuanRemaining -= quan;
        m_nOrderQuanProcessed += quan;
        }
        break;
      case OrderType::Limit: {
        // to handle order book
        double price = m_pCurrentOrder->GetPrice1();
        assert( 0 < price );
        switch ( m_pCurrentOrder->GetOrderSide() ) {
          case OrderSide::Buy:
            if ( trade.m_dblTrade < price ) {
              std::string id( GetExecId() );
              CExecution exec( m_pCurrentOrder->GetOrderId(), price, quan, OrderSide::Buy, "SIMLmtBuy", id );
              if ( NULL != OnOrderFill ) OnOrderFill( exec );
              m_nOrderQuanRemaining -= quan;
              m_nOrderQuanProcessed += quan;
            }
            break;
          case OrderSide::Sell:
            if ( trade.m_dblTrade > price ) {
              std::string id( GetExecId() );
              CExecution exec( m_pCurrentOrder->GetOrderId(), price, quan, OrderSide::Sell, "SIMLmtSell", id );
              if ( NULL != OnOrderFill ) OnOrderFill( exec );
              m_nOrderQuanRemaining -= quan;
              m_nOrderQuanProcessed += quan;
            }
            break;
          default:
            cout << "CSimulateOrderExecution::ProcessDelayQueues LimitOrder Unknown side." << std::endl;
            break;
        }
        break;
      }
    }
    if ( 0 == m_nOrderQuanRemaining ) {
      CalculateCommission( m_pCurrentOrder->GetOrderId(), m_nOrderQuanProcessed );
      m_pCurrentOrder = NULL;
      m_bOrdersQueued = !m_lDelayOrder.empty();
    }
  }
}
