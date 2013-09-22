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

#include "StdAfx.h"

#include "Process.h"

#include "Worker.h"

// For IQFeedGetHistory

Worker::Worker( const std::string& sPrefixPath, size_t nDatums ): 
  m_nDatums( nDatums ),
  m_sPrefixPath( sPrefixPath ),
  m_thread( boost::ref( *this ) ) 
{
}

Worker::~Worker(void) {
}

void Worker::operator()( void ) {

  Process process( m_sPrefixPath, m_nDatums );
  process.Start();
}