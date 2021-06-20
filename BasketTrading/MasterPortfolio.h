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

#include <map>
#include <string>
#include <thread>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treebase.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIQFeed/MarketSymbol.h>

#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

#include "Underlying.h"
#include "SymbolSelection.h"
#include "ManageStrategy.h"

class MasterPortfolio {
  friend class boost::serialization::access;
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pPortfolio_t =  ou::tf::PortfolioManager::pPortfolio_t;
  using pPosition_t = ou::tf::PortfolioManager::pPosition_t;
  using idPortfolio_t = ou::tf::PortfolioManager::idPortfolio_t;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  using trd_t = ou::tf::iqfeed::MarketSymbol::TableRowDef;
  using fGetTableRowDef_t = std::function<const trd_t&(const std::string& sIQFeedSymbolName)>;
  using fOptionDefinition_t = ManageStrategy::fOptionDefinition_t;
  using fGatherOptionDefinitions_t = ManageStrategy::fGatherOptionDefinitions_t;
  using fConstructPositionUnderlying_t = ManageStrategy::fConstructPosition_t;
  using fChartRoot_t = std::function<wxTreeItemId(const std::string&,pChartDataView_t)>;
  using fChartAdd_t = std::function<wxTreeItemId(wxTreeItemId,const std::string&,pChartDataView_t)>;
  using fChartDel_t = std::function<void(wxTreeItemId)>;

  MasterPortfolio(
    pPortfolio_t pMasterPortfolio,
    pProvider_t pExec, pProvider_t pData1, pProvider_t pData2,
    fGatherOptionDefinitions_t &&,
    fGetTableRowDef_t &&,
    fChartRoot_t&&, fChartAdd_t&&, fChartDel_t&&
    );
  ~MasterPortfolio(void);

  void Add( pPortfolio_t ); // from database load
  void Add( pPosition_t );  // from database load

  void Load( ptime dtLatestEod, bool bAddToList );

  void UpdateChart( double dblPLCurrent, double dblPLUnRealized, double dblPLRealized, double dblCommissionPaid );

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

  using pProviderIBTWS_t = ou::tf::IBTWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::IQFeedProvider::pProvider_t;
  using pProviderSim_t =  ou::tf::SimulationProvider::pProvider_t;

  using pInstrument_t =ou::tf::IBTWS::pInstrument_t;

  std::string m_sTSDataStreamStarted;

  bool m_bStarted;

  ou::tf::DatedDatum::volume_t m_nSharesTrading;

  std::thread m_worker;

  pProvider_t m_pExec;
  pProvider_t m_pData1;
  pProvider_t m_pData2;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;
  pProviderSim_t n_pSim;

  pPortfolio_t m_pMasterPortfolio;

  ou::tf::LiborFromIQFeed m_libor;
  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  wxTreeItemId m_idTreeRoot;
  wxTreeItemId m_idTreeSymbols;
  wxTreeItemId m_idTreeStrategies;

  using pManageStrategy_t = std::unique_ptr<ManageStrategy>;

  struct Strategy {
    wxTreeItemId idTree;
    const IIPivot iip;
    pManageStrategy_t pManageStrategy;
    ou::tf::Price::price_t priceOpen;
    double dblBestProbability;
    bool bChartActivated;
    pChartDataView_t pChartDataView;
    Strategy( const IIPivot&& iip_, pManageStrategy_t pManageStrategy_, pChartDataView_t& pChartDataView_ )
    : iip( std::move( iip_ ) ), pManageStrategy( std::move( pManageStrategy_ ) ),
      bChartActivated( false ), pChartDataView( pChartDataView_ ),
      priceOpen {}, dblBestProbability {}
    {}
    Strategy( const IIPivot&& iip_, pChartDataView_t& pChartDataView_ )
    : iip( std::move( iip_ ) ),
      bChartActivated( false ), pChartDataView( pChartDataView_ ),
      priceOpen {}, dblBestProbability {}
    {}
    void Set( pManageStrategy_t&& pManageStrategy_ ) { pManageStrategy = std::move( pManageStrategy_ ); }
  };

  using mapStrategy_t = std::map<ou::tf::Portfolio::idPortfolio_t,Strategy>;
  mapStrategy_t m_mapStrategy;

  // cache of portfolios and positions for use when building strategy instances
  using mapPosition_t = std::map<std::string,pPosition_t>;
  using mapPosition_iter = mapPosition_t::iterator;
  using mapPortfolio_t = std::map<std::string,pPortfolio_t>;
  using mapPortfolio_iter = mapPortfolio_t::iterator;
  struct StrategyArtifacts {
    // stuff during database load goes here temporarily
    bool m_bAccessed;
    pPortfolio_t m_pPortfolio;  // portfolio for the strategy
    mapPortfolio_t m_mapPortfolio; // sub-portfolios (option combos) -> recursive lookup
    mapPosition_t m_mapPosition; // positions associated with portfolio
    StrategyArtifacts( pPortfolio_t pPortfolio )
    : m_bAccessed( false ),
      m_pPortfolio( pPortfolio )
    {}
    StrategyArtifacts( const StrategyArtifacts&& rhs )
    : m_bAccessed( rhs.m_bAccessed ),
      m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
      m_mapPosition( std::move( rhs.m_mapPosition ) )
    {}
  };

  using mapStrategyArtifacts_t = std::map<ou::tf::Portfolio::idPortfolio_t,StrategyArtifacts>;
  using mapStrategyArtifacts_iter = mapStrategyArtifacts_t::iterator;
  mapStrategyArtifacts_t m_mapStrategyArtifacts;

  setSymbols_t m_setSymbols;

  using mapUnderlying_t = std::map<std::string, Underlying>;
  mapUnderlying_t m_mapUnderlying;

  //using mapVolatility_t = std::multimap<double, std::string>; // string is name of instrument
  //mapVolatility_t m_mapVolatility;

  fGatherOptionDefinitions_t m_fOptionNamesByUnderlying;
  fGetTableRowDef_t m_fGetTableRowDef;
  fChartRoot_t m_fChartRoot;
  fChartAdd_t m_fChartAdd;
  fChartDel_t m_fChartDel;

  void AddUnderlyingSymbol( const IIPivot& ); // ManageStrategy, migrate to multiple

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(MasterPortfolio, 1)