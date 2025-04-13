/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    AppBarChart.hpp
 * Author:  raymond@burkholder.net
 * Project: BarChart
 * Created: April 8, 2025 21:29:02
 */

#pragma once

#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <OUCommon/KeyWordMatch.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFIQFeed/BarHistory.h>

#include <TFVuTrading/TreeItem.hpp>

#include <TFBitsNPieces/FrameWork02.hpp>

//#include "Config.hpp"

class FrameMain;

namespace ou {
namespace tf {
  class PanelFinancialChart;
}
}

class AppBarChart:
  public wxApp
, public ou::tf::FrameWork02<AppBarChart>
{
  friend class boost::serialization::access;
  friend ou::tf::FrameWork02<AppBarChart>;
public:
protected:
private:

  //config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelFinancialChart* m_pPanelFinancialChart;

  using pHDF5DataManager_t = std::unique_ptr<ou::tf::HDF5DataManager>;
  pHDF5DataManager_t m_pdm;

  ou::tf::iqfeed::BarHistory::pBarHistory_t m_pBarHistory;

  ou::tf::TreeItem* m_ptiRoot;

  enum EChartSlot { Price, Volume };

  struct SymbolInfo {
    ou::tf::TreeItem* m_pti;
    ou::ChartDataView m_dvChart; // the data, not movable
    ou::ChartEntryBars m_cePriceBars;
    ou::ChartEntryVolume m_ceVolume;

    bool m_bBarsLoaded;

    SymbolInfo(): m_bBarsLoaded( false ) {}
    SymbolInfo( SymbolInfo&& rhs ): m_bBarsLoaded( false ) {} // nothing to do for emplace use case

  };

  // todo: map entries could be stored in TreeItem instead
  using mapSymbolInfo_t = std::unordered_map<std::string,SymbolInfo>;
  mapSymbolInfo_t m_mapSymbolInfo;

  ou::ChartDataView m_cdv;

  void OnFrameMainAutoMove( wxMoveEvent& );

  void LoadPanelFinancialChart();
  ou::tf::TreeItem* LoadGroupInfo( const std::string&, ou::tf::TreeItem* );
  bool LoadSymbolInfo( const std::string&, ou::tf::TreeItem* );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;

    m_ptiRoot->IterateChildren(
      [&ar]( ou::tf::TreeItem* ptiGroup )->bool {
        std::cout << "group: " << ptiGroup->GetText() << std::endl;
        ar & true;
        ar & ptiGroup->GetText();

        ptiGroup->IterateChildren(
          [&ar]( ou::tf::TreeItem* ptiSymbol )->bool {
            std::cout << "symbol: " << ptiSymbol->GetText() << std::endl;
            ar & true;
            ar & ptiSymbol->GetText();
            return true; // continue iterating
          } );
        ar & false;

        return true; // continue iteratiing
      } );
    ar & false;

  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;

    if ( 2 <= version ) {
      bool bLoadGroup;
      ar & bLoadGroup;
      while ( bLoadGroup ) {
        std::string sGroupName;
        ar & sGroupName;
        std::cout << "group: " << sGroupName << std::endl;
        ou::tf::TreeItem* ptiGroup = LoadGroupInfo( sGroupName, m_ptiRoot );

        if ( 3 <= version ) {
          bool bLoadSymbol;
          ar & bLoadSymbol;
          while ( bLoadSymbol ) {
            std::string sSymbolName;
            ar & sSymbolName;
            std::cout << "symbol: " << sSymbolName << std::endl;
            bool bLoaded = LoadSymbolInfo( sSymbolName, ptiGroup );
            ar & bLoadSymbol;
          }
        }
        ar & bLoadGroup;
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppBarChart, 3)

DECLARE_APP(AppBarChart)
