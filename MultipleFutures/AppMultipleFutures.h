/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:   AppMultipleFutures.h
 * Author: raymond@burkholder.net
 * Project: MultipleFutures
 * Created on March 22, 2020, 18:30
 */

#ifndef APPMULTIPLEFUTURES_H
#define APPMULTIPLEFUTURES_H

#include <string>
#include <vector>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <TFTrading/Watch.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Strategy.h"

class wxSplitterWindow;
class wxNotebook;
class wxBookCtrlEvent;

class FrameMain;
class FrameOrderEntry;

class AppMultipleFutures: public wxApp {
  friend class boost::serialization::access;
public:
protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pPosition_t   = ou::tf::Position::pPosition_t;
  using pPortfolio_t  = ou::tf::Portfolio::pPortfolio_t;

  using pProviderIB_t = ou::tf::IBTWS::pProvider_t;

  std::string m_sStateFileName;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  wxSplitterWindow* m_splitLogGraph;
  wxNotebook* m_nbStrategy;

  FrameOrderEntry* m_pFrameOrderEntry;

  pProviderIB_t m_pIB;
  bool m_bIBConnected;

  bool m_bInitialized;

  //wxTimer m_timerGuiRefresh;
  //ou::ChartMaster m_chart;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pWinChartView_t = ou::tf::WinChartView*;
  using pStrategy_t = std::unique_ptr<Strategy>;

  struct Instance {

    pWatch_t m_pWatch; // move this into strategy for start/stop
    pStrategy_t m_pStrategy;
    pWinChartView_t m_pWinChartView;

    Instance( pWatch_t pWatch, pStrategy_t&& pStrategy )
    : m_pWatch( pWatch ), m_pStrategy( std::move( pStrategy ) ),
      m_pWinChartView( nullptr )
    {}
    Instance( Instance&& instance )
    : m_pWatch( instance.m_pWatch ), m_pStrategy( std::move( instance.m_pStrategy ) ),
      m_pWinChartView( instance.m_pWinChartView )
      {}
    ~Instance() {
      if ( nullptr != m_pWinChartView ) {
        m_pWinChartView->SetChartDataView( nullptr );
        m_pWinChartView->Destroy();
        m_pWinChartView = nullptr;
      }
    }
  };

  using vInstance_t = std::vector<Instance>;
  vInstance_t m_vInstance;

  void ConstructInstance(
    boost::uint16_t nSecondsPerBar,
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day );

  void StartWatch();
  void StopWatch();

  void HandleMenuActionEmitSummary();

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleGuiRefresh( wxTimerEvent& );

  void HandleIBConnecting( int );
  void HandleIBConnected( int );
  void HandleIBDisconnecting( int );
  void HandleIBDisconnected( int );
  void HandleIBError( size_t );

  void OnNotebookPageChanging( wxBookCtrlEvent& event );
  void OnNotebookPageChanged( wxBookCtrlEvent& event );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState( bool bSilent = false );
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & m_splitLogGraph->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      int pos;
      ar & pos;
      m_splitLogGraph->SetSashPosition( pos );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppMultipleFutures, 2)
DECLARE_APP(AppMultipleFutures)

#endif /* APPMULTIPLEFUTURES_H */
