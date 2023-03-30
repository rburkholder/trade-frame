/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    AppAutoTrade.hpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:06
 */

#pragma once

#include <string>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/DBWrapper.h>

#include <TFBitsNPieces/FrameWork01.h>

class Strategy;
class FrameMain;

namespace ou {
namespace tf {
  class PanelLogging;
  class WinChartView;
  class BuildInstrument;
}
}

class AppAutoTrade:
  public wxApp,
  public ou::tf::FrameWork01<AppAutoTrade>
{
  friend ou::tf::FrameWork01<AppAutoTrade>;
  friend class boost::serialization::access;

public:
protected:
private:

  bool m_bConnectedLatch;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;

  std::string m_sSymbol;
  std::string m_sTSDataStreamStarted;
  int m_nTSDataStreamSequence;

  std::unique_ptr<ou::tf::db> m_pdb;

  ou::ChartDataView m_ChartDataView;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrumentBoth;
  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrumentIQFeed;

  std::unique_ptr<Strategy> m_pStrategy;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void HandleMenuActionCloseAndDone();
  void HandleMenuActionSaveValues();

  void ConstructIBInstrument();
  void ConstructSimInstrument();

  void ConfirmProviders();

  void HandleMenuActionSimStart();
  void HandleMenuActionSimStop();
  void HandleMenuActionSimEmitStats();

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

BOOST_CLASS_VERSION(AppAutoTrade, 1)

DECLARE_APP(AppAutoTrade)

