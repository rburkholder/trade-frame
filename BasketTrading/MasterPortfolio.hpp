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
#include <unordered_map>
#include <unordered_set>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/gdicmn.h>
#include <wx/treebase.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/BuildInstrument.hpp>
#include <TFTrading/PortfolioManager.h>

#include <TFIndicators/Pivots.h>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

#include <TFOptionCombos/SpreadSpecs.h>

#include "Underlying.hpp"
#include "ManageStrategy.hpp"
#include "OptionRegistry.hpp"

class wxMenu;
class wxSizer;
class wxWindow;

namespace ou { // One Unified
namespace tf { // TradeFrame
  class TreeItem;
  class FrameControls;
  class PanelComboOrder;
  class ComposeInstrument;
  class PanelFinancialChart;
namespace iqfeed { // IQFeed
  class HistoryRequest;
  class OptionChainQuery;
} // namespace iqfeed
namespace option {
  class Engine;
} // namespace option
} // namespace tf
} // namespace ou

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

  using fSetChartDataView_t = std::function<void(pChartDataView_t)>;

  MasterPortfolio(
    boost::gregorian::date dateTrading // from config file
  , size_t nPeriodWidth
  , size_t nStochasticPeriods
  , ou::tf::option::SpreadSpecs // from config file
  , vSymbol_t&& vSymbol // from config file? - maybe just send config file?
  , pPortfolio_t pMasterPortfolio
  , pProvider_t pExec, pProvider_t pData1, pProvider_t pData2
  , ou::tf::PanelFinancialChart* pPanelFinancialChart
  , wxWindow* pWindowParent
  );
  ~MasterPortfolio();

  static bool m_bFramePanelComboOrder;
  static wxPoint m_pointFramePanelComboOrder;
  static wxSize  m_sizeFramePanelComboOrder;

  void Add( pPortfolio_t ); // from database load
  void Add( pPosition_t );  // from database load

  void Load( ptime dtLatestEod );

  double UpdateChart();

  void ClosePositions();
  void SaveSeries( const std::string& sPath );

  void EmitInfo();
  void EmitIV();

  void TakeProfits();
  void CloseForProfits();
  void AddCombo( bool bForced );

  void Test();

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using pOrder_t = ou::tf::Order::pOrder_t;

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  //using pProviderSim_t =  ou::tf::SimulationProvider::pProvider_t;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  std::string m_sTSDataStreamStarted;

  bool m_bStarted;

  size_t m_nPeriodWidth;
  size_t m_nStochasticPeriods;
  boost::gregorian::date m_dateTrading;  // for use in DailyTradeTimeFrame
  ou::tf::option::SpreadSpecs m_spread_specs;
  ptime m_dtLatestEod;

  // need to unify this with m_setSymbol;
  vSymbol_t m_vSymbol;

  std::shared_ptr<ou::tf::ComposeInstrument> m_pComposeInstrument;

  ou::tf::DatedDatum::volume_t m_nSharesTrading;

  std::thread m_worker;

  pProvider_t m_pExec;
  pProvider_t m_pData1;
  pProvider_t m_pData2;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;
  //pProviderSim_t n_pSim;

  pPortfolio_t m_pMasterPortfolio;

  //ou::tf::LiborFromIQFeed m_libor;
  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  ou::tf::PanelFinancialChart* m_pPanelFinancialChart;
  pChartDataView_t m_pcdvMasterPortfolioPL;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  ou::tf::TreeItem* m_ptiTreeRoot;
  ou::tf::TreeItem* m_ptiTreeUnderlying;
  ou::tf::TreeItem* m_ptiTreeStrategies;
  //wxTreeItemId m_idTreeOptions;

  wxWindow* m_pWindowParent;

  using setOptionsInEngine_t = std::unordered_set<pOption_t>;
  setOptionsInEngine_t m_setOptionsInEngine;

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
    Statistics statistics;
    ou::tf::Bars m_barsHistory;
    std::atomic_uint32_t m_nQuery;
    ou::tf::TreeItem* ptiUnderlying; // may not need this anymore, OptionRegistry has it
    std::shared_ptr<OptionRegistry> m_pOptionRegistry;

    UnderlyingWithStrategies( pUnderlying_t pUnderlying_ )
    : pUnderlying( std::move( pUnderlying_ ) )
    , m_nQuery {}
    , ptiUnderlying( nullptr )
    {}
    //UnderlyingWithStrategies( const Statistics&& statistics_ )
    //: statistics( std::move( statistics_ ) ) {}

    UnderlyingWithStrategies( UnderlyingWithStrategies&& rhs )
    : pUnderlying( std::move( rhs.pUnderlying ) )
    , m_nQuery {}
    {
      assert( rhs.mapStrategyActive.empty() );
      assert( rhs.mapStrategyClosed.empty() );
    }

    ~UnderlyingWithStrategies() {
      pStrategyInWaiting.reset();
      mapStrategyClosed.clear();
      mapStrategyActive.clear();
    }

    void InitiOptionRegistry() {
      m_pOptionRegistry->AssignWatchUnderlying( pUnderlying->GetWatch() );
      m_pOptionRegistry->SetTreeItem( ptiUnderlying ); // TODO: need to validate this
    }

    void ClosePositions() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->ClosePositions();
      }
    }

    void SaveSeries( const std::string& sPrefix ) {
      assert( m_pOptionRegistry );
      pUnderlying->SaveSeries( sPrefix );
      m_pOptionRegistry->SaveSeries( sPrefix );
      //for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
      //  pStrategy_t& pStrategy( vt.second );
      //  pStrategy->pManageStrategy->SaveSeries( sPrefix );
      //}
    }

    double EmitInfo() {
      double sum {};
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        sum += pStrategy->pManageStrategy->EmitInfo();
      }
      return sum;
    }

    void EmitIV() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->EmitIV();
      }
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
  }; // UnderlyingWithStrategies

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

  fSetChartDataView_t m_fSetChartDataView;

  // used by Load
  using setSymbols_t = std::set<std::string>;
  setSymbols_t m_setSymbols;
  setSymbols_t::const_iterator m_iterSymbols;

  //using mapSpecs_t = std::map<std::string,ou::tf::option::SpreadSpecs>;
  //static const mapSpecs_t m_mapSpecs;

  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery;
  std::unique_ptr<ou::tf::iqfeed::HistoryRequest> m_pHistoryRequest;  // TODO: need to disconnect

  using mapOptions_t = std::unordered_map<std::string,pOption_t>; // keyed by sIQFeedName
  mapOptions_t m_mapOptions;

  void ProcessSeedList();
  void AddUnderlying( pWatch_t );

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  void ConstructDefaultStrategy(
    UnderlyingWithStrategies& uws,
    ou::tf::option::LegNote::Algo,
    ou::tf::option::ComboTraits::EMarketDirection
    );
  pManageStrategy_t ConstructStrategy(
     UnderlyingWithStrategies& uws,
     ou::tf::option::LegNote::Algo,
     ou::tf::option::ComboTraits::EMarketDirection
     );
  void StartUnderlying( UnderlyingWithStrategies& );
  void Add_ManageStrategy_ToTree( const idPortfolio_t&, pManageStrategy_t );
  void AddAsActiveStrategy( UnderlyingWithStrategies&, pStrategy_t&&, const idPortfolio_t& idPortfolioStrategy );

  void HandleFramePanelComboOrder_Move( wxMoveEvent& );
  void HandleFramePanelComboOrder_Size( wxSizeEvent& );

  void OnDestroy_PanelFinancialChart( wxWindowDestroyEvent& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(MasterPortfolio, 1)