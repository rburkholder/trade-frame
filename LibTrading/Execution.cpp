#include "StdAfx.h"
#include "Execution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CExecution::CExecution( 
    unsigned long nOrderId, 
    //unsigned long nExecutionId,
    double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
    const std::string &sExchange, const std::string &sExecutionId )
    : m_nOrderId( nOrderId ), 
    //m_nExecutionId( nExecutionId ), 
    m_dblPrice( dblPrice ), m_nSize( nSize ), m_eOrderSide( eOrderSide ),
    m_sExchange( sExchange ), m_sExecutionId( sExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nSize );
  m_dtExecutionTimeStamp = CTimeSource::Internal();
}

CExecution::~CExecution(void) {
}
