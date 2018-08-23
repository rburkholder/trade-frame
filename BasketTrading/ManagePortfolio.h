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

#include <string>
#include <map>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

#include "Position.h"

class ManagePortfolio {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::PortfolioManager::pPortfolio_t pPortfolio_t;
  typedef ou::tf::PortfolioManager::pPosition_t pPosition_t;

  ManagePortfolio( void );
  ~ManagePortfolio(void);

  void AddSymbol( const std::string& sName, const ou::tf::Bar& bar, double dblStop );
  void Start( pPortfolio_t pPortfolio, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2 );
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

  pPortfolio_t m_pPortfolio;

  typedef std::map<std::string,ManagePosition*> mapPositions_t;
  typedef std::pair<std::string,ManagePosition*> mapPositions_pair_t;
  typedef mapPositions_t::iterator mapPositions_iter_t;
  mapPositions_t m_mapPositions;

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

};

