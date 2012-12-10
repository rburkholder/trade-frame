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

#include "Position.h"

class ManagePortfolio {
public:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  ManagePortfolio(void);
  ~ManagePortfolio(void);

  void AddSymbol( const std::string& sName, const ou::tf::Bar& bar );
  void Start( pProvider_t pExec, pProvider_t pData1, pProvider_t pData2 );
  void SaveSeries( const std::string& sPath );

protected:
private:

  std::string m_sTSDataStreamStarted;

  double m_dblPortfolioCashToTrade;
  double m_dblPortfolioMargin;

  pProvider_t m_pExec;
  pProvider_t m_pData1;
  pProvider_t m_pData2;

  typedef std::map<std::string,Position*> mapPositions_t;
  typedef std::pair<std::string,Position*> mapPositions_pair_t;
  mapPositions_t m_mapPositions;

};

