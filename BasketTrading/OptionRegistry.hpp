/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    OptionRegistry.hpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrader
 * Created: March 4, 2023 19:42:36
 */

#pragma once

#include <functional>
#include <unordered_map>

#include <TFTrading/Position.h>

#include <TFOptions/Option.h>

#include "OptionStatistics.hpp"

namespace ou {
namespace tf {
  class TreeItem;
}
}

class OptionRegistry {
public:

  using pWatch_t     = ou::tf::option::Option::pWatch_t;
  using pOption_t    = ou::tf::option::Option::pOption_t;
  using pPosition_t  = ou::tf::Position::pPosition_t;

  using fStartCalc_t = std::function<void(pOption_t,pWatch_t)>; // option, underlying
  using fStopCalc_t =  std::function<void(pOption_t,pWatch_t)>; // option, underlying

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;
  using fSetChartDataView_t = std::function<void(pChartDataView_t)>;

  OptionRegistry(
      fStartCalc_t&& fStartCalc
    , fStopCalc_t&& fStopCalc
    , fSetChartDataView_t&& fSetChartDataView
    );

  ~OptionRegistry();

  void AssignWatchUnderlying( pWatch_t pWatchUnderlying ) {
    assert( !m_pWatchUnderlying );
    m_pWatchUnderlying = pWatchUnderlying;
  }

  void SetTreeItem( ou::tf::TreeItem* ptiParent ) {
    assert( !m_ptiParent );
    m_ptiParent = ptiParent;
  }

  void Add( pOption_t pOption );
  void Add( pOption_t pOption, const std::string& sLegName );
  void Add( pOption_t pOption, pPosition_t pPosition, const std::string& sLegName );
  void Remove( pOption_t pOption, bool bRemoveStatistics );

  pChartDataView_t ChartDataView( pOption_t );

  void SaveSeries( const std::string& sPrefix );

protected:
private:

  pWatch_t m_pWatchUnderlying;

  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;
  fSetChartDataView_t m_fSetChartDataView;

  ou::tf::TreeItem* m_ptiParent;

  using pOptionStatistics_t = OptionStatistics::pOptionStatistics_t;

  struct RegistryEntry {

    size_t nReference_option;
    pOption_t pOption;

    size_t nReference_stats;
    pOptionStatistics_t pOptionStatistics;

    RegistryEntry( pOption_t pOption_ )
    : nReference_option( 1 ), pOption( std::move( pOption_ ) )
    , nReference_stats {}
    {}
  };

  using mapOption_t = std::unordered_map<std::string,RegistryEntry>; // for m_fStartCalc, m_fStopCalc
  mapOption_t m_mapOption;

  using mapOptionRegistered_t = std::unordered_map<std::string, pOption_t>; // registered in option engine
  mapOptionRegistered_t m_mapOptionRegistered;

  mapOption_t::iterator LookUp( pOption_t pOption );

  pOptionStatistics_t Add_private( pOption_t pOption, const std::string& sLegName );

};
