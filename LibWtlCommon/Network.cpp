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
#include "Network.h"

using boost::asio::ip::tcp;

CNetwork::CNetwork(CAppModule* pModule, const structMessages& messages)
: CGuiThreadImpl<CNetwork>( pModule ), m_Messages( messages )
{
  m_psocket = new tcp::socket( m_io );
}

CNetwork::~CNetwork(void) {
}

BOOL CNetwork::InitializeThread( void ) {

// synchronous
//  socket.connect(server_endpoint);
//boost::system::error_code ec;
//socket.connect(server_endpoint, ec);

// async
// void your_completion_handler(const boost::system::error_code& ec);
//  socket.async_connect(server_endpoint, your_completion_handler);
  // io_service::run()


  BOOL b;
  if ( 0 != m_Messages.msgInitialized ) {
    if ( 0 != m_Messages.hWnd ) {
      b = ::PostMessage( m_Messages.hWnd, m_Messages.msgInitialized, NULL, NULL );
    }
    
  }
  return TRUE;
}

void CNetwork::CleanupThread( DWORD dw ) {

  BOOL b;
  if ( 0 != m_Messages.msgClosed ) {
    if ( 0 != m_Messages.hWnd ) {
      b = ::PostMessage( m_Messages.hWnd, m_Messages.msgClosed, NULL, NULL );
    }
    
  }
}

LRESULT CNetwork::OnOpen( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  const structConnection& pConn = reinterpret_cast<const structConnection&>( wParam ); 

  bHandled = true;
  return 1;
}

LRESULT CNetwork::OnSend( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CNetwork::OnClose( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CNetwork::OnProcessed( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}