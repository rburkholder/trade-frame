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

#pragma once

// For IQFeedGetHistory

#include <string>

#include <boost/thread/thread.hpp>
//#include <boost/noncopyable.hpp>

#include <TFIQFeed/InMemoryMktSymbolList.h>

class Worker {
public:
  Worker( 
    ou::tf::iqfeed::InMemoryMktSymbolList&,
    const std::string& sPrefixPath, size_t nDatums );
  ~Worker(void);
  void operator()( void );
  void Join( void ) { m_thread.join(); };
protected:
private:
  ou::tf::iqfeed::InMemoryMktSymbolList& m_list;
  std::string m_sPrefixPath;
  const size_t m_nDatums;
  boost::thread m_thread;
};

