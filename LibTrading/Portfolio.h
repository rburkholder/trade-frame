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

#include <LibSqlite/sqlite3.h>

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

  static void CreateDbTable( sqlite3* pDb );

protected:
  
private:

  typedef std::map<std::string, pPosition_t> map_t;
  typedef std::pair<std::string, pPosition_t> map_t_pair;
  typedef map_t::iterator iterator;
  map_t m_mapPositionsViaUserName;
  map_t m_mapPositionsViaInstrumentName;

  std::string m_sPortfolioName;

  struct structPL {
    double dblUnRealized;
    double dblRealized;
    double dblCommissionsPaid;
    double dblNet;
    structPL( void ): dblUnRealized( 0 ), dblRealized( 0 ), dblNet( 0 ), dblCommissionsPaid( 0 ) {};
    void Zero( void ) { dblUnRealized = dblRealized = dblNet = dblCommissionsPaid = 0; };
    void Sum( void ) { dblNet = dblUnRealized + dblRealized - dblCommissionsPaid; };
    bool operator>( const structPL& pl ) const { return  dblNet > pl.dblNet; };
    bool operator<( const structPL& pl ) const { return  dblNet < pl.dblNet; };
  };

  structPL m_plCurrent;
  structPL m_plMax;
  structPL m_plMin;

  void ReCalc( void );

  void HandleQuote( const CPosition* );
  void HandleTrade( const CPosition* );
  void HandleExecution( CPosition::execution_delegate_t );
  void HandleCommission( const CPosition* );

};
