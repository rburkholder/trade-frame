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
#include <memory>

#include <boost/tuple/tuple.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCommon/Delegate.h>

#include "KeyTypes.h"

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

// Usage Note:
//   Position watches continously

namespace ou { // One Unified
namespace tf { // TradeFrame

class Position {
  friend class boost::serialization::access;
  friend std::ostream& operator<<( std::ostream& os, const Position& );
public:

  using pPosition_t = std::shared_ptr<Position>;

  using pProvider_t = ProviderInterfaceBase::pProvider_t;

  using pInstrument_t = Instrument::pInstrument_t;

  using pWatch_t = Watch::pWatch_t;

  using idOrder_t = Order::idOrder_t;
  using pOrder_t = Order::pOrder_t ;
  using pOrder_ref = Order::pOrder_ref;

  using execution_pair_t = std::pair<const Position&, const Execution&>;
  using execution_delegate_t = const execution_pair_t&;

  using quote_pair_t = std::pair<const Position&, const Quote&>;
  using trade_pair_t = std::pair<const Position&, const Trade&>;

  using PositionDelta_delegate_t = boost::tuple<const Position&, double, double>;  // position, old value, new value

  using idPosition_t = keytypes::idPosition_t;
  using idPortfolio_t = keytypes::idPortfolio_t;
  using idAccount_t = keytypes::idAccount_t;
  using idInstrument_t = keytypes::idInstrument_t;

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
    OrderSide::EOrderSide eOrderSidePending;
    boost::uint32_t nPositionPending; // indicates whether we are in a long or short position
    OrderSide::EOrderSide eOrderSideActive;
    boost::uint32_t nPositionActive;
  // following value markers exclude commission
    double dblConstructedValue;  // based upon position trades  used for RealizedPL calcs, keeps accruing
  // following value markers exclude commission
    double dblUnRealizedPL;  // based upon market quotes
    double dblRealizedPL;  // based upon position trades
    double dblCommissionPaid; // contains total commissions

    // account and instrument objects need to be manually asssigned in a second step
    TableRowDefNoKey()
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

    TableRowDef() : idPosition( 0 ), TableRowDefNoKey() {};
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

  Position( pInstrument_t&, pProvider_t pExecutionProvider, pProvider_t pDataProvider, // persisted Position (old style construction)
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const idPortfolio_t&, const std::string& sNamePosition, const std::string& sAlgorithm );
  Position( pWatch_t, pProvider_t pExecutionProvider );  // in memory Position (new style construction)
  Position( pWatch_t, pProvider_t pExecutionProvider,   // persisted Position
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const idPortfolio_t&, const std::string& sNamePosition, const std::string& sAlgorithm );
  Position(  // persisted Position (streamlined style construction)
    pWatch_t, pProvider_t pExecutionProvider,
    const idPortfolio_t&, const std::string& sNamePosition, const std::string& sAlgorithm );
  Position( pInstrument_t&, pProvider_t pExecutionProvider, pProvider_t pDataProvider );
  Position( pInstrument_t&, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes );
  Position( pInstrument_t&, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const TableRowDef& row );
  Position( const TableRowDef& row );
  Position();
  virtual ~Position();

  const std::string& Notes() const { return m_row.sNotes; };
  void SetNotes( const std::string& sNote ) { m_row.sNotes = sNote; }
  void AppendNotes( std::string& sNotes ) { m_row.sNotes += sNotes; };

  pInstrument_t GetInstrument() { assert( nullptr != m_pWatch.get() ); return m_pWatch->GetInstrument(); }
  pWatch_t GetWatch() { assert( nullptr != m_pWatch.get() ); return m_pWatch; }

  size_t GetActiveSize() const { return m_row.nPositionActive; }
   ou::tf::OrderSide::EOrderSide GetActiveSide() const { return m_row.eOrderSideActive; }

  double GetUnRealizedPL() const { return m_row.dblUnRealizedPL; };
  double GetRealizedPL() const { return m_row.dblRealizedPL; };
  double GetCommissionPaid() const { return m_row.dblCommissionPaid; };
  void QueryStats( double& dblUnRealized, double& dblRealized, double& dblCommissionsPaid, double& dblTotal ) const {
    dblTotal  = ( dblUnRealized = m_row.dblUnRealizedPL );
    dblTotal += ( dblRealized = m_row.dblRealizedPL );
    dblTotal -= ( dblCommissionsPaid = m_row.dblCommissionPaid );
  }

  idPosition_t Id() const { return m_row.idPosition; }
  idPortfolio_t IdPortfolio() const { return m_row.idPortfolio; }

  bool IsActive() const { return ( 0 != m_row.nPositionActive ); }

  bool OrdersPending() const { return ( 0 != m_row.nPositionPending ); };
  bool BuyOrdersPending() const { return ( OrdersPending() && ( OrderSide::Buy == m_row.eOrderSidePending ) ); };
  bool SellOrdersPending() const { return ( OrdersPending() && ( OrderSide::Sell == m_row.eOrderSidePending ) ); };

  Order::pOrder_t ConstructOrder( // market
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity
    );
  Order::pOrder_t ConstructOrder( // limit or stop
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1
    );
  Order::pOrder_t ConstructOrder( // limit and stop
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,
    double dblPrice2
    );

  Order::pOrder_t PlaceOrder( // market
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity
    );
  Order::pOrder_t PlaceOrder( // limit or stop
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1
    );
  Order::pOrder_t PlaceOrder( // limit and stop
    OrderType::EOrderType eOrderType,
    OrderSide::EOrderSide eOrderSide,
    boost::uint32_t nOrderQuantity,
    double dblPrice1,
    double dblPrice2
    );

  void PlaceOrder( pOrder_t pOrder );
  void UpdateOrder( pOrder_t pOrder );

  void CancelOrder( idOrder_t idOrder );
  void CancelOrders();
  void ClosePosition( OrderType::EOrderType eOrderType = OrderType::Market );

  ou::Delegate<const ou::tf::Trade&> OnTrade;
  ou::Delegate<const ou::tf::Quote&> OnQuote;  // emitted in HandleQuote, prior to statistic calculations
  ou::Delegate<const quote_pair_t&> OnQuotePostProcess;  // updates UnRealizedPL, emitted by HandleQuote

  ou::Delegate<const Position&> OnPositionChanged;  // after order placement, order canceled, order execution

  ou::Delegate<execution_delegate_t> OnExecutionRaw;

  // could probably change these to fast delegate
  ou::Delegate<const Position&> OnUpdateExecutionForPortfolioManager;
  ou::Delegate<const Position&> OnUpdateCommissionForPortfolioManager;

  ou::Delegate<const PositionDelta_delegate_t&> OnExecution;   // < - used by portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnCommission;  // < - used by portfolio
  ou::Delegate<const PositionDelta_delegate_t&> OnUnRealizedPL;// < - use by portfolio, ( *this, dblPreviousUnRealizedPL, m_row.dblUnRealizedPL )  when unrealizedPL changes, updated by HandleQuote

  void Set( pInstrument_t&, pProvider_t& pExecutionProvider, pProvider_t& pDataProvider );  // need to set verification that pointers have been set
  void Set( idPosition_t idPosition ) { m_row.idPosition = idPosition; };
  void Set( const std::string& sName ) { m_row.sName = sName; }

  const TableRowDef& GetRow() const { return m_row; };

  pProvider_t GetExecutionProvider() { return m_pExecutionProvider; };
  pProvider_t GetDataProvider() { assert( nullptr != m_pWatch.get() ); return m_pWatch->GetProvider(); };

protected:

  using quote_t = SymbolBase::quote_t;
  using trade_t = SymbolBase::trade_t;
  pProvider_t m_pExecutionProvider;

  pWatch_t m_pWatch;

  using vOrders_t = std::vector<pOrder_t>;
  using vOrders_iter_t = vOrders_t::iterator;
  vOrders_t m_vOpenOrders;  // active orders waiting to be executed or canceled
  vOrders_t m_vClosedOrders;  // orders that have executed or have canceled
  vOrders_t m_vAllOrders;  // keeps track of all orders in case we have to search both lists

  TableRowDef m_row;

private:

  double m_dblMultiplier;

  void ConstructWatch( pInstrument_t&, pProvider_t pDataProvider );
  void Construction();

  void Register( pOrder_t pOrder );

  void HandleExecution( const std::pair<const Order&, const Execution&>& );
  void HandleCommission( const Order& );
  void HandleCancellation( const Order& );

  void CancelOrder( vOrders_iter_t iter );
  void CancelOrder( pOrder_t& pOrder );

  void HandleQuote( quote_t& );
  void HandleTrade( trade_t& );

  void UpdateRowValues( double price, boost::uint32_t quan, OrderSide::EOrderSide side );

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