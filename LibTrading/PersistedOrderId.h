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
#include "LibBerkeleyDB/KeyValuePair.h"

class CPersistedOrderId :
  public CKeyValuePair {
public:
  CPersistedOrderId(void);
  virtual ~CPersistedOrderId(void);
  unsigned long GetNextOrderId( void ); // may need to worry about multi-threading at some point in time
protected:
  const static unsigned long m_nStartingOrderId = 1000;
private:
};
