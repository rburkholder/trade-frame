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

#include "StdAfx.h"

#include "Position.h"
#include <LibTrading/OrderManager.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CPosition::m_sSqlCreate( 
  "create table positions ( \
    positionid INTEGER PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    portfolioid TEXT NOT NULL, \
    name TEXT NOT NULL, \
    notes TEXT DEFAULT "", \
    executionaccountid TEXT NOT NULL, \
    dataaccountid TEXT NOT NULL, \
    instrumentid TEXT NOT NULL, \
    ordersidepending SMALLINT NOT NULL, \
    ordersideactive SMALLINT NOT NULL, \
    quantitypending INT NOT NULL, \
    quantityactive INT NOT NULL, \
    constructedvalue double NOT NULL, \
    marketvalue double NOT NULL, \
    plunrealized double NOT NULL, \
    plrealized double NOT NULL, \
    commissionpaid double NOT NULL, \
    CONSTRAINT fk_positions_portfolioid \
      FOREIGN KEY(portfolioid) REFERENCES portfolios(portfolioid) \
      ON DELETE RESTRICT ON UPDATE CASCADE, \
    CONSTRAINT fk_positions_executionaccountid \
      FOREIGN KEY(executionaccountid) REFERENCES accounts(accountid) \
      ON DELETE RESTRICT ON UPDATE CASCADE, \
    CONSTRAINT fk_positions_dataaccountid \
      FOREIGN KEY(dataaccountid) REFERENCES accounts(accountid) \
      ON DELETE RESTRICT ON UPDATE CASCADE, \
    CONSTRAINT fk_postions_instrumentid \
      FOREIGN KEY(instrumentid) REFERENCES instruments(instrumentid) \
      ON DELETE RESTRICT ON UPDATE CASCADE \
        \
  );");
const std::string CPosition::m_sSqlSelect( 
  "SELECT portfolioid, name, notes, executionaccountid, dataaccountid, instrumentid, \
    ordersidepending, ordersideactive, quantitypending, quantityactive, \
    constructedvalue, marketvalue, plunrealized, plrealized, commissionpaid \
  FROM positions WHERE positionid = :id \
  ;" );
const std::string CPosition::m_sSqlInsert( 
  "INSERT into positions (positionid, portfolioid, name, notes, executionaccountid, dataaccountid, instrumentid, \
    ordersidepending, ordersideactive, quantitypending, quantityactive, \
    constructedvalue, marketvalue, plunrealized, plrealized, commissionpaid ) \
  VALUES ( :positionid, :portfolioid, :name, :notes, :executionaccountid, :dataaccountid, :instrumentid, \
    :ordersidepending, :ordersideactive, :quantitypending, :quantityactive, \
    :constructedvalue, :marketvalue, :plunrealized, :plrealized, :commissionpaid ) \
    ;" );
const std::string CPosition::m_sSqlUpdate( 
  "UPDATE positions SET \
  portfolioid = :portfolioid, \
  name = :name, \
  notes = :notes, \
  executionaccountid = :executionaccountid, \
  dataaccountid = :dataaccountid, \
  instrumentid = :instrumentid, \
  ordersidepending = :ordersidepending, \
  ordersideactive = :ordersideactive, \
  quantitypending = :quantitypending, \
  quantityactive = :quantityactive, \
  constructedvalue = :constructedvalue, \
  marketvalue = :marketvalue, \
  plunrealized = :plunrealized, \
  plrealized = :plrealized, \
  WHERE positionid = ::id \
  ;" );
const std::string CPosition::m_sSqlDelete( "DELETE FROM positions WHERE positionid = :id;" );

CPosition::CPosition( pInstrument_cref pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider ) 
: m_pExecutionProvider( pExecutionProvider ), m_pDataProvider( pDataProvider ), 
  m_pInstrument( pInstrument ), 
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblConstructedValue( 0 ), m_dblMarketValue( 0 ),
  m_dblUnRealizedPL( 0 ), m_dblRealizedPL( 0 ),
  m_dblMultiplier( 1 ), 
  m_dblCommissionPaid( 0 ),
  m_bInstrumentAssigned ( true ), m_bExecutionAccountAssigned( true ), m_bDataAccountAssigned( true )
{
  Construction();
}

CPosition::CPosition( pInstrument_cref pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes ) 
: m_pExecutionProvider( pExecutionProvider ), m_pDataProvider( pDataProvider ), 
  m_pInstrument( pInstrument ), 
  m_sNotes( sNotes ),
  m_nPositionPending( 0 ), m_nPositionActive( 0 ), 
  m_eOrderSidePending( OrderSide::Unknown ), m_eOrderSideActive( OrderSide::Unknown ),
  m_dblConstructedValue( 0 ), m_dblMarketValue( 0 ),
  m_dblUnRealizedPL( 0 ), m_dblRealizedPL( 0 ),
  m_dblMultiplier( 1 ), 
  m_dblCommissionPaid( 0 ),
  m_bInstrumentAssigned ( true ), m_bExecutionAccountAssigned( true ), m_bDataAccountAssigned( true )
{
  Construction();
}

CPosition::CPosition( pInstrument_cref pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider, 
  idPosition_t idPosition, sqlite3_stmt* pStmt ) 
: m_idPosition( idPosition ),
  m_dblMultiplier( 1 ), 
  m_pExecutionProvider( pExecutionProvider ), m_pDataProvider( pDataProvider ), 
  m_pInstrument( pInstrument ), 
  m_idPortfolio( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_sNotes( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 2 ) ) ),
  m_sidExecutionAccount( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 3 ) ) ),
  m_sidDataAccount( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 4 ) ) ),
  m_sInstrumentName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 5 ) ) ),
  m_eOrderSidePending( static_cast<OrderSide::enumOrderSide>( sqlite3_column_int( pStmt, 6 ) ) ),
  m_eOrderSideActive( static_cast<OrderSide::enumOrderSide>( sqlite3_column_int( pStmt, 7 ) ) ),
  m_nPositionPending( sqlite3_column_int( pStmt, 8 ) ),
  m_nPositionActive( sqlite3_column_int( pStmt, 9 ) ),
  m_dblConstructedValue( sqlite3_column_double( pStmt, 10 ) ),
  m_dblMarketValue( sqlite3_column_double( pStmt, 11 ) ),
  m_dblUnRealizedPL( sqlite3_column_double( pStmt, 12 ) ),
  m_dblRealizedPL( sqlite3_column_double( pStmt, 13 ) ),
  m_dblCommissionPaid( sqlite3_column_double( pStmt, 14 ) ),
  m_bInstrumentAssigned ( true ), m_bExecutionAccountAssigned( true ), m_bDataAccountAssigned( true )
{
  Construction();
}

CPosition::CPosition( idPosition_t idPosition, sqlite3_stmt* pStmt ) 
: m_idPosition( idPosition ),
  m_dblMultiplier( 1 ), 
  m_idPortfolio( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_sNotes( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 2 ) ) ),
  m_sidExecutionAccount( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 3 ) ) ),
  m_sidDataAccount( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 4 ) ) ),
  m_sInstrumentName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 5 ) ) ),
  m_eOrderSidePending( static_cast<OrderSide::enumOrderSide>( sqlite3_column_int( pStmt, 6 ) ) ),
  m_eOrderSideActive( static_cast<OrderSide::enumOrderSide>( sqlite3_column_int( pStmt, 7 ) ) ),
  m_nPositionPending( sqlite3_column_int( pStmt, 8 ) ),
  m_nPositionActive( sqlite3_column_int( pStmt, 9 ) ),
  m_dblConstructedValue( sqlite3_column_double( pStmt, 10 ) ),
  m_dblMarketValue( sqlite3_column_double( pStmt, 11 ) ),
  m_dblUnRealizedPL( sqlite3_column_double( pStmt, 12 ) ),
  m_dblRealizedPL( sqlite3_column_double( pStmt, 13 ) ),
  m_dblCommissionPaid( sqlite3_column_double( pStmt, 14 ) ),
  m_bInstrumentAssigned ( false ), m_bExecutionAccountAssigned( false ), m_bDataAccountAssigned( false )
{
  // need flags to wait for execution, data, instrument variables to be set
}

void CPosition::Construction( void ) {
  m_dblMultiplier = m_pInstrument->GetMultiplier();
  if ( m_pDataProvider->ProvidesQuotes() ) {
    m_pDataProvider->AddQuoteHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleQuote ) );
  }
  if ( m_pDataProvider->ProvidesTrades() ) {
    m_pDataProvider->AddTradeHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleTrade ) );
  }
  if ( m_pDataProvider->ProvidesGreeks() ) {
    m_pDataProvider->AddGreekHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleGreek ) );
  }
}

void CPosition::Set( pInstrument_cref pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider ) {

  m_pInstrument = pInstrument;
  m_bInstrumentAssigned = true;

  m_pExecutionProvider = pExecutionProvider;
  m_bExecutionAccountAssigned = true;

  m_pDataProvider = pDataProvider;
  m_bDataAccountAssigned = true;

  Construction();

}

CPosition::~CPosition(void) {
  if ( m_pDataProvider->ProvidesQuotes() ) {
    m_pDataProvider->RemoveQuoteHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleQuote ) );
  }
  if ( m_pDataProvider->ProvidesTrades() ) {
    m_pDataProvider->RemoveTradeHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleTrade ) );
  }
  if ( m_pDataProvider->ProvidesGreeks() ) {
    m_pDataProvider->RemoveGreekHandler( m_pInstrument, MakeDelegate( this, &CPosition::HandleGreek ) );
  }
  for ( std::vector<pOrder_t>::iterator iter = m_AllOrders.begin(); iter != m_AllOrders.end(); ++iter ) {
    iter->get()->OnCommission.Remove( MakeDelegate( this, &CPosition::HandleCommission ) );
    iter->get()->OnExecution.Remove( MakeDelegate( this, &CPosition::HandleExecution ) );
  }
  m_OpenOrders.clear();
  m_ClosedOrders.clear();
  m_AllOrders.clear();
}

void CPosition::HandleQuote( quote_t quote ) {
  bool bProcessed(false);
  switch ( m_eOrderSideActive ) {
    case OrderSide::Buy:
      m_dblMarketValue = m_nPositionActive * quote.Bid() * m_dblMultiplier;
      m_dblUnRealizedPL = m_dblMarketValue - m_dblConstructedValue;
      bProcessed = true;
      break;
    case OrderSide::Sell:
      m_dblMarketValue = - ( m_nPositionActive * quote.Ask() ) * m_dblMultiplier;
      m_dblUnRealizedPL = m_dblConstructedValue - m_dblMarketValue;
      bProcessed = true;
      break;
  }

  if ( bProcessed ) {
    OnQuote( this );
  }
}

void CPosition::HandleTrade( trade_t trade ) {
  OnTrade( this );
}

void CPosition::HandleGreek( greek_t greek ) {
}

COrder::pOrder_t CPosition::PlaceOrder( // market
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity
) {

  assert( OrderSide::Unknown != eOrderSide );
  assert( OrderType::Market == eOrderType );
  pOrder_t pOrder( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity, m_idPosition ) );
  PlaceOrder( pOrder );
  return pOrder;
}

COrder::pOrder_t CPosition::PlaceOrder( // limit or stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1
) {

  assert( OrderSide::Unknown != eOrderSide );
  assert( ( OrderType::Limit == eOrderType) || ( OrderType::Stop == eOrderType ) || ( OrderType::Trail == eOrderType ) );
  pOrder_t pOrder( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1, m_idPosition ) );
  PlaceOrder( pOrder );
  return pOrder;
}

COrder::pOrder_t CPosition::PlaceOrder( // limit and stop
  OrderType::enumOrderType eOrderType,
  OrderSide::enumOrderSide eOrderSide,
  unsigned long nOrderQuantity,
  double dblPrice1,  
  double dblPrice2
) {

  assert( OrderSide::Unknown != eOrderSide );
  assert( ( OrderType::StopLimit == eOrderType) || ( OrderType::TrailLimit == eOrderType ) );
  pOrder_t pOrder( new COrder( m_pInstrument, eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2, m_idPosition ) );
  PlaceOrder( pOrder );
  return pOrder;
}

void CPosition::PlaceOrder( pOrder_t pOrder ) {

  if ( OrderSide::Unknown != m_eOrderSidePending ) { // ensure new order matches existing orders
    if ( m_eOrderSidePending != pOrder->GetOrderSide() ) {
      throw std::runtime_error( "CPosition::PlaceOrder, new order does not match pending order type" );
    }
  }
  m_eOrderSidePending = pOrder->GetOrderSide();

  m_nPositionPending += pOrder->GetQuantity();
  m_AllOrders.push_back( pOrder );
  m_OpenOrders.push_back( pOrder );
  pOrder->OnExecution.Add( MakeDelegate( this, &CPosition::HandleExecution ) ); 
  pOrder->OnCommission.Add( MakeDelegate( this, &CPosition::HandleCommission ) );
  COrderManager::Instance().PlaceOrder( &(*m_pExecutionProvider), pOrder );
}

void CPosition::CancelOrders( void ) {
  // may have a problem getting out of sync with broker if orders are cancelled by broker
  for ( std::vector<pOrder_t>::iterator iter = m_OpenOrders.begin(); iter != m_OpenOrders.end(); ++iter ) {
    COrderManager::Instance().CancelOrder( iter->get()->GetOrderId() );
    m_ClosedOrders.push_back( *iter );
  }
  m_OpenOrders.clear();
}

void CPosition::ClosePosition( void ) {
  // should outstanding orders be auto cancelled?
  // position is closed with a market order, can try to do limit in the futre, but need active market data
  switch ( m_eOrderSideActive ) {
    case OrderSide::Buy:
      PlaceOrder( OrderType::Market, OrderSide::Sell, m_nPositionActive );
      break;
    case OrderSide::Sell:
      PlaceOrder( OrderType::Market, OrderSide::Buy, m_nPositionActive );
      break;
    case OrderSide::Unknown:
      break;
  }
}

void CPosition::HandleCommission( const COrder& order ) {
  m_dblCommissionPaid += order.GetCommission();
  OnCommission( this );
}

// before entry to this method, sanity check:  side on execution is same as side on order
void CPosition::HandleExecution( const std::pair<const COrder&, const CExecution&>& status ) {

  // should be able to calculate profit/loss & position cost as exections are encountered
  // should be able to calculate position cost basis as position is updated (with and without commissions)
  // will need market feed in order to calculate profit/loss

  const COrder& order = status.first;
  const CExecution& exec = status.second;
  COrder::idOrder_t orderId = order.GetOrderId();

  // update position, regardless of whether we see order open or closed
  //double dblNewAverageCostPerShare = 0;
  double dblAvgConstructedCost = 0;
  double dblRealizedPL = 0;
  switch ( m_eOrderSideActive ) {
    case OrderSide::Buy:  // existing is long
      switch ( exec.GetOrderSide() ) {
        case OrderSide::Buy:  // increase long
          m_nPositionActive += exec.GetSize();
          m_dblConstructedValue += exec.GetSize() * exec.GetPrice() * m_dblMultiplier;
          break;
        case OrderSide::Sell:  // decrease long
          assert( m_nPositionActive >= exec.GetSize() );
          dblAvgConstructedCost = m_dblConstructedValue / ( m_nPositionActive * m_dblMultiplier );
          dblRealizedPL = exec.GetSize() * ( exec.GetPrice() - dblAvgConstructedCost ) * m_dblMultiplier;
          m_dblRealizedPL += dblRealizedPL;
          m_nPositionActive -= exec.GetSize();
          m_dblConstructedValue -= exec.GetSize() * dblAvgConstructedCost;
          //m_dblConstructedValue -= ( exec.GetSize() * exec.GetPrice() * m_dblMultiplier - dblRealizedPL );
          if ( 0 == m_nPositionActive ) {
            m_eOrderSideActive = OrderSide::Unknown;
            m_dblUnRealizedPL = 0.0;
          }
          break;
      }
      break;
    case OrderSide::Sell:  // existing is short
      switch ( exec.GetOrderSide() ) {
        case OrderSide::Sell:  // increase short
          m_nPositionActive += exec.GetSize();
          m_dblConstructedValue -= exec.GetSize() * exec.GetPrice() * m_dblMultiplier;
          break;
        case OrderSide::Buy:  // decrease short
          assert( m_nPositionActive >= exec.GetSize() );
          dblAvgConstructedCost = m_dblConstructedValue / ( m_nPositionActive * m_dblMultiplier );
          dblRealizedPL = exec.GetSize() * ( - exec.GetPrice() - dblAvgConstructedCost ) * m_dblMultiplier;
          m_dblRealizedPL += dblRealizedPL;
          m_nPositionActive -= exec.GetSize();
          m_dblConstructedValue += exec.GetSize() * dblAvgConstructedCost;
          //m_dblConstructedValue += ( exec.GetSize() * exec.GetPrice() * m_dblMultiplier + dblRealizedPL );  // is this correctly calculated?
          if ( 0 == m_nPositionActive ) {
            m_eOrderSideActive = OrderSide::Unknown;
            m_dblUnRealizedPL = 0.0;
          }
          break;
      }
      break;
    case OrderSide::Unknown:
      assert( 0 == m_nPositionActive );
      m_eOrderSideActive = exec.GetOrderSide();
      m_nPositionActive = exec.GetSize();
      switch ( m_eOrderSideActive ) {
        case OrderSide::Buy:
          m_dblConstructedValue += exec.GetSize() * exec.GetPrice() * m_dblMultiplier;
          break;
        case OrderSide::Sell:
          m_dblConstructedValue -= exec.GetSize() * exec.GetPrice() * m_dblMultiplier;
          break;
      }
      break;
  }

  // check that we think that the order is still active
  bool bOrderFound = false;
  for ( std::vector<pOrder_t>::iterator iter = m_OpenOrders.begin(); iter != m_OpenOrders.end(); ++iter ) {
    if ( orderId == iter->get()->GetOrderId() ) {
      // update position based upon current position and what is executing
      //   decrease position when execution is opposite position
      //   increase position when execution is same as position
      m_nPositionPending -= exec.GetSize();
      if ( 0 == m_nPositionPending ) m_eOrderSidePending = OrderSide::Unknown;

      if ( 0 == order.GetQuanRemaining() ) {  // move from open to closed on order filled
        m_ClosedOrders.push_back( *iter );
        m_OpenOrders.erase( iter );
      }
      
      bOrderFound = true;
      break;
    }
  }
  if ( !bOrderFound ) {
    // need to handle the case where order was cancelled, but not in time to prevent execution
    throw std::runtime_error( "CPosition::HandleExecution doesn't have an Open Order" );
  }

  OnExecution( execution_pair_t( this, exec ) );
  
}

void CPosition::EmitStatus( std::stringstream& ssStatus ) {
  ssStatus << "Position " << m_pInstrument->GetInstrumentName() << ": "
    << "Active " << m_nPositionActive
    << ", unRPL " << m_dblUnRealizedPL 
    << ", RPL " << m_dblRealizedPL
    << ", Cmsn " << m_dblCommissionPaid
    << ", PL-C " << m_dblRealizedPL - m_dblCommissionPaid
    << std::endl
    ;
}

// process execution to convert Pending to Active

void CPosition::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table positions: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

  rtn = sqlite3_exec( pDb, 
    "create index idx_positions_portfolioid on positions( portfolioid );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating index idx_positions_portfolioid: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

int CPosition::BindDbKey( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn = sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_idPosition );
  return rtn;
}

int CPosition::BindDbVariables( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":portfolioid" ), m_idPortfolio.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":name" ), m_sName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":notes" ), m_sNotes.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":executionaccountid" ), m_sidExecutionAccount.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":dataaccountid" ), m_sidDataAccount.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":instrumentid" ), m_sInstrumentName.c_str(), -1, SQLITE_TRANSIENT );
  rtn = sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":ordersidepending" ), m_eOrderSidePending );
  rtn = sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":ordersideactive" ), m_eOrderSideActive );
  rtn = sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":quantitypending" ), m_nPositionPending );
  rtn = sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":quantityactive" ), m_nPositionActive );
  rtn = sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":constructedvalue" ), m_dblConstructedValue );
  rtn = sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":marketvalue" ), m_dblMarketValue );
  rtn = sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":plunrealized" ), m_dblUnRealizedPL );
  rtn = sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":plrealized" ), m_dblRealizedPL );
  rtn = sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":commissionpaid" ), m_dblCommissionPaid );
  return rtn;
}

} // namespace tf
} // namespace ou
