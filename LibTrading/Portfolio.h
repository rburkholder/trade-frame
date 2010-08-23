/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "boost/shared_ptr.hpp"

#include "Position.h"

// has series of positions, CPosition

// what about PositionCombos?

// set up timer to scan and report on portfolio once a second, or on significant events

class CPortfolio {
public:

  typedef CPosition::pPosition_t pPosition_t;

  typedef boost::shared_ptr<CPortfolio> pPortfolio_t;

  CPortfolio( const std::string &sPortfolioName );
  ~CPortfolio(void);

  void AddPosition( const std::string& sName, pPosition_t pPosition );
  void DeletePosition( const std::string& sName );
  void RenamePosition( const std::string& sOld, const std::string& sNew );
  pPosition_t GetPosition( const std::string& sName );

  void EmitStats( std::stringstream& ss );

protected:
  
private:

  typedef std::map<std::string, pPosition_t> map_t;
  typedef std::pair<std::string, pPosition_t> map_t_pair;
  typedef map_t::iterator iterator;
  map_t m_mapPositionsViaUserName;
  map_t m_mapPositionsViaInstrumentName;

  std::string m_sPortfolioName;

  double m_dblUnRealizedPL;
  double m_dblRealizedPL;
  double m_dblCommissionsPaid;
  double m_dblNetPL;

  void HandleQuote( const CPosition* );
  void HandleTrade( const CPosition* );
  void HandleExecution( const CPosition* );

};
