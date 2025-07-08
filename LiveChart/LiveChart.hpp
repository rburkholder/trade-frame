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
#include <wx/treectrl.h>

#include <OUCommon/Worker.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Config.hpp"
#include "ChartData.hpp"

class AppLiveChart:
  public wxApp, public ou::tf::FrameWork01<AppLiveChart> {
    friend class boost::serialization::access;
    friend ou::tf::FrameWork01<AppLiveChart>;
public:
protected:
private:

  config::Choices m_choices;

  ou::action::Worker m_worker;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::WinChartView* m_pWinChartView;

  ChartData* m_pChartData;

  ou::tf::DBOps m_db;

  ptime m_dtTopOfMinute;

  class CustomItemData: public wxTreeItemData { // wxTreeCtrl node/leaf info
  public:
    enum enumNodeType { Root, Group, Object } m_eNodeType;
    enum enumDatumType { Quotes, Trades, Bars, NoDatum } m_eDatumType;
    CustomItemData( enumNodeType eNodeType, enumDatumType eDatumType )
      : m_eNodeType( eNodeType ), m_eDatumType( eDatumType ) {};
  };
  wxTreeCtrl* m_pHdf5Root;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html
  std::string m_sCurrentPath;  // used while traversing and building tree
  wxTreeItemId m_curTreeItem; // used while traversing and building tree
  CustomItemData::enumDatumType m_eLatestDatumType;  // need this until all timeseries have a signature attribute associated

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateDatabase( void );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void HandleMenuAction0ObtainNewIQFeedSymbolListRemote( void );
  void HandleMenuAction1ObtainNewIQFeedSymbolListLocal( void );
  void HandleMenuAction2LoadIQFeedSymbolList( void );
  void HandleMenuActionInitializeSymbolSet( void );
  void HandleMenuActionSaveValues( void );
  void HandleMenuActionEmitYieldCurve( void );
  void HandleMenuActionLoadTree( void );

  void HandleObtainNewIQFeedSymbolListRemote( void );
  void HandleObtainNewIQFeedSymbolListLocal( void );
  void HandleLoadIQFeedSymbolList( void );

  void HandleSaveValues( void );

  void HandleLoadTreeHdf5Group( const std::string& s1, const std::string& s2 );
  void HandleLoadTreeHdf5Object( const std::string& s1, const std::string& s2 );

  void HandleBuildTreePathParts( const std::string& sPath );

  void HandleTreeEventItemActivated( wxTreeEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    //ar & m_splitPanels->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    //m_splitPanels->SetSashPosition( x );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppLiveChart, 1)
DECLARE_APP(AppLiveChart)
