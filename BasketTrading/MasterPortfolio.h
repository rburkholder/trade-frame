/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// TODO: refactor into UnderlyingPortfolio

#include <set>
#include <map>
#include <vector>
#include <string>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treebase.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/BuildInstrument.h>

#include <TFIndicators/Pivots.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>
//#include <TFSimulation/SimulationProvider.h>

#include <TFOptionCombos/SpreadSpecs.h>

#include "Underlying.h"
#include "ManageStrategy.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  class TreeItem;
namespace iqfeed { // IQFeed
  class HistoryRequest;
  class OptionChainQuery;
} // namespace iqfeed
namespace option {
  class Engine;
} // namespace option
} // namespace tf
} // namespace ou

class wxMenu;

class MasterPortfolio {
  friend class boost::serialization::access;
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pPortfolio_t = ou::tf::PortfolioManager::pPortfolio_t;
  using pPosition_t = ou::tf::PortfolioManager::pPosition_t;
  using idPortfolio_t = ou::tf::PortfolioManager::idPortfolio_t;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  using vSymbol_t = std::vector<std::string>;

  using fConstructPositionUnderlying_t = ManageStrategy::fConstructPosition_t;

  using fChartRoot_t = std::function<ou::tf::TreeItem*(const std::string&,pChartDataView_t)>;
  using fSetChartDataView_t = std::function<void(pChartDataView_t)>;

  MasterPortfolio(
    boost::gregorian::date dateTrading // from config file
  , ou::tf::option::SpreadSpecs // from config file
  , vSymbol_t&& vSymbol // from config file? - maybe just send config file?
  , pPortfolio_t pMasterPortfolio
  , pProvider_t pExec, pProvider_t pData1, pProvider_t pData2
  , fChartRoot_t&&
  , fSetChartDataView_t&&
  );
  ~MasterPortfolio();

  void Add( pPortfolio_t ); // from database load
  void Add( pPosition_t );  // from database load

  void Load( ptime dtLatestEod );

  double UpdateChart();

  void ClosePositions( void );
  void SaveSeries( const std::string& sPath );

  void Test( void );

  void TakeProfits();
  void CloseExpiryItm( boost::gregorian::date );
  void CloseFarItm();
  void CloseForProfits();
  void CloseItmLeg();
  void AddCombo( bool bForced );
  void EmitInfo();

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using pOrder_t = ou::tf::Order::pOrder_t;

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  //using pProviderSim_t =  ou::tf::SimulationProvider::pProvider_t;

  using pInstrument_t = ou::tf::ib::TWS::pInstrument_t;

  std::string m_sTSDataStreamStarted;

  bool m_bStarted;

  boost::gregorian::date m_dateTrading;  // for use in DailyTradeTimeFrame
  ou::tf::option::SpreadSpecs m_spread_specs;
  ptime m_dtLatestEod;

  // need to unify this with m_setSymbol;
  vSymbol_t m_vSymbol;

  ou::tf::DatedDatum::volume_t m_nSharesTrading;

  std::thread m_worker;

  pProvider_t m_pExec;
  pProvider_t m_pData1;
  pProvider_t m_pData2;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;
  //pProviderSim_t n_pSim;

  pPortfolio_t m_pMasterPortfolio;

  ou::tf::LiborFromIQFeed m_libor;
  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  ou::tf::TreeItem* m_ptiTreeRoot;
  ou::tf::TreeItem* m_ptiTreeUnderlying;
  ou::tf::TreeItem* m_ptiTreeStrategies;
  //wxTreeItemId m_idTreeOptions;

  using pManageStrategy_t = std::shared_ptr<ManageStrategy>;

  struct Strategy {
    pManageStrategy_t pManageStrategy;
    ou::tf::Price::price_t priceOpen;
    double dblBestProbability;
    bool bChartActivated;
    Strategy( pManageStrategy_t pManageStrategy_ )
    : pManageStrategy( std::move( pManageStrategy_ ) ),
      bChartActivated( false ),
      priceOpen {}, dblBestProbability {}
    {}
    Strategy()
    :
      bChartActivated( false ),
      priceOpen {}, dblBestProbability {}
    {}
    void Set( pManageStrategy_t&& pManageStrategy_ ) { pManageStrategy = std::move( pManageStrategy_ ); }
  };

  struct Statistics {
    ou::tf::PivotSet setPivots;
  };

  using pUnderlying_t = std::unique_ptr<Underlying>;
  using pStrategy_t = std::unique_ptr<Strategy>;
  using mapStrategy_t = std::map<idPortfolio_t,pStrategy_t>;
  using iterStrategy_t = mapStrategy_t::iterator;

  struct UnderlyingWithStrategies {

    pUnderlying_t pUnderlying;
    pStrategy_t pStrategyInWaiting;
    mapStrategy_t mapStrategyActive;
    mapStrategy_t mapStrategyClosed;
    ou::tf::TreeItem* pti;
    Statistics statistics;
    ou::tf::Bars m_barsHistory;

    UnderlyingWithStrategies( pUnderlying_t pUnderlying_ )
    : pUnderlying( std::move( pUnderlying_ ) ) {}
    //UnderlyingWithStrategies( const Statistics&& statistics_ )
    //: statistics( std::move( statistics_ ) ) {}

    UnderlyingWithStrategies( UnderlyingWithStrategies&& rhs )
    : pUnderlying( std::move( rhs.pUnderlying ) )
    {
      assert( rhs.mapStrategyActive.empty() );
      assert( rhs.mapStrategyClosed.empty() );
    }

    ~UnderlyingWithStrategies() {
      pStrategyInWaiting.reset();
      mapStrategyClosed.clear();
      mapStrategyActive.clear();
    }

    void ClosePositions() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->ClosePositions();
      }
    }

    void SaveSeries( const std::string& sPrefix ) {
      pUnderlying->SaveSeries( sPrefix );
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->SaveSeries( sPrefix );
      }
    }
    double EmitInfo() {
      double sum {};
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        sum += pStrategy->pManageStrategy->EmitInfo();
      }
      return sum;
    }
    void CloseForProfits() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->CloseForProfits();
      }
    }
    void TakeProfits() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->TakeProfits();
      }
    }
  };

  using mapUnderlyingWithStrategies_t = std::map<std::string /* sUGenericUnderlying */, UnderlyingWithStrategies>;
  using iterUnderlyingWithStrategies_t = mapUnderlyingWithStrategies_t::iterator;
  mapUnderlyingWithStrategies_t m_mapUnderlyingWithStrategies;

  // cache of portfolios and positions for use when building strategy instances
  using mapPosition_t = std::map<std::string,pPosition_t>;
  using mapPosition_iter = mapPosition_t::iterator;

  using mapPortfolio_t = std::map<idPortfolio_t,pPortfolio_t>;
  using mapPortfolio_iter = mapPortfolio_t::iterator;

  struct StrategyCache {
    // stuff during database load goes here temporarily
    bool m_bAccessed;
    pPortfolio_t m_pPortfolio;  // portfolio for the strategy
    mapPortfolio_t m_mapPortfolio; // sub-portfolios (option combos) -> recursive lookup [or could be a set or vector?]
    mapPosition_t m_mapPosition; // positions associated with portfolio

    StrategyCache( pPortfolio_t pPortfolio )
    : m_bAccessed( false ),
      m_pPortfolio( pPortfolio )
    {}
    StrategyCache( const StrategyCache&& rhs )
    : m_bAccessed( rhs.m_bAccessed ),
      m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
      m_mapPosition( std::move( rhs.m_mapPosition ) )
    {}

    ~StrategyCache() {
      m_mapPosition.clear();
      m_mapPortfolio.clear();
      m_pPortfolio.reset();
    }
  };

  using mapStrategyCache_t = std::map<ou::tf::Portfolio::idPortfolio_t /* owner */, StrategyCache>;
  using mapStrategyCache_iter = mapStrategyCache_t::iterator;
  mapStrategyCache_t m_mapStrategyCache;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  //using mapVolatility_t = std::multimap<double, std::string>; // string is name of instrument
  //mapVolatility_t m_mapVolatility;

  fChartRoot_t m_fChartRoot;
  fSetChartDataView_t m_fSetChartDataView;

  // used by Load
  using setSymbols_t = std::set<std::string>;
  setSymbols_t m_setSymbols;
  setSymbols_t::const_iterator m_iterSymbols;

  //using mapSpecs_t = std::map<std::string,ou::tf::option::SpreadSpecs>;
  //static const mapSpecs_t m_mapSpecs;

  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery; // need to disconnect
  std::unique_ptr<ou::tf::iqfeed::HistoryRequest> m_pHistoryRequest;  // TODO: need to disconnect

  void ProcessSeedList();
  void AddUnderlying( pWatch_t );

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  pManageStrategy_t ConstructStrategy( UnderlyingWithStrategies& uws );
  void StartUnderlying( UnderlyingWithStrategies& );
  void Add_ManageStrategy_ToTree( const idPortfolio_t&, pManageStrategy_t );
  void AddAsActiveStrategy( UnderlyingWithStrategies&, pStrategy_t&&, const idPortfolio_t& idPortfolioStrategy );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(MasterPortfolio, 1)