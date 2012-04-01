/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "CrossThreadMerge.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CCrossThreadMerge::CCrossThreadMerge(void) 
: CMergeDatedDatums(), CThreadCrossingMessage<CCrossThreadMerge>()
{
  m_hSignal = CreateEvent( NULL, FALSE, FALSE, "" );
  assert( NULL != m_hSignal );
}

CCrossThreadMerge::~CCrossThreadMerge() {
  if ( NULL != m_hSignal ) {
    CloseHandle( m_hSignal );
    m_hSignal = NULL;
  }
}

void CCrossThreadMerge::ProcessCarrierInBackGround( CMergeCarrierBase *pCarrier ) {
  // happens in background thread
  BOOL b = PostMessage( UWM_CARRIEREVENT, reinterpret_cast<WPARAM>( pCarrier ) );
  assert( b );
  DWORD dw = WaitForSingleObject( m_hSignal, INFINITE);
  assert( WAIT_OBJECT_0 == dw );
}

LRESULT CCrossThreadMerge::ProcessCarrierInForeGround( UINT, WPARAM w, LPARAM l, BOOL &bHandled ) {
  // happens in foreground thread
  CMergeCarrierBase *pCarrier = reinterpret_cast<CMergeCarrierBase *>( w );
  pCarrier->ProcessDatum();  // process the datum in the gui thread
  BOOL b = SetEvent( m_hSignal ); // set after so carrier is available through full sequence
  assert( b );
  bHandled = TRUE;
  return 1;
}

} // namespace tf
} // namespace ou
