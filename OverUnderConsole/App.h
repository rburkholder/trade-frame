/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// 2015/11/08 needs substantial rebuilding as it relies on the old berkeley db code
//  which is no longer actively used.  
// The berkelydb stuff has been replaced by the sql and sqlite modules.

#include <vector>
#include <string>

#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <TFTrading/ProviderManager.h>

#include "Operation.h"

class App {
public:

  App(void);
  ~App(void);

  void Run( void );
  void SelectTradeableSymbols( void );

protected:

private:

  typedef ou::tf::ProviderManager::pProvider_t pProvider_t;
  typedef ou::tf::InstrumentManager::pInstrument_t pInstrument_t;

  ou::tf::IBTWS::pProvider_t m_ptws;
  ou::tf::IQFeedProvider::pProvider_t m_piqfeed;

  boost::thread m_asioThread;
  boost::asio::io_service m_io;
  boost::asio::io_service::work* m_pwork;

  std::string m_sTSDataStreamStarted;

  double m_dblPortfolioCashToTrade;
  double m_dblPortfolioMargin;

  ou::tf::InstrumentManager& m_mgrInstrument;

  typedef boost::shared_ptr<Operation> pOperation_t;
  typedef std::vector<pOperation_t> vOperation_t;
  vOperation_t m_vOperation;

  void AppendTradeableSymbol( const Operation::structSymbolInfo& );

  void WorkerThread( void ); // worker thread

  void Connected( int i );
  void DisConnected( int i );

};

