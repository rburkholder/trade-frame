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

#pragma once

#include "boost/shared_ptr.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "TradingEnumerations.h"

class CExecution {
public:

  typedef unsigned long ExecutionId_t;  // used for database updates, need to persist like orderid
  typedef boost::shared_ptr<CExecution> pExecution_t;
  typedef const pExecution_t& pExecution_ref;

  CExecution( 
    double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExecutionId );
  ~CExecution(void);

  double GetPrice( void ) const { return m_dblPrice; };
  unsigned long GetSize( void ) const { return m_nSize; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_eOrderSide; };
  const std::string &GetExchange( void ) const { return m_sExchange; };
  const std::string &GetExecutionId( void ) const { return m_sExecutionId; };

protected:

  double m_dblPrice;  // execution price
  unsigned long m_nSize;  // quantity executed
  OrderSide::enumOrderSide m_eOrderSide;
  ptime m_dtExecutionTimeStamp;
  std::string m_sExchange;
  std::string m_sExecutionId;  // unique execution id supplied by provider

private:
};
