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

  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::pOrder_ref pOrder_ref;

  typedef std::pair<const CPosition*, const CExecution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idPortfolio_t idPortfolio_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "positionid", idPosition );
      ou::db::Field( a, "portfolioid", idPortfolio );
      ou::db::Field( a, "name", sName );
      ou::db::Field( a, "notes", sNotes );
      ou::db::Field( a, "executionaccountid", idExecutionAccount );
      ou::db::Field( a, "dataaccountid", idDataAccount );
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "ordersidepending", eOrderSidePending );
      ou::db::Field( a, "quantitypending", nPositionPending );
      ou::db::Field( a, "ordersideactive", eOrderSideActive );
      ou::db::Field( a, "quantityactive", nPositionActive );
      ou::db::Field( a, "constructedvalue", dblConstructedValue );
      ou::db::Field( a, "marketvalue", dblMarketValue );
      ou::db::Field( a, "unrealizedpl", dblUnRealizedPL );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "CommissionPaid", dblCommissionPaid );
    }

    idPosition_t idPosition;
    idPortfolio_t idPortfolio;
    std::string sName;
    std::string sNotes;
    std::string idExecutionAccount;
    std::string idDataAccount;
    std::string idInstrument;
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

    TableRowDef( void ) : idPosition( 0 ), eOrderSidePending( OrderSide::Unknown ), eOrderSideActive( OrderSide::Unknown ), 
      nPositionPending( 0 ), nPositionActive( 0 ), dblConstructedValue( 0.0 ), dblMarketValue( 0.0 ),
      dblUnRealizedPL( 0.0 ), dblRealizedPL( 0.0 ), dblCommissionPaid( 0.0 ) {};

  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "positionid" );
      ou::db::Constraint( a, "portfolioid", tablenames::sPortfolio, "portfolioid" );
      ou::db::Constraint( a, "executionaccountid", tablenames::sAccount, "accountid" );
      ou::db::Constraint( a, "dataaccountid", tablenames::sAccount, "accountid" );
      ou::db::Constraint( a, "instrumentid", tablenames::sInstruments, "instrumentid" );
      //"create index idx_positions_portfolioid on positions( portfolioid );",
    }
  };

  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes );
  CPosition( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const TableRowDef& row );
  CPosition( const TableRowDef& row );
  CPosition( void );
  ~CPosition(void);

  const std::string& Notes( void ) const { return m_row.sNotes; };
  void Append( std::string& sNotes ) { m_row.sNotes += sNotes; };

  pInstrument_cref GetInstrument( void ) const { return m_pInstrument; };
  double GetUnRealizedPL( void ) const { return m_row.dblUnRealizedPL; };
  double GetRealizedPL( void ) const { return m_row.dblRealizedPL; };
  double GetCommissionPaid( void ) const { return m_row.dblCommissionPaid; };

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
  void CancelOrders( void );
  void ClosePosition( void );

  ou::Delegate<const CPosition*> OnQuote;
  ou::Delegate<const CPosition*> OnTrade;  // nothing useful currently
  ou::Delegate<execution_delegate_t> OnExecution;
  ou::Delegate<const CPosition*> OnCommission;

  void EmitStatus( std::stringstream& ssStatus );

  void Set( pInstrument_cref, pProvider_t pExecutionProvider, pProvider_t pDataProvider );

protected:

  typedef CSymbolBase::quote_t quote_t;
  typedef CSymbolBase::trade_t trade_t;
  typedef CSymbolBase::greek_t greek_t;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

  pInstrument_t m_pInstrument;

  std::vector<pOrder_t> m_OpenOrders;  // active orders waiting to be executed or cancelled
  std::vector<pOrder_t> m_ClosedOrders;  // orders that have executed or have cancelled
  std::vector<pOrder_t> m_AllOrders;  // keeps track of all orders in case we have to search both lists

private:

  TableRowDef m_row;

  bool m_bInstrumentAssigned;
  bool m_bExecutionAccountAssigned;
  bool m_bDataAccountAssigned;

  double m_dblMultiplier;

  void Construction( void );

  void HandleExecution( const std::pair<const COrder&, const CExecution&>& );
  void HandleCommission( const COrder& );

  void PlaceOrder( pOrder_t pOrder );

  void HandleQuote( quote_t );
  void HandleTrade( trade_t );
  void HandleGreek( greek_t );

};

} // namespace tf
} // namespace ou
