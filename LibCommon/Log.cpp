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
#include "Log.h"
#include <ostream>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

int CLog::m_cntInstances = 0;
boost::shared_ptr<CLog::text_sink> CLog::m_pSink;
src::logger CLog::lg;

CLog log_;

CLog::CLog(void) {
  if ( 0 == m_cntInstances ) {
    boost::shared_ptr<text_sink> pSink(new text_sink);
    m_pSink = pSink;
    text_sink::locked_backend_ptr pBackend = m_pSink->locked_backend();
    //shared_ptr<std::ostream> pStream(&std::clog, logging::empty_deleter());
    boost::shared_ptr<std::ostream> pStream(&std::cout, logging::empty_deleter());
    pBackend->add_stream(pStream);
    pBackend->auto_flush(true);

    //shared_ptr< std::ofstream > pStream2(new std::ofstream("sample.log"));
    //assert(pStream2->is_open());
    //pBackend->add_stream(pStream2);

    logging::core::get()->add_sink(m_pSink);

    BOOST_LOG(lg) << "Added Logging Sink";
  }
  ++m_cntInstances;
  BOOST_LOG(lg) << "New Logger " << m_cntInstances;
}

CLog::~CLog(void) {
  assert( 0 != m_cntInstances );
  --m_cntInstances;
  if ( 0 == m_cntInstances ) {
    BOOST_LOG(lg) << "Removed Logging Sink";
    logging::core::get()->remove_sink(m_pSink);
    boost::shared_ptr<text_sink> pSink;
    m_pSink = pSink;  // assign null, implies delete previous content
  }
}
