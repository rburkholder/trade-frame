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

#include <TFIQFeed/MarketSymbol.h>

#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

#include "SymbolSelection.h"
#include "ManageStrategy.h"

class MasterPortfolio {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::PortfolioManager::pPortfolio_t pPortfolio_t;
  typedef ou::tf::PortfolioManager::pPosition_t pPosition_t;

  typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;
  typedef std::function<const trd_t&(const std::string& sIQFeedSymbolName)> fGetTableRowDef_t;
  typedef ManageStrategy::fOptionDefinition_t fOptionDefinition_t;
  typedef ManageStrategy::fGatherOptionDefinitions_t fGatherOptionDefinitions_t;
  typedef ManageStrategy::fConstructPosition_t fConstructPositionUnderlying_t;

  MasterPortfolio(
    pProvider_t pExec, pProvider_t pData1, pProvider_t pData2,
    fGatherOptionDefinitions_t, fGetTableRowDef_t,
    pPortfolio_t pMasterPortfolio );
  ~MasterPortfolio(void);

  void Load( ptime dtLatestEod, bool bAddToList );
  void Start();
  void Stop( void );
  void SaveSeries( const std::string& sPath );

protected:
private:

  typedef ou::tf::Watch::pWatch_t pWatch_t;
  typedef ou::tf::option::Option::pOption_t pOption_t;

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;

  typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;

  std::string m_sTSDataStreamStarted;

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

  typedef std::unique_ptr<ManageStrategy> pManageStrategy_t;

  typedef std::map<std::string,pManageStrategy_t> mapStrategy_t;
  mapStrategy_t m_mapStrategy;

  fGatherOptionDefinitions_t m_fOptionNamesByUnderlying;
  fGetTableRowDef_t m_fGetTableRowDef;

  void AddSymbol( const std::string& sName, const ou::tf::Bar& bar );
};

