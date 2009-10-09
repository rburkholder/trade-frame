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

#include "IQ32.H"
#include "IQFeed.h"

#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// http://www.dtn.com/trading.cfm?sidenav=sn_trading&content=pr_nxcore

void __stdcall IQFeedCallBack( int x, int y ) {
  std::stringstream ss;
  ss << "IQFeed Callback" << x << ", " << y;
}

CIQFeed::CIQFeed(CAppModule* pModule) 
: CGuiThreadImpl<CIQFeed>( pModule ), m_pModule( pModule )
{
}

CIQFeed::~CIQFeed(void) {
}

BOOL CIQFeed::InitializeThread( void ) {

  return TRUE;
}

void CIQFeed::CleanupThread( DWORD dw ) {

}

void CIQFeed::Connect( void ) {

  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );

  //CNetwork::structMessages messages( 

}

void CIQFeed::Disconnect( void ) {

  RemoveClientApp( NULL );

}

LRESULT CIQFeed::OnConnInitialized( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnClosed( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnSendDone( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CIQFeed::OnConnError( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
  bHandled = true;
  return 1;
}

