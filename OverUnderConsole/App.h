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

#include <boost/thread.hpp>  // separate thread for asio run processing
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFTimeSeries/TimeSeries.h>
#include <TFTrading/InstrumentManager.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/OrderManager.h>

#pragma once
class App {
public:
  App(void);
  ~App(void);

  void Run( void );
protected:
private:

  typedef ou::tf::CProviderManager::pProvider_t pProvider_t;
  typedef ou::tf::CInstrumentManager::pInstrument_t pInstrument_t;

  ou::tf::CIBTWS m_tws;
  ou::tf::CIQFeedProvider m_iqfeed;

  boost::thread m_asioThread;
  boost::asio::io_service m_io;
  boost::asio::io_service::work* m_pwork;

  ou::tf::CInstrumentManager& m_mgrInstrument;

  ou::tf::CInstrument::pInstrument_t m_pInstrument;

  void WorkerThread( void ); // worker thread

  void Connected( int i );
  void DisConnected( int i );

  // will need to migrate to a container when doing more than one instrument
  void HandleQuote( const ou::tf::CQuote& quote );
  void HandleTrade( const ou::tf::CTrade& trade );

  void HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details, const pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );
};

