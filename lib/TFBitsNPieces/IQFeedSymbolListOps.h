/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2013/11/14

#pragma once

#include <boost/atomic.hpp>

#include <OUCommon/Worker.h>

#include <TFIQFeed/LoadMktSymbols.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class IQFeedSymbolListOps {
public:

  IQFeedSymbolListOps( ou::tf::iqfeed::InMemoryMktSymbolList& );
  ~IQFeedSymbolListOps(void);

  void ObtainNewIQFeedSymbolListRemote( void );
  void ObtainNewIQFeedSymbolListLocal( void );
  void LoadIQFeedSymbolList( void );
  void SaveSymbolSubset( const std::string& sFileName, const ou::tf::iqfeed::InMemoryMktSymbolList& subset );  // sample
  void LoadSymbolSubset( const std::string& sFileName );  // sample
  void ClearIQFeedSymbolList( void );

protected:
private:

  boost::atomic<int> m_fenceWorker;
  ou::action::Worker m_worker;
  ou::tf::iqfeed::InMemoryMktSymbolList& m_listIQFeedSymbols;

  void WorkerObtainNewIQFeedSymbolListRemote( void );
  void WorkerObtainNewIQFeedSymbolListLocal( void );
  void WorkerLoadIQFeedSymbolList( void );
};

} // namespace tf
} // namespace ou
