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

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIQFeed/MarketSymbol.h>

#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

//#include "Sentiment.h"
#include "SymbolSelection.h"
#include "ManageStrategy.h"

class MasterPortfolio {
public:

  enum class EStrategyChart { Root, Active, Info };

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
  using fSupplyStrategyChart_t = std::function<void(EStrategyChart,const std::string&,pChartDataView_t)>;

  MasterPortfolio(
    pProvider_t pExec, pProvider_t pData1, pProvider_t pData2,
    fGatherOptionDefinitions_t, fGetTableRowDef_t, fSupplyStrategyChart_t,
    pPortfolio_t pMasterPortfolio );
  ~MasterPortfolio(void);

  void Add( pPortfolio_t ); // from database load
  void Add( pPosition_t );  // from database load

  void Load( ptime dtLatestEod, bool bAddToList );
  //void GetSentiment( size_t& nUp, size_t& nDown ) const; // TODO: will probably be jitter around 60 second crossing
  void Start();

  void UpdateChart( double dblPLCurrent, double dblPLUnRealized, double dblPLRealized, double dblCommissionPaid );

  void Stop( void );
  void SaveSeries( const std::string& sPath );

  void Test( void );

  void TakeProfits();
  void CloseExpiryItm( boost::gregorian::date );
  void CloseFarItm();
  void CloseForProfits();

protected:
private:

  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef ou::tf::option::Option::pOption_t pOption_t;

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;

  typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;

  std::string m_sTSDataStreamStarted;

  bool m_bStarted;

  double m_dblPortfolioCashToTrade;
  double m_dblPortfolioMargin;
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

  //Sentiment m_sentiment;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  using pManageStrategy_t = std::unique_ptr<ManageStrategy>;

  struct Strategy {
    const IIPivot iip;
    pManageStrategy_t pManageStrategy;
    ou::tf::Price::price_t priceOpen;
    double dblBestProbability;
    pChartDataView_t pChartDataView;
    Strategy( const IIPivot&& iip_, pManageStrategy_t pManageStrategy_, pChartDataView_t& pChartDataView_ )
    : iip( std::move( iip_ ) ), pManageStrategy( std::move( pManageStrategy_ ) ),
      pChartDataView( pChartDataView_ ),
      priceOpen {}, dblBestProbability {}
    {}
    Strategy( const IIPivot&& iip_, pChartDataView_t& pChartDataView_ )
    : iip( std::move( iip_ ) ),
      pChartDataView( pChartDataView_ ),
      priceOpen {}, dblBestProbability {}
    {}
    void Set( pManageStrategy_t&& pManageStrategy_ ) { pManageStrategy = std::move( pManageStrategy_ ); }
//    const Strategy& operator=( const Strategy&& rhs) {
//      iip = std::move( rhs.iip );
//      pManageStrategy = std::move( rhs.pManageStrategy );
//      return *this;
//    }
  };

  using mapStrategy_t = std::map<std::string,Strategy>;
  mapStrategy_t m_mapStrategy;

  //struct Ranking {
  //  std::string sName; // for lookup in m_mapStrategy
  //  IIPivot::Direction direction;
  //  Ranking( const std::string& sName_, IIPivot::Direction direction_ )
  //    : sName( sName_ ), direction( direction_ )
  //  {}
  //};

  // cache of portfolios and positions for use when building strategy instances
  using mapPosition_t = std::map<std::string,pPosition_t>;
  using mapPosition_iter = mapPosition_t::iterator;
  struct StrategyArtifacts {
    // stuff during database load goes here temporarily
    bool m_bAccessed;
    pPortfolio_t m_pPortfolio;  // portfolio for the strategy
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
  mapStrategyArtifacts_iter m_curStrategyArtifacts;  // positions go to 'current' portfolio

  setSymbols_t m_setSymbols;


  //enum class EAllocate { Waiting, Process, Done };

  //EAllocate m_eAllocate;
  //using mapPivotProbability_t = std::multimap<double,Ranking>; // double is probability
  //mapPivotProbability_t m_mapPivotProbability;

  //using mapVolume_t = std::multimap<volume_t, std::string>; // string is name of instrument
  //mapVolume_t m_mapVolumeRanking;

  using mapVolatility_t = std::multimap<double, std::string>; // string is name of instrument
  mapVolatility_t m_mapVolatility;

  fGatherOptionDefinitions_t m_fOptionNamesByUnderlying;
  fGetTableRowDef_t m_fGetTableRowDef;
  fSupplyStrategyChart_t m_fSupplyStrategyChart;

  void AddSymbol( const IIPivot& );
};

