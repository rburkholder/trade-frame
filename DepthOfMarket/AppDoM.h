/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    AppDoM.h
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created on October 12, 2021, 23:04
 */

#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFTrading/Watch.h>

#include <TFIQFeed/HistoryRequest.h>

#include <TFIQFeed/Level2/Symbols.hpp>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>

#include <TFIndicators/Pivots.h>

#include "PanelStatistics.hpp"

class AppDoM:
  public wxApp,
  public ou::tf::FrameWork01<AppDoM>
{
  friend ou::tf::FrameWork01<AppDoM>;
  friend class boost::serialization::access;
public:
protected:
private:

  config::Options m_options;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  ou::tf::l2::PanelTrade* m_pPanelTrade;

  int m_cntLoops;
  PanelStatistics::values_t m_valuesStatistics;
  PanelStatistics* m_pPanelStatistics;

  //wxMenu* m_pMenuLoadDays;

  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;  // one per symbol
  ou::tf::iqfeed::HistoryRequest::pHistoryRequest_t m_pHistoryRequest;

  ou::tf::Watch::pWatch_t m_pWatch;

  ou::tf::Bars m_barsHistory;
  ou::tf::PivotSet m_setPivots;

  void EmitMarketMakerMaps();

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

  void OnFundamentals( const ou::tf::Watch::Fundamentals& );
  void OnQuote( const ou::tf::Quote& );
  void OnTrade( const ou::tf::Trade& );

  void LoadDailyHistory();

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

BOOST_CLASS_VERSION(AppDoM, 1)

DECLARE_APP(AppDoM)
