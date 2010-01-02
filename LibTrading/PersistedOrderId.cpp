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
#include "PersistedOrderId.h"

CPersistedOrderId::CPersistedOrderId(void) 
: CKeyValuePair()
{
}

CPersistedOrderId::~CPersistedOrderId(void) {
}

unsigned long CPersistedOrderId::GetNextOrderId() {
  static const std::string key = "OrderId";
  unsigned long nOrderId;
  try {
    nOrderId = GetUnsignedLong( key );
    ++nOrderId;
    Save( key, nOrderId );
  }
  catch ( std::out_of_range e ) {
    Save( key, m_nStartingOrderId );
    nOrderId = m_nStartingOrderId;
  }
  return nOrderId;
}
