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

#include <map>
#include <set>
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
class wxCheckListBox;
class wxCommandEvent;

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

  wxCheckListBox* m_clbTags;

  using setTag_t = std::set<std::string>;
  using setSymbol_t = std::set<std::string>;
  using mapTagSymbol_t = std::map<std::string, setSymbol_t>;
  mapTagSymbol_t m_mapTagSymbol;

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

    setTag_t m_setTag;

    SymbolInfo()
    : m_bBarsLoaded( false )
    , m_pti( nullptr )
    {
      Init();
    }

    SymbolInfo( setTag_t&& setTag )
    : m_bBarsLoaded( false )
    , m_pti( nullptr )
    , m_setTag( std::move( setTag ) )
    {
      Init();
    }

    SymbolInfo( SymbolInfo&& rhs )
    : m_bBarsLoaded( false )
    , m_pti( rhs.m_pti )
    , m_setTag( std::move( rhs.m_setTag ) )
    {
      Init();
    }

    void Init() {
      m_cePriceBars.SetName( "Daily" );
      m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
      m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );
    }

  };

  // todo: map entries could be stored in TreeItem instead
  using mapSymbolInfo_t = std::unordered_map<std::string,SymbolInfo>;
  mapSymbolInfo_t m_mapSymbolInfo;

  ou::ChartDataView m_cdv;

  void OnFrameMainAutoMove( wxMoveEvent& );

  void AddTag( const std::string& sTag, const std::string& sSymbol );
  void FilterByTag();

  void LoadPanelFinancialChart();
  ou::tf::TreeItem* LoadGroupInfo( const std::string&, ou::tf::TreeItem* );
  void LoadSymbolInfo( const std::string&, ou::tf::TreeItem* );

  void HandleCheckListBoxEvent( wxCommandEvent& );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;

    if ( 3 == version ) {
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

    if ( 4 == version ) {
      ar & m_mapSymbolInfo.size();
      for ( const mapSymbolInfo_t::value_type& vt: m_mapSymbolInfo ) {
        ar & vt.first;
        const setTag_t& setTag( vt.second.m_setTag );
        ar & setTag.size();
        for ( const setTag_t::value_type& item: setTag ) {
          ar & item;
        }
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;

    switch ( version ) {
      case 2:
      case 3:
        {
          bool bLoadGroup;
          ar & bLoadGroup;
          while ( bLoadGroup ) {
            std::string sGroupName;
            ar & sGroupName;
            std::cout << "group: " << sGroupName << std::endl;
            //ou::tf::TreeItem* ptiGroup = LoadGroupInfo( sGroupName, m_ptiRoot );

            if ( 3 <= version ) {
              bool bLoadSymbol;
              ar & bLoadSymbol;
              while ( bLoadSymbol ) {
                std::string sSymbolName;
                ar & sSymbolName;
                std::cout << "symbol: " << sSymbolName << std::endl;

                mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSymbolName );
                assert( m_mapSymbolInfo.end() == iterSymbolInfo ); // symbols are unique across groups

                AddTag( sGroupName, sSymbolName );
                setTag_t setTag;
                setTag.emplace( sGroupName );
                auto result = m_mapSymbolInfo.emplace( sSymbolName, SymbolInfo( std::move( setTag ) ) );
                assert( result.second );

                LoadSymbolInfo( sSymbolName, m_ptiRoot );
                ar & bLoadSymbol;
              }
            }
            ar & bLoadGroup;
          }
        }
        break;
      case 4:
        {
          mapSymbolInfo_t::size_type nSymbolInfo;
          ar & nSymbolInfo;
          while ( 0 != nSymbolInfo ) {
            std::string sSymbol;
            ar & sSymbol;
            setTag_t::size_type nTag;
            ar & nTag;
            setTag_t setTag;
            while ( 0 != nTag ) {
              std::string sTag;
              ar & sTag;
              AddTag( sTag, sSymbol );
              setTag.emplace( std::move( sTag ) );
              --nTag;
            }
            m_mapSymbolInfo.emplace( sSymbol, SymbolInfo( std::move( setTag ) ) );
            LoadSymbolInfo( sSymbol, m_ptiRoot );
            --nSymbolInfo;
          }
        }
        break;
    }

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppBarChart, 4)

DECLARE_APP(AppBarChart)
