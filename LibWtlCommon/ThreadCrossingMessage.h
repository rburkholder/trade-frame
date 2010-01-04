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

#include <cassert>

//typedef CWinTraits<WS_CHILD, 0>	CBasicWinTraits;

template <typename T>
class CThreadCrossingMessage: public CWindowImpl<CThreadCrossingMessage<T>, CWindow, CNullTraits>
{
public:
  enum enumMessageIds {
    UWM_THREADCROSSINGMESSAGE = WM_APP + 1,
    UWM_THREADCROSSINGMESSAGE_NEXT
  };
  CThreadCrossingMessage(void);
  ~CThreadCrossingMessage(void);

  typedef CWindowImpl<CThreadCrossingMessage<T>, CWindow, CNullTraits> CThreadCrossingMessage_base_t;

protected:
  BEGIN_MSG_MAP_EX(CThreadCrossingMessage<T>)
    MESSAGE_HANDLER( UWM_THREADCROSSINGMESSAGE, HandleThreadCrossingMessage )
  END_MSG_MAP()

  LRESULT HandleThreadCrossingMessage( UINT, WPARAM, LPARAM, BOOL &bHandled );
private:
  HWND m_hThisWindow;
};

template <typename T>
CThreadCrossingMessage<T>::CThreadCrossingMessage( void )
: CThreadCrossingMessage_base_t()
{
  m_hThisWindow = CWindowImpl::Create( NULL, NULL );
  assert( NULL != m_hThisWindow );
}

template <typename T>
CThreadCrossingMessage<T>::~CThreadCrossingMessage( void ) {
  BOOL b = CThreadCrossingMessage_base_t::DestroyWindow();
}

template <typename T>
LRESULT CThreadCrossingMessage<T>::HandleThreadCrossingMessage( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  bHandled = TRUE;
  return 1;
}
