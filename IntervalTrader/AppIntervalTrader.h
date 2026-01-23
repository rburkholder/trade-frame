/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
/*
 * File:   AppIntervalTrader.h
 * Author: raymond@burkholder.net
 * Project: IntervalTrader
 * Created on October 6, 2019, 1:44 PM
 */

#ifndef APPINTERVALTRADER_H
#define APPINTERVALTRADER_H

#include <thread>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <wx/wx.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include <TFTrading/Portfolio.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "Instance.h"

class AppIntervalTrader: public wxApp {
  friend class boost::serialization::access;
public:
protected:
private:

  using vSymbol_t = std::vector<std::string>;
  using vInstance_t = std::vector<Instance>;

  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  using pProviderIB_t = ou::tf::ib::TWS::pProvider_t;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  std::string m_sStateFileName;
  std::string m_sFieldFiller;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  pProviderIQFeed_t m_pIQFeed;
  bool m_bIQFeedConnected;

  pProviderIB_t m_pIB;
  bool m_bIBConnected;

  vSymbol_t m_vSymbol;

  size_t m_nIntervalSeconds;
  boost::posix_time::ptime m_dtInterval;

  std::thread m_thread;
  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::system_timer> m_ptimerInterval;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork;

  vInstance_t m_vInstance;

  bool m_bPolling;

  pPortfolio_t m_pPortfolio;
  pPosition_t m_pActivePosition;

  void HandleIQFeedConnecting( int );
  void HandleIQFeedConnected( int );
  void HandleIQFeedDisconnecting( int );
  void HandleIQFeedDisconnected( int );
  void HandleIQFeedError( size_t );

  void HandleIBConnecting( int );
  void HandleIBConnected( int );
  void HandleIBDisconnecting( int );
  void HandleIBDisconnected( int );
  void HandleIBError( size_t );

  void StartPoll();

  void HandlePoll( const boost::system::error_code& );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState( bool bSilent = false );
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppIntervalTrader, 1)
DECLARE_APP(AppIntervalTrader)

#endif /* APPINTERVALTRADER_H */

