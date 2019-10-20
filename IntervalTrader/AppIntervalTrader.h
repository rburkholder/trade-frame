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
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <wx/wx.h>

#include <TFIQFeed/IQFeedProvider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "Instance.h"

class AppIntervalTrader: public wxApp {
  friend class boost::serialization::access;
public:
protected:
private:

  std::string m_sStateFileName;
  std::string m_sFieldFiller;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  using pProviderIQFeed_t = ou::tf::IQFeedProvider::pProvider_t;

  pProviderIQFeed_t m_pIQFeed;
  bool m_bIQFeedConnected;

  using pProviderIB_t = ou::tf::IBTWS::pProvider_t;

  pProviderIB_t m_pIB;
  bool m_bIBConnected;

  using vSymbol_t = std::vector<std::string>;
  vSymbol_t m_vSymbol;

  size_t m_nIntervalSeconds;
  boost::posix_time::ptime m_dtInterval;

  std::thread m_thread;
  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::deadline_timer> m_ptimerInterval;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork;

  using vInstance_t = std::vector<Instance>;
  vInstance_t m_vInstance;

  bool m_bPolling;

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

