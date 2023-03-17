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

#pragma once

// Started 2013/11/30

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/timer.h>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "EventProviderConnected.h"
#include "PanelArmsIndex.h"

class AppArmsIndex:
  public wxApp,
  public ou::tf::FrameWork01<AppArmsIndex>
{
  friend ou::tf::FrameWork01<AppArmsIndex>;
  friend class boost::serialization::access;
public:
protected:
private:

  //typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;
  //typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  bool m_bData1Connected;
  bool m_bExecConnected;
  bool m_bStarted;

  std::string m_sDbName;

  wxTimer m_timerGuiRefresh;

  FrameMain* m_pFrameMain;

  ou::tf::PanelArmsIndex* m_pPanelArmsIndex;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::DBOps m_db;
  std::string m_sStateFileName;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void Start();

  void OnIQFeedConnected( int );

  void OnData1Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnExecDisconnected( int );

  void HandlePopulateDatabase();
  void HandleLoadDatabase();

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

  void HandleGuiRefresh( wxTimerEvent& event );
  void HandleProviderConnected( EventProviderConnected& );

  void SaveState();
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

BOOST_CLASS_VERSION(AppArmsIndex, 1)
DECLARE_APP(AppArmsIndex)

