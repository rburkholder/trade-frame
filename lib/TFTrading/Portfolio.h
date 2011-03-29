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

#include <boost/shared_ptr.hpp>

#include "KeyTypes.h"
#include "Position.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// has series of positions, CPosition

// what about PositionCombos?

// set up timer to scan and report on portfolio once a second, or on significant events

class CPortfolio {
public:

  typedef CPosition::pPosition_t pPosition_t;

  typedef boost::shared_ptr<CPortfolio> pPortfolio_t;

  typedef keytypes::idPortfolio_t idPortfolio_t;
  typedef keytypes::idAccountOwner_t idAccountOwner_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "portfolioid", idPortfolio );
      ou::db::Field( a, "accountownerid", idAccountOwner );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "commissionspaid", dblCommissionsPaid );
    }

    idPortfolio_t idPortfolio;
    idAccountOwner_t idAccountOwner;
    std::string sDescription;
    double dblRealizedPL;
    double dblCommissionsPaid;

    TableRowDef( void ) : dblRealizedPL( 0.0 ), dblCommissionsPaid( 0.0 ) {};
    TableRowDef ( const TableRowDef& row ) 
      : idPortfolio( row.idPortfolio ), idAccountOwner( row.idAccountOwner ), sDescription( row.sDescription ),
      dblRealizedPL( row.dblRealizedPL ), dblCommissionsPaid( row.dblCommissionsPaid ) {};
    TableRowDef(
      const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_,
      const std::string& sDescription_, double dblRealizedPL_, double dblCommissionsPaid_ )
      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ),
        sDescription( sDescription_ ), dblRealizedPL( dblRealizedPL_ ), dblCommissionsPaid( dblCommissionsPaid_ ) {};
    TableRowDef( const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_, const std::string& sDescription_ )
      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ), 
        sDescription( sDescription_ ),
        dblRealizedPL( 0.0 ), dblCommissionsPaid( 0.0 ) {};
    TableRowDef( const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_ )
      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ), 
        dblRealizedPL( 0.0 ), dblCommissionsPaid( 0.0 ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "portfolioid" );
      ou::db::Constraint( a, "accountownerid", tablenames::sAccountOwner, "accountownerid" );
      // "create index idx_portfolio_accountid on portfolios( accountid );
    }
  };

  CPortfolio( // for use in memory only
    const idPortfolio_t& idPortfolio, 
    const std::string& sDescription = "" );
  CPortfolio( // can be stored to disk
    const idPortfolio_t& idPortfolio, 
    const idAccountOwner_t& idAccountOwner, 
    const std::string& sDescription );
  CPortfolio( const TableRowDef& row );
  ~CPortfolio(void);

  void AddPosition( const std::string& sName, pPosition_t pPosition );
  void DeletePosition( const std::string& sName );
  void RenamePosition( const std::string& sOld, const std::string& sNew );
  pPosition_t GetPosition( const std::string& sName );

  void EmitStats( std::stringstream& ss );

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
  
private:

  typedef std::map<std::string, pPosition_t> map_t;
  typedef std::pair<std::string, pPosition_t> map_t_pair;
  typedef map_t::iterator iterator;
  map_t m_mapPositionsViaUserName;
  map_t m_mapPositionsViaInstrumentName;

  bool m_bCanUseDb;

  TableRowDef m_row;

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

} // namespace tf
} // namespace ou
