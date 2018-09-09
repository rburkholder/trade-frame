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
#include <functional>

//#include <TFIQFeed/InMemoryMktSymbolList.h>
#include <TFIQFeed/MarketSymbol.h>

#include <TFOptions/Engine.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>
#include <TFTrading/NoRiskInterestRateSeries.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

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
  typedef ManageStrategy::fConstructOption_t fConstructOption_t;
  typedef ManageStrategy::fConstructPositionUnderlying_t fConstructPositionUnderlying_t;
  typedef ManageStrategy::fConstructPositionOption_t fConstructPositionOption_t;

  MasterPortfolio( fGatherOptionDefinitions_t, fGetTableRowDef_t );
  ~MasterPortfolio(void);

  void AddSymbol( const std::string& sName, const ou::tf::Bar& bar, double dblStop );
  void Start( pPortfolio_t pMasterPortfolio, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2 );
  void Stop( void );
  void SaveSeries( const std::string& sPath );

protected:
private:

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;  

  typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;

  std::string m_sTSDataStreamStarted;

  double m_dblPortfolioCashToTrade;
  double m_dblPortfolioMargin;
  ou::tf::DatedDatum::volume_t m_nSharesTrading;

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

  typedef std::map<std::string,ManageStrategy*> mapStrategy_t; // use a unique_ptr here?
  mapStrategy_t m_mapStrategy;
  
  fGatherOptionDefinitions_t m_fOptionNamesByUnderlying;
  fGetTableRowDef_t m_fGetTableRowDef;
  //fConstructOption_t m_fConstructOption;
  //fConstructPositionUnderlying_t m_fConstructPositionUnderlying;
  //fConstructPositionOption_t m_fConstructPositionOption;

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

};

