#include "StdAfx.h"
#include "SimulateOrderExecution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimulateOrderExecution::CSimulateOrderExecution(void)
: m_dtOrderDelay( milliseconds( 100) ), m_dblCommission( 0 )
{
}

CSimulateOrderExecution::~CSimulateOrderExecution(void) {
}

void CSimulateOrderExecution::NewTrade( const CTrade &trade ) {
  m_dtLatestTradePrice = trade.m_dblTrade;
  m_nLastTradeSize = trade.m_nTradeSize;
  ProcessDelayQueue( trade.m_dt );
}

void CSimulateOrderExecution::NewQuote( const CQuote &quote ) {

}

void CSimulateOrderExecution::SubmitOrder( const COrder &order ) {
}

void CSimulateOrderExecution::CancelOrder( unsigned long nOrderId ) {

}

void CSimulateOrderExecution::ProcessDelayQueue( const ptime &dtMark ) {
  bool bNoMore = false;
  while ( !bNoMore ) {
    COrder *pOrder = m_qDelay.back();
    if ( ( pOrder->GetDateTimeOrderSubmitted() + m_dtOrderDelay ) < dtMark ) {
      bNoMore = true;  // havn't waited long enough to simulate order submission
    }
    else {
      // waited long enough, now process order
      m_qDelay.pop(); // remove the element
      switch ( pOrder->GetOrderType() ) {
        case OrderType::Market:
          break;
        case OrderType::Limit:
          break;
      }
    }
  }
}