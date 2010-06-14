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

const std::string CPersistedOrderId::m_key = "OrderId";

CPersistedOrderId::CPersistedOrderId(void) 
: CKeyValuePair()
{
}

CPersistedOrderId::~CPersistedOrderId(void) {
}

CPersistedOrderId::OrderId_t CPersistedOrderId::GetNextOrderId() {
  OrderId_t nOrderId;
  try {
    nOrderId = GetUnsignedLong( m_key );
    ++nOrderId;
    Save( m_key, nOrderId );
  }
  catch ( std::out_of_range e ) {
    Save( m_key, m_nStartingOrderId );
    nOrderId = m_nStartingOrderId;
  }
  return nOrderId;
}

CPersistedOrderId::OrderId_t CPersistedOrderId::GetCurrentOrderId() {
  OrderId_t nOrderId;
  try {
    nOrderId = GetUnsignedLong( m_key );
  }
  catch ( std::out_of_range e ) {
    Save( m_key, m_nStartingOrderId );
    nOrderId = m_nStartingOrderId;
  }
  return nOrderId;
}

void CPersistedOrderId::SetNextOrderId( OrderId_t nOrderId ) {
  Save( m_key, nOrderId );
}
