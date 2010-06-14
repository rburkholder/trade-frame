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
#include <LibBerkeleyDB/KeyValuePair.h>

// may need to worry about multi-threading at some point in time

class CPersistedOrderId :
  public CKeyValuePair {
public:

  typedef unsigned long OrderId_t;

  CPersistedOrderId(void);
  virtual ~CPersistedOrderId(void);
  void SetNextOrderId( OrderId_t );
  OrderId_t GetNextOrderId( void ); 
  OrderId_t GetCurrentOrderId( void );
protected:
  const static OrderId_t m_nStartingOrderId = 1000;
  static const std::string m_key;
private:
};
