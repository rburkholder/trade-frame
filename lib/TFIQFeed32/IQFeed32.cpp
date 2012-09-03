/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <sstream>

#include "IQFeed32.h"

// http://www.dtn.com/trading.cfm?sidenav=sn_trading&content=pr_nxcore

void __stdcall IQFeedCallBack( int x, int y ) {
  std::stringstream ss;
  ss << "IQFeed Callback" << x << ", " << y;
}

IQFeed32::IQFeed32( void ) {
  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( NULL, "ONE_UNIFIED", "0.11111111", "2.0" );
}

IQFeed32::~IQFeed32( void ) {
  RemoveClientApp( NULL );
}