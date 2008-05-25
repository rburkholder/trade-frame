#include "StdAfx.h"
#include "OrderExecution.h"

COrderExecution::COrderExecution(void) {
}

COrderExecution::COrderExecution( COrder *order, ptime dtExecutionTime, unsigned long nExecutedQuantity, OrderSide::enumOrderSide eOrderSide) :
 m_pOrder( order ), m_dtExecutionTime( dtExecutionTime ), m_nExecutedQuantity( nExecutedQuantity ), m_eOrderSide( eOrderSide )
{
}

COrderExecution::~COrderExecution(void) {
}
