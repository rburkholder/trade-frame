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

#include "LibCommon\TimeSource.h"

#include "Execution.h"

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
  m_dtExecutionTimeStamp = m_timesource.Internal();
}

CExecution::~CExecution(void) {
}
