/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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
#include <vector>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include <OUCommon/Delegate.h>

#include "TradingEnumerations.h"
#include "KeyTypes.h"

#include "ProviderInterface.h"
#include "Order.h"
#include "Instrument.h"

// Multiple position records grouped together would be a multi-legged instrument, aka Combo
//   -- not sure how to construct this yet
//    -- a position consists of one or more legs, so a leg would be the atomic unit of composition
// once a position with multiple legs is in a portfolio, should be able to sum up the legs to see if rebalancing or adjustments are required
// A Portfolio should be a collection of position records, whether individual positions, or Combos
// check that orders for both sell side and buy side are not opened simultaneously
// a position is provider dependent, ie, only one provider per position
// Create Delegates so trade and market data updates propogate to combo and portfolio

// todo:  there is nothing for setting a portfolio id

namespace ou { // One Unified
namespace tf { // TradeFrame

class CPosition {
public:

  typedef boost::shared_ptr<CPosition> pPosition_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;

  typedef COrder::idOrder_t idOrder_t;
  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::pOrder_ref pOrder_ref;

  typedef std::pair<const CPosition&, const CExecution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idPortfolio_t idPortfolio_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idInstrument_t idInstrument_t;

  struct TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      
      ou::db::Field( a, "portfolioid", idPortfolio );
      ou::db::Field( a, "name", sName );
      ou::db::Field( a, "notes", sNotes );
      ou::db::Field( a, "executionaccountid", idExecutionAccount );
      ou::db::Field( a, "dataaccountid", idDataAccount );
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "algorithm", sAlgorithm );
      ou::db::Field( a, "ordersidepending", eOrderSidePending );
      ou::db::Field( a, "quantitypending", nPositionPending );
      ou::db::Field( a, "ordersideactive", eOrderSideActive );
      ou::db::Field( a, "quantityactive", nPositionActive );
      ou::db::Field( a, "constructedvalue", dblConstructedValue );
      ou::db::Field( a, "marketvalue", dblMarketValue );
      ou::db::Field( a, "unrealizedpl", dblUnRealizedPL );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "commission", dblCommissionPaid );
    }

    idPortfolio_t idPortfolio;
    std::string sName;
    std::string sNotes;
    ou::tf::keytypes::idAccount_t idExecutionAccount;
    ou::tf::keytypes::idAccount_t idDataAccount;
    ou::tf::keytypes::idInstrument_t idInstrument;
    std::string sAlgorithm;
  // all pending orders must be on the same side
  // pending orders need to cancelled in order to change sides
  // use an opposing position if playing both sides of the market
    OrderSide::enumOrderSide eOrderSidePending;  
    boost::uint32_t nPositionPending;
  // indicates whether we are in a long or short position
    OrderSide::enumOrderSide eOrderSideActive;
    boost::uint32_t nPositionActive;
  // following value markers exclude commission
    double dblConstructedValue;  // based upon position trades  used for RealizedPL calcs, keeps accrueing
    double dblMarketValue;  // based upon market quotes
  // following value markers exclude commission
    double dblUnRealizedPL;  // based upon market quotes
    double dblRealizedPL;  // based upon position trades
  // contains total commissions
    double dblCommissionPaid;

    // account and instrument objects need to be manually asssigned in a second step
    TableRowDefNoKey( void ) 
      : eOrderSidePending( OrderSide::Unknown ), eOrderSideActive( OrderSide::Unknown ), 
      nPositionPending( 0 ), nPositionActive( 0 ), dblConstructedValue( 0.0 ), dblMarketValue( 0.0 ),
      dblUnRealizedPL( 0.0 ), dblRealizedPL( 0.0 ), dblCommissionPaid( 0.0 ) {};
    TableRowDefNoKey( const TableRowDefNoKey& row ) 
      : idPortfolio( row.idPortfolio ), sName( row.sName ), sNotes( row.sNotes ), 
      idExecutionAccount( row.idExecutionAccount ), idDataAccount( row.idDataAccount ), idInstrument( row.idInstrument ),
      sAlgorithm( row.sAlgorithm ), eOrderSidePending( row.eOrderSidePending ), nPositionPending( row.nPositionPending ), 
      eOrderSideActive( row.eOrderSideActive ), nPositionActive( row.nPositionActive ), 
      dblConstructedValue( row.dblConstructedValue ), dblMarketValue( row.dblMarketValue ), 
      dblUnRealizedPL( row.dblUnRealizedPL ), dblRealizedPL( row.dblRealizedPL ), dblCommissionPaid( row.dblCommissionPaid ) {};
    TableRowDefNoKey( const idPortfolio_t& idPortfolio_, const std::string& sName_, const idInstrument_t& idInstrument_,
      const idAccount_t& idExecutionAccount_, const idAccount_t& idDataAccount_, const std::string& sAlgorithm_ ) 
      : idPortfolio( idPortfolio_ ), sName( sName_ ), idInstrument( idInstrument_ ), sAlgorithm( sAlgorithm_ ),
      idExecutionAccount( idExecutionAccount_ ), idDataAccount( idDataAccount_ ),
      eOrderSidePending( OrderSide::Unknown ), eOrderSideActive( OrderSide::Unknown ), 
      nPositionPending( 0 ), nPositionActive( 0 ), dblConstructedValue( 0.0 ), dblMarketValue( 0.0 ),
      dblUnRealizedPL( 0.0 ), dblRealizedPL( 0.0 ), dblCommissionPaid( 0.0 ) {};

  };

  struct TableRowDef: TableRowDefNoKey { // separated out so can an auto key on insertion with only a TableRowDefNoKey
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "positionid", idPosition );
      TableRowDefNoKey::Fields( a );
    }
    idPosition_t idPosition;

    TableRowDef( void ) : idPosition( 0 ), TableRowDefNoKey() {};
    TableRowDef( const idPortfolio_t& idPortfolio_, const std::string& sName_, const idInstrument_t& idInstrument_,
      const idAccount_t& idExecutionAccount_, const idAccount_t& idDataAccount_, const std::string& sAlgorithm_ ) 
      : idPosition( 0 ), 
        TableRowDefNoKey( idPortfolio_, sName_, idInstrument_, idExecutionAccount_, idDataAccount_, sAlgorithm_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "positionid" );
      ou::db::Constraint( a, "portfolioid", tablenames::sPortfolio, "portfolioid" );
      ou::db::Constraint( a, "executionaccountid", tablenames::sAccount, "accountid" );
      ou::db::Constraint( a, "dataaccountid", tablenames::sAccount, "accountid" );
      ou::db::Constraint( a, "instrumentid", tablenames::sInstrument, "instrumentid" );
      //"create index idx_positions_portfolioid on positions( portfolioid );",
    }
  };

  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount, 
    const idPortfolio_t&, const std::string& sName, const std::string& sAlgorithm );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const TableRowDef& row );
  CPosition( const TableRowDef& row );
  CPosition( void );
  ~CPosition(void);

  const std::string& Notes( void ) const { return m_row.sNotes; };
  void Append( std::string& sNotes ) { m_row.sNotes += sNotes; };

  pInstrument_cref GetInstrument( void ) const { assert( 0 != m_pInstrument ); return m_pInstrument; };
  double GetUnRealizedPL( void ) const { return m_row.dblUnRealizedPL; };
  double GetRealizedPL( void ) const { return m_row.dblRealizedPL; };
  double GetCommissionPaid( void ) const { return m_row.dblCommissionPaid; };

  bool OrdersPending( void ) const { return ( 0 != m_row.nPositionPending ); };
  bool BuyOrdersPending( void ) const { return ( OrdersPending() && ( OrderSide::Buy == m_row.eOrderSidePending ) ); };
  bool SellOrdersPending( void ) const { return ( OrdersPending() && ( OrderSide::Sell == m_row.eOrderSidePending ) ); };

  COrder::pOrder_t PlaceOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity
    );
  COrder::pOrder_t PlaceOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1
    );
  COrder::pOrder_t PlaceOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );
  void CancelOrder( idOrder_t idOrder );
  void CancelOrders( void );
  void ClosePosition( OrderType::enumOrderType eOrderType = OrderType::Market );

  ou::Delegate<const CPosition*> OnQuote;
  ou::Delegate<const CPosition*> OnTrade;  // nothing useful currently
  ou::Delegate<execution_delegate_t> OnExecution;
  ou::Delegate<const CPosition*> OnCommission;

  void EmitStatus( std::stringstream& ssStatus ) const;

  void Set( pInstrument_cref, pProvider_t& pExecutionProvider, pProvider_t& pDataProvider );  // need to set verification that pointers have been set
  void Set( idPosition_t idPosition ) { m_row.idPosition = idPosition; };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:

  typedef CSymbolBase::quote_t quote_t;
  typedef CSymbolBase::trade_t trade_t;
  typedef CSymbolBase::greek_t greek_t;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

  pInstrument_t m_pInstrument;

  typedef std::vector<pOrder_t> vOrders_t;
  vOrders_t m_OpenOrders;  // active orders waiting to be executed or cancelled
  vOrders_t m_ClosedOrders;  // orders that have executed or have cancelled
  vOrders_t m_AllOrders;  // keeps track of all orders in case we have to search both lists

private:

  TableRowDef m_row;

  bool m_bInstrumentAssigned;
  bool m_bExecutionAccountAssigned;
  bool m_bDataAccountAssigned;
  bool m_bConnectedToDataProvider;

  double m_dblMultiplier;

  void Construction( void );
  void DisconnectFromDataProvider( int );

  void HandleExecution( const std::pair<const COrder&, const CExecution&>& );
  void HandleCommission( const COrder& );

  void PlaceOrder( pOrder_t pOrder );

  void HandleQuote( quote_t );
  void HandleTrade( trade_t );
  void HandleGreek( greek_t );

};

} // namespace tf
} // namespace ou
