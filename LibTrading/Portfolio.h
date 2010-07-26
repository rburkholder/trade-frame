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

#include "LibCommon/Delegate.h"

#include "Position.h"

// somehow organize 
  // has series of positions, CPosition
  // has series of orders, COrder
  // has series of executions, CExecution

// what about PositionCombos?

class CPortfolio {
public:

  typedef CPosition::pPosition_t pPosition_t;

  typedef boost::shared_ptr<CPortfolio> pPortfolio_t;

  struct structUpdatePortfolioRecord {
    pPosition_t pPosition;
    int nPosition;
    double dblPrice;
    double dblAverageCost;
    structUpdatePortfolioRecord( pPosition_t pPosition_, int nPosition_, double dblPrice_, double dblAverageCost_ )
      : pPosition( pPosition_ ), nPosition( nPosition_ ), dblPrice( dblPrice_ ), dblAverageCost( dblAverageCost_ ) {};
  };

  typedef const structUpdatePortfolioRecord& UpdatePortfolioRecord_t;
  typedef Delegate<UpdatePortfolioRecord_t>::OnMessageHandler UpdatePortfolioRecordHandler_t;

  CPortfolio( const std::string &sPortfolioName );
  ~CPortfolio(void);

  void AddPosition( const std::string& sName, pPosition_t pPosition );
  void DeletePosition( const std::string& sName );
  void RenamePosition( const std::string& sOld, const std::string& sNew );
  pPosition_t GetPosition( const std::string& sName );

  // need an on change event so delta can be recalculated on value or record addition (quote, trade, portfolio record, order, execution )


protected:
  
private:

  typedef std::map<std::string, pPosition_t> map_t;
  typedef std::pair<std::string, pPosition_t> map_t_pair;
  typedef map_t::iterator iterator;
  map_t m_mapPositions;

  std::string m_sPortfolioName;

};
