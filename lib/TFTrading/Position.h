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
#include <boost/tuple/tuple.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCommon/Delegate.h>


#include "TradingEnumerations.h"
#include "KeyTypes.h"

#include "Instrument.h"
#include "Watch.h"
#include "Order.h"
#include "ProviderInterface.h"

// Multiple position records grouped together would be a multi-legged instrument, aka Combo
//   -- not sure how to construct this yet
//    -- a position consists of one or more legs, so a leg would be the atomic unit of composition
// once a position with multiple legs is in a portfolio, should be able to sum up the legs to see if re-balancing or adjustments are required
// A Portfolio should be a collection of position records, whether individual positions, or Combos
// check that orders for both sell side and buy side are not opened simultaneously
// a position is provider dependent, ie, only one provider per position
// therefore:  multi-legged constructions have one leg in each position.
//    use multiple positions, and group into a portfolio to track the legs

// Create Delegates so trade and market data updates propogate to combo and portfolio

// todo:  there is nothing for setting a portfolio id

namespace ou { // One Unified
namespace tf { // TradeFrame

class Position {
  friend class boost::serialization::access;
public:

  friend std::ostream& operator<<( std::ostream& os, const Position& );

  typedef boost::shared_ptr<Position> pPosition_t;

  typedef ProviderInterfaceBase::pProvider_t pProvider_t;

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef Instrument::pInstrument_cref pInstrument_cref;
  
  typedef Watch::pWatch_t pWatch_t;

  typedef Order::idOrder_t idOrder_t;
  typedef Order::pOrder_t pOrder_t;
  typedef Order::pOrder_ref pOrder_ref;

  typedef std::pair<const Position&, const Execution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef std::pair<const Position&, const Quote&> quote_pair_t;
  typedef std::pair<const Position&, const Trade&> trade_pair_t;

  typedef boost::tuple<const Position&, double, double> PositionDelta_delegate_t;  // position, old value, new value

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
    boost::uint32_t nPositionPending; // indicates whether we are in a long or short position
    OrderSide::enumOrderSide eOrderSideActive;
    boost::uint32_t nPositionActive;
  // following value markers exclude commission
    double dblConstructedValue;  // based upon position trades  used for RealizedPL calcs, keeps accruing
  // following value markers exclude commission
    double dblUnRealizedPL;  // based upon market quotes
    double dblRealizedPL;  // based upon position trades
    double dblCommissionPaid; // contains total commissions

    // account and instrument objects need to be manually asssigned in a second step
    TableRowDefNoKey( void ) 
      : eOrderSidePending( OrderSide::Unknown ), eOrderSideActive( OrderSide::Unknown ), 
      nPositionPending( 0 ), nPositionActive( 0 ), dblConstructedValue( 0.0 ), 
      dblUnRealizedPL( 0.0 ), dblRealizedPL( 0.0 ), dblCommissionPaid( 0.0 ) {};
    TableRowDefNoKey( const TableRowDefNoKey& row ) 
      : idPortfolio( row.idPortfolio ), sName( row.sName ), sNotes( row.sNotes ), 
      idExecutionAccount( row.idExecutionAccount ), idDataAccount( row.idDataAccount ), idInstrument( row.idInstrument ),
      sAlgorithm( row.sAlgorithm ), eOrderSidePending( row.eOrderSidePending ), nPositionPending( row.nPositionPending ), 
      eOrderSideActive( row.eOrderSideActive ), nPositionActive( row.nPositionActive ), 
      dblConstructedValue( row.dblConstructedValue ), 
      dblUnRealizedPL( row.dblUnRealizedPL ), dblRealizedPL( row.dblRealizedPL ), dblCommissionPaid( row.dblCommissionPaid ) {};
    TableRowDefNoKey( const idPortfolio_t& idPortfolio_, const std::string& sName_, const idInstrument_t& idInstrument_,
      const idAccount_t& idExecutionAccount_, const idAccount_t& idDataAccount_, const std::string& sAlgorithm_ ) 
      : idPortfolio( idPortfolio_ ), sName( sName_ ), idInstrument( idInstrument_ ), sAlgorithm( sAlgorithm_ ),
      idExecutionAccount( idExecutionAccount_ ), idDataAccount( idDataAccount_ ),
      eOrderSidePending( OrderSide::Unknown ), eOrderSideActive( OrderSide::Unknown ), 
      nPositionPending( 0 ), nPositionActive( 0 ), dblConstructedValue( 0.0 ), 
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

  Position( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount, 
    const idPortfolio_t&, const std::string& sName, const std::string& sAlgorithm );
  Position( pWatch_t, pProvider_t pExecutionProvider );
  Position( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );
  Position( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes );
  Position( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const TableRowDef& row );
  Position( const TableRowDef& row );
  Position( void );
  virtual ~Position(void);

  const std::string& Notes( void ) const { return m_row.sNotes; };
  void Append( std::string& sNotes ) { m_row.sNotes += sNotes; };

  pInstrument_t GetInstrument( void ) { assert( nullptr != m_pWatch.get() ); return m_pWatch->GetInstrument(); }
  pWatch_t GetWatch( void ) { assert( nullptr != m_pWatch.get() ); return m_pWatch; }
  double GetUnRealizedPL( void ) const { return m_row.dblUnRealizedPL; };
  double GetRealizedPL( void ) const { return m_row.dblRealizedPL; };
  double GetCommissionPaid( void ) const { return m_row.dblCommissionPaid; };

  bool OrdersPending( void ) const { return ( 0 != m_row.nPositionPending ); };
  bool BuyOrdersPending( void ) const { return ( OrdersPending() && ( OrderSide::Buy == m_row.eOrderSidePending ) ); };
  bool SellOrdersPending( void ) const { return ( OrdersPending() && ( OrderSide::Sell == m_row.eOrderSidePending ) ); };

  Order::pOrder_t ConstructOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity
    );
  Order::pOrder_t ConstructOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1
    );
  Order::pOrder_t ConstructOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );

  Order::pOrder_t PlaceOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity
    );
  Order::pOrder_t PlaceOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1
    );
  Order::pOrder_t PlaceOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );

  void PlaceOrder( pOrder_t pOrder );

  void CancelOrder( idOrder_t idOrder );
  void CancelOrders( void );
  void ClosePosition( OrderType::enumOrderType eOrderType = OrderType::Market );

  ou::Delegate<const ou::tf::Trade&> OnTrade; 
  ou::Delegate<const ou::tf::Quote&> OnQuote;
  ou::Delegate<const quote_pair_t&> OnQuotePostProcess;  // updates UnRealizedPL

  ou::Delegate<const Position&> OnPositionChanged;  // after order placement, order canceled, order execution

  ou::Delegate<execution_delegate_t> OnExecutionRaw;

  // could probably change these to fast delegate
  ou::Delegate<const Position&> OnUpdateExecutionForPortfolioManager;
  ou::Delegate<const Position&> OnUpdateCommissionForPortfolioManager;

  ou::Delegate<const PositionDelta_delegate_t&> OnExecution;   // < - used by portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnCommission;  // < - used by portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnUnRealizedPL;/* ( *this, dblPreviousUnRealizedPL, m_row.dblUnRealizedPL ) */  // < - use by portfolio

  void Set( pInstrument_cref, pProvider_t& pExecutionProvider, pProvider_t& pDataProvider );  // need to set verification that pointers have been set
  void Set( idPosition_t idPosition ) { m_row.idPosition = idPosition; };
  void Set( const std::string& sName ) { m_row.sName = sName; }

  const TableRowDef& GetRow( void ) const { return m_row; };

  pProvider_t GetExecutionProvider( void ) { return m_pExecutionProvider; };
  pProvider_t GetDataProvider( void ) { assert( nullptr != m_pWatch.get() ); return m_pWatch->GetProvider(); };

protected:

  typedef SymbolBase::quote_t quote_t;
  typedef SymbolBase::trade_t trade_t;
  pProvider_t m_pExecutionProvider;

  pWatch_t m_pWatch;

  typedef std::vector<pOrder_t> vOrders_t;
  typedef vOrders_t::iterator vOrders_iter_t;
  vOrders_t m_vOpenOrders;  // active orders waiting to be executed or canceled
  vOrders_t m_vClosedOrders;  // orders that have executed or have canceled
  vOrders_t m_vAllOrders;  // keeps track of all orders in case we have to search both lists

  TableRowDef m_row;

private:

  bool m_bExecutionAccountAssigned;
  bool m_bDataAccountAssigned;
  bool m_bWatchConstructedLocally;

  double m_dblMultiplier;

  void ConstructWatch( pInstrument_cref, pProvider_t pDataProvider );
  void Construction( void );

  void HandleExecution( const std::pair<const Order&, const Execution&>& );
  void HandleCommission( const Order& );
  void HandleCancellation( const Order& );

  void CancelOrder( vOrders_iter_t iter );

  void HandleQuote( quote_t );
  void HandleTrade( trade_t );

  void UpdateRowValues( double price, boost::uint32_t quan, OrderSide::enumOrderSide side );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //std::cout << "saving " << m_row.idPosition << "," << m_row.sName << "," << m_row.eOrderSidePending << "," << m_row.eOrderSidePending << std::endl;
    ar & m_row.idPortfolio;
    ar & m_row.idPosition;
    ar & m_row.eOrderSidePending;
    ar & m_row.nPositionPending;
    ar & m_row.eOrderSideActive;
    ar & m_row.nPositionActive;
    ar & m_row.dblCommissionPaid;
    ar & m_row.dblConstructedValue;
    ar & m_row.dblRealizedPL;
    ar & m_row.dblUnRealizedPL;
    ar & m_row.sAlgorithm;
    ar & m_row.sName;
    ar & m_row.sNotes;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //std::cout << "loading " << m_row.idPosition << "," << m_row.sName << "," << m_row.eOrderSidePending << "," << m_row.eOrderSidePending << std::endl;
    ar & m_row.idPortfolio;
    ar & m_row.idPosition;
    ar & m_row.eOrderSidePending;
    ar & m_row.nPositionPending;
    ar & m_row.eOrderSideActive;
    ar & m_row.nPositionActive;
    ar & m_row.dblCommissionPaid;
    ar & m_row.dblConstructedValue;
    ar & m_row.dblRealizedPL;
    ar & m_row.dblUnRealizedPL;
    ar & m_row.sAlgorithm;
    ar & m_row.sName;
    ar & m_row.sNotes;
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

std::ostream& operator<<( std::ostream& os, const Position& );

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::Position, 1)