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

// Started 2013/09/26

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFBitsNPieces/FrameWork01.h>

#include "Config.hpp"

class FrameMain;

namespace ou {
namespace tf {
  class PanelLogging;
  class PanelChartHdf5;
}
}

class AppHdf5Chart:
  public wxApp, public ou::tf::FrameWork01<AppHdf5Chart> {
    friend class boost::serialization::access;
    friend ou::tf::FrameWork01<AppHdf5Chart>;
public:
protected:
private:

  std::string m_sHdf5FileName;

  config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::PanelChartHdf5* m_pPanelChartHdf5;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void HandleSelectHdf5File( wxCommandEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & m_sHdf5FileName;
    ar & *m_pFrameMain;
    ar & *m_pPanelChartHdf5;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    if ( 3 <= version ) {
      ar & m_sHdf5FileName;
    }
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      ar & *m_pPanelChartHdf5;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppHdf5Chart, 3)
DECLARE_APP(AppHdf5Chart)

