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

#include <OUCommon/Delegate.h>

#include "TradingEnumerations.h"

#include "KeyTypes.h"
#include "Position.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// has series of positions, Position

// what about PositionCombos -- is composed of recursed portfolios

// set up timer to scan and report on portfolio once a second, or on significant events

// 20130106
//   master portfolio for base currency
//   master portfolio for each other trading currency, summed into base currency master portfolio
//   sub portfolios for subsequent instrument collections under appropriate master portfolio
// monitor delta at each portfolio/sub-portfolio level.  Each level may have different master hedging positions.

class Portfolio {
public:

  friend std::ostream& operator<<( std::ostream& os, const Portfolio& );

  using pPosition_t = Position::pPosition_t;

  using pPortfolio_t = std::shared_ptr<Portfolio>;

  using execution_delegate_t = Position::execution_delegate_t;
  using PositionDelta_delegate_t = Position::PositionDelta_delegate_t;

  using idPosition_t = keytypes::idPosition_t;
  using idPortfolio_t = keytypes::idPortfolio_t;
  using idAccountOwner_t = keytypes::idAccountOwner_t;

  using currency_t = Currency::type;

  enum EPortfolioType { Master=1, CurrencySummary=2, Standard=10, MultiLeggedPosition, Basket, Aggregate };
  // only one Master, can only have AlternateCurrency at next level below
  // AlternateCurrency only at level below Master, can have any combination of lower three Portfolio types
  // Standard can have variety of position types (including multiple sub-portfolios?)
  // MultiLeggedPosition, typically all positions have same underlying
  // Basket, multiple symbol types, typically traded in batch
  // Aggregate, added 20210620 as portfolio of portfolios, might be same as Basket

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "portfolioid", idPortfolio );
      ou::db::Field( a, "accountownerid", idAccountOwner );
      ou::db::Field( a, "ownerid", idOwner );  // portfolio of portfolios for classifying and grouping positions
      ou::db::Field( a, "portfoliotype", ePortfolioType );
      ou::db::Field( a, "active", bActive );
      ou::db::Field( a, "currency", sCurrency );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "commission", dblCommissionsPaid );
      // unrealized is not here as it is a dynamic value, realized is non-dynamic, updated as position changes
    }

    idPortfolio_t idPortfolio;
    idAccountOwner_t idAccountOwner;
    idPortfolio_t idOwner;
    bool bActive;
    EPortfolioType ePortfolioType;
    currency_t sCurrency;
    std::string sDescription;
    double dblRealizedPL; // does not include commissions paid
    double dblCommissionsPaid;

    TableRowDef( void )
      : dblRealizedPL( 0.0 ), dblCommissionsPaid( 0.0 ), bActive( false ), ePortfolioType( Standard ), sCurrency( Currency::Name[ Currency::USD ] ) {};
    TableRowDef ( const TableRowDef& row )
      : idPortfolio( row.idPortfolio ), idAccountOwner( row.idAccountOwner ), idOwner( row.idOwner ), ePortfolioType( row.ePortfolioType ),
      bActive( true ), sCurrency( row.sCurrency ), sDescription( row.sDescription ),
      dblRealizedPL( row.dblRealizedPL ), dblCommissionsPaid( row.dblCommissionsPaid ) {};
//    TableRowDef(  // initializaton of top level portfolio record (portfolio currency master record)
//      const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_, currency_t sCurrency_,
//      const std::string& sDescription_, double dblRealizedPL_, double dblCommissionsPaid_ )
//      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ), bActive( true ), sCurrency( sCurrency_ ), ePortfolioType( Master ),
//        sDescription( sDescription_ ), dblRealizedPL( dblRealizedPL_ ), dblCommissionsPaid( dblCommissionsPaid_ ) {};
    TableRowDef( // initialization of portfolio records, each required portfolio owner id, empty if master portfolio record
      const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_, const idPortfolio_t& idOwner_, EPortfolioType ePortfolioType_,
      currency_t sCurrency_, const std::string& sDescription_, double dblRealizedPL_, double dblCommissionsPaid_ )
      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ), idOwner( idOwner_ ), bActive( true ), ePortfolioType( ePortfolioType_ ),
        sCurrency( sCurrency_ ), sDescription( sDescription_ ), dblRealizedPL( dblRealizedPL_ ), dblCommissionsPaid( dblCommissionsPaid_ ) {};
    TableRowDef( // sub-portfolio with zero'd realized, commission
      const idPortfolio_t& idPortfolio_, const idAccountOwner_t& idAccountOwner_, const idPortfolio_t& idOwner_, EPortfolioType ePortfolioType_,
        currency_t sCurrency_, const std::string& sDescription_ = "" )
      : idPortfolio( idPortfolio_ ), idAccountOwner( idAccountOwner_ ), idOwner( idOwner_ ), bActive( true ), ePortfolioType( ePortfolioType_ ),
        sCurrency( sCurrency_ ), sDescription( sDescription_ ), dblRealizedPL( 0.0 ), dblCommissionsPaid( 0.0 ) {};
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

//  Portfolio( // for use in memory only
//    const idPortfolio_t& idPortfolio, EPortfolioType ePortfolioType,
//    currency_t sCurrency = Currency::Name[ Currency::USD ],
//    const std::string& sDescription = "" );
//  Portfolio( // can be stored to disk, master portfolio currency record
//    const idPortfolio_t& idPortfolio,
//    const idAccountOwner_t& idAccountOwner, currency_t eCurrency,
//    const std::string& sDescription );
  Portfolio( // can be stored to disk, sub-portfolio records
    const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner, EPortfolioType ePortfolioType_,
    currency_t eCurrency, const std::string& sDescription );
  Portfolio( const TableRowDef& row );
  virtual ~Portfolio();

  const idPortfolio_t& Id() { return m_row.idPortfolio; };

  pPosition_t AddPosition( const std::string& sName, pPosition_t pPosition );
  void DeletePosition( const std::string& sName );  // is this a delete, remove, or unlink?
  void RenamePosition( const std::string& sOld, const std::string& sNew );
  pPosition_t GetPosition( const std::string& sName );

  // are std::map references only in order to perform in-memory recalcs
  void AddSubPortfolio( pPortfolio_t& pPortfolio );
  void RemoveSubPortfolio( const idPortfolio_t& idPortfolio );
  //void SetOwnerPortfolio( const idPortfolio_t& idPortfolio, pPortfolio_t& pPortfolio );

  void QueryStats( double& dblUnRealized, double& dblRealized, double& dblCommissionsPaid, double& dblTotal ) const {
    dblTotal  = ( dblUnRealized = m_plCurrent.dblUnRealized );
    dblTotal += ( dblRealized = m_plCurrent.dblRealized );
    dblTotal -= ( dblCommissionsPaid = m_plCurrent.dblCommissionsPaid );
  }
  void AddStats( double& dblUnRealized, double& dblRealized, double& dblCommissionsPaid ) const {
    dblUnRealized += m_plCurrent.dblUnRealized;
    dblRealized += m_plCurrent.dblRealized;
    dblCommissionsPaid += m_plCurrent.dblCommissionsPaid;
  }

  const TableRowDef& GetRow() const { return m_row; };

  void SetActive( bool ); // ie, false, when portfolio is done

  ou::Delegate<const Portfolio&> OnUnRealizedPLUpdate;
  ou::Delegate<const Portfolio&> OnExecutionUpdate;
  ou::Delegate<const Portfolio&> OnCommissionUpdate;

  ou::Delegate<const PositionDelta_delegate_t&> OnExecution;  // < - use by owning portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnCommission;  // < - use by owning portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnUnRealizedPL;/* ( *this, dblPreviousUnRealizedPL, m_row.dblUnRealizedPL ) */  // < - use by portfolio

protected:

private:

  typedef std::map<std::string, pPosition_t> mapPositions_t;
  typedef std::pair<std::string, pPosition_t> mapPositions_pair_t;
  typedef mapPositions_t::iterator mapPositions_iter_t;
  mapPositions_t m_mapPositionsViaUserName;
  mapPositions_t m_mapPositionsViaInstrumentName;

  typedef std::map<idPortfolio_t, pPortfolio_t> mapPortfolios_t;
  typedef std::pair<idPortfolio_t, pPortfolio_t> mapPortfolios_pair_t;
  typedef mapPortfolios_t::iterator mapPortfolios_iter_t;
  mapPortfolios_t m_mapSubPortfolios;

  TableRowDef m_row;

  struct structPL {
    double dblUnRealized;
    double dblRealized;
    double dblCommissionsPaid;
    double dblNet;
    structPL( void ): dblUnRealized( 0.0 ), dblRealized( 0.0 ), dblNet( 0.0 ), dblCommissionsPaid( 0.0 ) {};
    void Zero( void ) { dblUnRealized = dblRealized = dblNet = dblCommissionsPaid = 0.0; };
    void Sum( void ) { dblNet = dblUnRealized + dblRealized - dblCommissionsPaid; };
    bool operator>( const structPL& pl ) const { return  dblNet > pl.dblNet; };
    bool operator<( const structPL& pl ) const { return  dblNet < pl.dblNet; };
  };

  structPL m_plCurrent;
  structPL m_plMax;
  structPL m_plMin;

  void ReCalc( void );  // not used at the moment, may require tuning

  void HandleExecution( const PositionDelta_delegate_t& );
  void HandleCommission( const PositionDelta_delegate_t& );
  void HandleUnRealizedPL( const PositionDelta_delegate_t& );

};

std::ostream& operator<<( std::ostream& os, const Portfolio& );

} // namespace tf
} // namespace ou
