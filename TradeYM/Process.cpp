/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <vector>
#include <string>

#include "Process.h"

CProcess::CProcess(void)
: 
//  CIQFeed<CProcess>(),
  m_tws( "U215226" )
{
  m_tws.Connect();
}

CProcess::~CProcess(void)
{
  m_tws.Disconnect();
}

void CProcess::OnIQFeedConnected( void ) {
  std::vector<std::string> vs;
  vs.push_back( "@YM#" );
//  vs.push_back( "INDU" );
//  vs.push_back( "TICk" );
//  vs.push_back( "TRIN" );
}

void CProcess::OnIQFeedDisConnected( void ) {
}

//void CProcess::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg) {
//}

