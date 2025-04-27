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

#include <boost/date_time/gregorian/greg_date.hpp>

#include <wx/app.h>
#include <wx/frame.h>

#include <OUCommon/KeyWordMatch.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/BarHistory.h>

#include <TFVuTrading/TreeItem.hpp>

#include <TFBitsNPieces/FrameWork02.hpp>

//#include "Config.hpp"

class wxCheckListBox;
class wxCommandEvent;

class FrameMain;
class PanelSymbolInfo;

namespace ou {
namespace tf {
  class PanelFinancialChart;
  class AcquireFundamentals;
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

  wxFrame* m_pFrameSymbolInfo;
  PanelSymbolInfo* m_pPanelSymbolInfo;

  wxCheckListBox* m_clbTags;

  using setTag_t = std::set<std::string>;
  using setSymbol_t = std::set<std::string>;
  using mapTagSymbol_t = std::map<std::string, setSymbol_t>;
  mapTagSymbol_t m_mapTagSymbol;

  using pHDF5DataManager_t = std::unique_ptr<ou::tf::HDF5DataManager>;
  pHDF5DataManager_t m_pdm;

  ou::tf::iqfeed::BarHistory::pBarHistory_t m_pBarHistory;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;
  pAcquireFundamentals_t m_pAcquireFundamentals;
  pAcquireFundamentals_t m_pAcquireFundamentals_burial;

  ou::tf::TreeItem* m_ptiRoot;

  enum EChartSlot { Price, Volume };

  struct KeyInfo {

    std::string sCompanyName;
    double dblLast;
    double dblRate;
    double dblYield;
    double dblAmount;

    boost::gregorian::date datePayed;
    boost::gregorian::date dateExDividend;

    bool bLoaded;

    KeyInfo()
    : dblLast {}
    , dblRate {}
    , dblYield {}
    , dblAmount {}
    , bLoaded( false )
    {}
  };

  struct SymbolInfo {

    ou::tf::TreeItem* m_pti;
    ou::ChartDataView m_dvChart; // the data, not movable
    ou::ChartEntryBars m_cePriceBars;
    ou::ChartEntryVolume m_ceVolume;

    KeyInfo m_key_info;

    bool m_bBarsLoaded;
    std::string m_sNotes;

    SymbolInfo()
    : m_bBarsLoaded( false )
    , m_pti( nullptr )
    {
      Init();
    }

    SymbolInfo( SymbolInfo&& rhs )
    : m_bBarsLoaded( false )
    , m_pti( rhs.m_pti )
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

  void HandleIQFeedConnected( int );
  void SymbolFundamentals( mapSymbolInfo_t::iterator );

  void AddTag( const std::string& sTag, const std::string& sSymbol );
  void DelTag( const std::string& sTag, const std::string& sSymbol );
  void FilterByTag();

  void LoadPanelFinancialChart();
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

    wxSize size = m_pFrameSymbolInfo->GetSize();
    ar & size.x;
    ar & size.y;

    wxPoint point = m_pFrameSymbolInfo->GetPosition();
    ar & point.x;
    ar & point.y;

    ar & m_mapSymbolInfo.size();
    for ( const mapSymbolInfo_t::value_type& vt: m_mapSymbolInfo ) {
      ar & vt.first;
      ar & vt.second.m_sNotes;
    }

    ar & m_mapTagSymbol.size();
    for ( const mapTagSymbol_t::value_type& vt: m_mapTagSymbol ) {
      ar & vt.first;
      const setSymbol_t& setSymbol( vt.second );
      ar & setSymbol.size();
      for ( const setSymbol_t::value_type& sSymbol: setSymbol ) {
        ar & sSymbol;
      }
    }

  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {

    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;

    if ( 7 <= version ) {
      int x, y;
      ar & x;
      ar & y;
      wxSize size( x, y );
      m_pFrameSymbolInfo->SetSize( size );

      ar & x;
      ar & y;
      wxPoint point( x, y );
      m_pFrameSymbolInfo->SetPosition( point );
    }

    switch ( version ) {
      case 2:
      case 3:
        {
          bool bLoadTag;
          ar & bLoadTag;
          while ( bLoadTag ) {

            std::string sTag;
            ar & sTag;
            std::cout << "tag: " << sTag << std::endl;

            if ( 3 <= version ) {
              bool bLoadSymbol;
              ar & bLoadSymbol;
              while ( bLoadSymbol ) {

                std::string sSymbolName;
                ar & sSymbolName;
                std::cout << "symbol: " << sSymbolName << std::endl;

                mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSymbolName );
                assert( m_mapSymbolInfo.end() == iterSymbolInfo ); // symbols are unique across groups

                AddTag( sTag, sSymbolName );
                auto result = m_mapSymbolInfo.emplace( sSymbolName, SymbolInfo() );
                assert( result.second );

                LoadSymbolInfo( sSymbolName, m_ptiRoot );
                ar & bLoadSymbol;
              }
            }
            ar & bLoadTag;
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
            while ( 0 != nTag ) {
              std::string sTag;
              ar & sTag;
              AddTag( sTag, sSymbol );
              --nTag;
            }
            m_mapSymbolInfo.emplace( sSymbol, SymbolInfo() );
            LoadSymbolInfo( sSymbol, m_ptiRoot );
            --nSymbolInfo;
          }
        }
        break;
      case 5:
      case 6:
      case 7:
        {
          mapSymbolInfo_t::size_type nSymbolInfo;
          ar & nSymbolInfo;
          while ( 0 != nSymbolInfo ) {
            std::string sSymbol;
            ar & sSymbol;
            auto result = m_mapSymbolInfo.emplace( sSymbol, SymbolInfo() );
            assert( result.second );
            if ( 6 <= version ) {
              ar & result.first->second.m_sNotes;
            }
            LoadSymbolInfo( sSymbol, m_ptiRoot );
            --nSymbolInfo;
          }

          mapTagSymbol_t::size_type nTagSymbol;
          ar & nTagSymbol;
          while ( 0 != nTagSymbol ) {
            std::string sTag;
            ar & sTag;
            setSymbol_t setSymbol;
            setSymbol_t::size_type nSymbol;
            ar & nSymbol;
            while ( 0 != nSymbol ) {
              std::string sSymbol;
              ar & sSymbol;
              setSymbol.insert( sSymbol );
              AddTag( sTag, sSymbol );
              --nSymbol;
            }
            m_mapTagSymbol.emplace( sTag, std::move( setSymbol ) );
            --nTagSymbol;
          }
        }
      break;
    }

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppBarChart, 7)

DECLARE_APP(AppBarChart)
