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
 * File:    IntervalSampler.h
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 11:11 AM
 */

#ifndef INTERVALSAMPLER_H
#define INTERVALSAMPLER_H

#include <vector>
#include <thread>
#include <fstream>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <wx/wx.h>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/Provider.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "Capture.h"

class AppIntervalSampler:
  public wxApp
{
  friend class boost::serialization::access;
public:
protected:
private:

  enum class ECollectionMethod { unknown, interval, time } m_eCollectionMethod;
  enum class EDefaultContent   { unknown, filler, stale } m_eDefaultContent;

  std::string m_sStateFileName;
  std::string m_sFieldFiller;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  pProviderIQFeed_t m_pIQFeed;
  bool m_bIQFeedConnected;

  using vSymbol_t = std::vector<std::string>;
  vSymbol_t m_vSymbol;

  //ptime m_dtNextRotation;
  boost::gregorian::date m_dateNextRotation;
  std::ofstream m_out;

  using pCapture_t = std::unique_ptr<Capture>;

  struct Instance {
    std::string m_sInstrument;
    pCapture_t m_pCapture;
    Instance() {
      m_pCapture = std::move( std::make_unique<Capture>() );
    }
  };

  using vInstance_t = std::vector<Instance>;
  vInstance_t m_vInstance;

  //wxTimer m_timerPoller;

  size_t m_nSequence;
  boost::gregorian::date m_dateSequence;

  size_t m_nIntervalSeconds;
  boost::posix_time::ptime m_dtInterval;

  std::thread m_thread;
  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::system_timer> m_ptimerInterval;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork;

  // TODO: convert to map (auto-sorts on insertion)
  //std::string m_sTimeZoneRegion;
  boost::local_time::time_zone_ptr m_ptz;
  using vtdCollectAt_t = std::vector<boost::posix_time::time_duration>;
  vtdCollectAt_t m_vtdCollectAt;

  void CalcNextPollTime();
  void CalcNextPoll();
  void SubmitPoll();

  void HandleIQFeedConnecting( int );
  void HandleIQFeedConnected( int );
  void HandleIQFeedDisconnecting( int );
  void HandleIQFeedDisconnected( int );
  void HandleIQFeedError( size_t );

  void HandlePoll( const boost::system::error_code& );

  void OutputFileOpen( boost::gregorian::date );
  void OutputFileCheck( boost::gregorian::date );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState( bool bSilent = false );
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;

    if ( boost::gregorian::date() == m_dateSequence ) {
      ar & 0;
    }
    else {
      ar & 1;

      int tmp;
      tmp = m_dateSequence.year();
      ar & tmp;
      tmp = m_dateSequence.month();
      ar & tmp;
      tmp = m_dateSequence.day();
      ar & tmp;

      ar & m_nSequence;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      int tmp, year, month, day;

      ar & tmp;
      if ( 1 == tmp ) {
        ar & year;
        ar & month;
        ar & day;
        m_dateSequence = boost::gregorian::date( year, month, day );
        ar & m_nSequence;
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppIntervalSampler, 2)
DECLARE_APP(AppIntervalSampler)

#endif /* INTERVALSAMPLER_H */

