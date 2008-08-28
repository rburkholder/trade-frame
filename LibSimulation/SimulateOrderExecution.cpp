#include "StdAfx.h"
#include "SimulateOrderExecution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimulateOrderExecution::CSimulateOrderExecution(void)
: m_dtOrderDelay( milliseconds( 250 ) ), m_dblCommission( 0 )
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

void CSimulateOrderExecution::SubmitOrder( COrder *pOrder ) {
  m_lDelay.push_back( pOrder );
}

void CSimulateOrderExecution::CancelOrder( unsigned long nOrderId ) {
  for ( std::list<COrder *>::iterator iter = m_lDelay.begin(); iter != m_lDelay.end(); ++iter ) {
    if ( nOrderId == (*iter)->GetOrderId() ) {
      // TODO:  perform cancellation and remove from list and exit
      m_lDelay.erase( iter );
      break;
    }
  }
}

void CSimulateOrderExecution::ProcessDelayQueue( const ptime &dtMark ) {
  bool bNoMore = false;
  while ( !bNoMore && ( 0 < m_lDelay.size() ) ) {
    COrder *pOrder = m_lDelay.front();
    if ( ( pOrder->GetDateTimeOrderSubmitted() + m_dtOrderDelay ) < dtMark ) {
      bNoMore = true;  // havn't waited long enough to simulate order submission
    }
    else {
      // waited long enough, now process order
      m_lDelay.pop_front(); // remove the element
      switch ( pOrder->GetOrderType() ) {
        case OrderType::Market:
          break;
        case OrderType::Limit:
          break;
      }
    }
  }
}