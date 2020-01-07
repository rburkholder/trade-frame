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
 * File:   AppESBracketOrder.h
 * Author: raymond@burkholder.net
 * Project: ESBracketOrder
 * Created on December 28, 2019, 21:44 PM
 */

#ifndef APPESBRACKETORDER_H
#define APPESBRACKETORDER_H

#include "TFTimeSeries/DatedDatum.h"
#include <string>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/wx.h>
#include <wx/splitter.h>

//#include <OUCharting/ChartDataView.h>
//#include <OUCharting/ChartMaster.h>

#include <TFTrading/Watch.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Strategy.h"

class AppESBracketOrder: public wxApp {
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

  pProviderIB_t m_pIB;
  bool m_bIBConnected;

  bool m_bInitialized;

  ou::tf::Watch::pWatch_t m_pWatch;

  wxTimer m_timerGuiRefresh;
  ou::ChartMaster m_chart;
  ou::tf::WinChartView* m_pWinChartView;

  using pStrategy_t = std::unique_ptr<Strategy>;
  pStrategy_t m_pStrategy;

  void StartWatch();
  void StopWatch();

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleGuiRefresh( wxTimerEvent& );

  void HandleIBConnecting( int );
  void HandleIBConnected( int );
  void HandleIBDisconnecting( int );
  void HandleIBDisconnected( int );
  void HandleIBError( size_t );

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
    if ( 2 == version ) {
      int pos;
      ar & pos;
      m_splitLogGraph->SetSashPosition( pos );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppESBracketOrder, 2)
DECLARE_APP(AppESBracketOrder)

#endif /* APPESBRACKETORDER_H */
