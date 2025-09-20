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

#include <algorithm>

#include <TFOptions/Option.h>

#include "OrderManager.h"
#include "Position.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

Position::Position( pInstrument_t& pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider,
  const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
  const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm )
: m_pExecutionProvider( pExecutionProvider ), //m_pDataProvider( pDataProvider ),
  m_dblMultiplier( 1 ), //m_bConnectedToDataProvider( false ),
  m_row( idPortfolio, sName, pInstrument->GetInstrumentName(), idExecutionAccount, idDataAccount, sAlgorithm )
{
  ConstructWatch( pInstrument, pDataProvider );
  Construction();
}

Position::Position( pWatch_t pWatch, pProvider_t pExecutionProvider )
: m_dblMultiplier( 1 ),
  m_pWatch( pWatch ),
  m_pExecutionProvider( pExecutionProvider )
{
  assert( 0 != m_pWatch.use_count() );
  assert( nullptr != m_pWatch.get() );  // this is probably a better check than use_count
  assert( 0 != m_pWatch->GetInstrument().use_count() );
  assert( 0 != m_pWatch->GetProvider().use_count() );
  Construction();
}

Position::Position( pWatch_t pWatch, pProvider_t pExecutionProvider,
  const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
  const idPortfolio_t& idPortfolio, const std::string& sNamePosition, const std::string& sAlgorithm )
: m_pExecutionProvider( pExecutionProvider ), //m_pDataProvider( pDataProvider ),
  m_dblMultiplier( 1 ), //m_bConnectedToDataProvider( false ),
  m_pWatch( pWatch ),
  m_row( idPortfolio, sNamePosition, pWatch->GetInstrument()->GetInstrumentName(), idExecutionAccount, idDataAccount, sAlgorithm )
{
  assert( 0 != m_pWatch.use_count() );
  assert( nullptr != m_pWatch.get() );  // this is probably a better check than use_count
  assert( 0 != m_pWatch->GetInstrument().use_count() );
  assert( 0 != m_pWatch->GetProvider().use_count() );
  Construction();
}

Position::Position(
  pWatch_t pWatch, pProvider_t pExecutionProvider,
  const idPortfolio_t& idPortfolio, const std::string& sNamePosition, const std::string& sAlgorithm )
: m_pExecutionProvider( pExecutionProvider ), //m_pDataProvider( pDataProvider ),
  m_dblMultiplier( 1 ), //m_bConnectedToDataProvider( false ),
  m_pWatch( pWatch ),
  m_row( idPortfolio, sNamePosition, pWatch->GetInstrument()->GetInstrumentName(), pExecutionProvider->GetName(), pWatch->GetProvider()->GetName(), sAlgorithm )
{
  assert( 0 != m_pWatch.use_count() );
  assert( nullptr != m_pWatch.get() );  // this is probably a better check than use_count
  assert( 0 != m_pWatch->GetInstrument().use_count() );
  assert( 0 != m_pWatch->GetProvider().use_count() );
  Construction();
}

Position::Position( pInstrument_t& pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider )
: m_pExecutionProvider( pExecutionProvider ), //m_pDataProvider( pDataProvider ),
  m_dblMultiplier( 1 )
{
  ConstructWatch( pInstrument, pDataProvider );
  Construction();
}

Position::Position(
  pInstrument_t& pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const std::string& sNotes )
: m_pExecutionProvider( pExecutionProvider ), //m_pDataProvider( pDataProvider ),
  m_dblMultiplier( 1 )
{
  m_row.sNotes = sNotes;
  ConstructWatch( pInstrument, pDataProvider );
  Construction();
}

Position::Position(
  pInstrument_t& pInstrument, pProvider_t pExecutionProvider, pProvider_t pDataProvider, const TableRowDef& row )
: m_row( row ),
  m_pExecutionProvider( pExecutionProvider ),
  m_dblMultiplier( 1 )
{
  ConstructWatch( pInstrument, pDataProvider );
  Construction();
}

Position::Position( const TableRowDef& row )
: m_row( row ),
  m_dblMultiplier( 1 )
{
}

Position::Position()
: m_dblMultiplier( 1 )
{
}

void Position::ConstructWatch( pInstrument_t& pInstrument, pProvider_t pDataProvider ) {
  assert( nullptr == m_pWatch.get() );
  assert( nullptr != pInstrument.get() );
  assert( nullptr != pDataProvider.get() );
  switch ( pInstrument->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Option:
    case ou::tf::InstrumentType::FuturesOption:
      m_pWatch = std::make_shared<ou::tf::option::Option>( pInstrument, pDataProvider );
      break;
    default:
      m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pDataProvider );
      break;
  }
}

void Position::Construction() {
  assert( nullptr != m_pWatch.get() );
  assert( nullptr != m_pWatch->GetProvider().get() );  // new validation, and could trip up some code
  m_dblMultiplier = m_pWatch->GetInstrument()->GetMultiplier();
  //std::cout << m_pWatch->GetInstrumentName() << " multiplier=" << m_dblMultiplier << std::endl;
//  HandleQuote( m_pWatch->LastQuote() ); // ensure we have at least one quote (some options don't quote on very otm) -- watch not yet activated
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Position::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Position::HandleTrade ) );
  m_pWatch->StartWatch();
}

void Position::Set( pInstrument_t& pInstrument, pProvider_t& pExecutionProvider, pProvider_t& pDataProvider ) {

  m_pExecutionProvider = pExecutionProvider;

  ConstructWatch( pInstrument, pDataProvider );
  Construction();

}

Position::~Position() {

  if ( nullptr != m_pWatch.get() ) {
    m_pWatch->StopWatch();
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &Position::HandleQuote ) );
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &Position::HandleTrade ) );
  }

  for ( std::vector<pOrder_t>::iterator iter = m_vAllOrders.begin(); iter != m_vAllOrders.end(); ++iter ) {
    iter->get()->OnOrderCancelled.Remove( MakeDelegate( this, &Position::HandleCancellation ) );
    iter->get()->OnCommission.Remove( MakeDelegate( this, &Position::HandleCommission ) );
    iter->get()->OnExecution.Remove( MakeDelegate( this, &Position::HandleExecution ) );
  }

  m_vOpenOrders.clear();
  m_vClosedOrders.clear();
  m_vAllOrders.clear();

}

void Position::HandleQuote( quote_t& quote ) {

  // TODO: use a flag to determine if to use zero based or not?
  //       maybe allow on optiosn, but not futures/equity?
  if ( ( 0 == quote.Bid() ) && ( 0 == quote.Ask() ) ) {
    return; // some very otm options will have a 0 bid
  }

  OnQuote( quote );  // note OnQuotePostProcess deals with post-update notification

  bool bProcessed( false );
  double dblMarketValue {};
  double dblPreviousUnRealizedPL( m_row.dblUnRealizedPL );

  switch ( m_row.eOrderSideActive ) {
    case OrderSide::Buy:
      dblMarketValue = m_row.nPositionActive * quote.Bid() * m_dblMultiplier;
      m_row.dblUnRealizedPL = dblMarketValue - m_row.dblConstructedValue;
      bProcessed = true;
      break;
    case OrderSide::Sell:
      dblMarketValue = - ( m_row.nPositionActive * quote.Ask() ) * m_dblMultiplier;
      m_row.dblUnRealizedPL = dblMarketValue - m_row.dblConstructedValue;
      bProcessed = true;
      break;
  }

  if ( bProcessed ) {
    OnQuotePostProcess( quote_pair_t( *this, quote ) );
    if ( dblPreviousUnRealizedPL != m_row.dblUnRealizedPL ) {
      OnUnRealizedPL( PositionDelta_delegate_t( *this, dblPreviousUnRealizedPL, m_row.dblUnRealizedPL ) );
    }
  }

}

void Position::HandleTrade( trade_t& trade ) {
  OnTrade( trade );
}

Order::pOrder_t Position::PlaceOrder( // market
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity
) {
  pOrder_t pOrder = ConstructOrder( eOrderType, eOrderSide, nOrderQuantity );
  PlaceOrder( pOrder );
  return pOrder;
}

Order::pOrder_t Position::PlaceOrder( // limit or stop
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity,
  double dblPrice1
) {
  pOrder_t pOrder = ConstructOrder( eOrderType, eOrderSide, nOrderQuantity, dblPrice1 );
  PlaceOrder( pOrder );
  return pOrder;
}

Order::pOrder_t Position::PlaceOrder( // limit and stop
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity,
  double dblPrice1,
  double dblPrice2
) {
  pOrder_t pOrder = ConstructOrder( eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2 );
  PlaceOrder( pOrder );
  return pOrder;
}

Order::pOrder_t Position::ConstructOrder( // market
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity
) {
  assert( OrderSide::Unknown != eOrderSide );
  assert( OrderType::Market == eOrderType );
  assert( nullptr != m_pWatch.get() );
  pOrder_t pOrder
   = OrderManager::LocalCommonInstance().ConstructOrder( m_pWatch->GetInstrument(), eOrderType, eOrderSide, nOrderQuantity, m_row.idPosition );
  Register( pOrder );
  return pOrder;
}

Order::pOrder_t Position::ConstructOrder( // limit or stop
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity,
  double dblPrice1
) {
  assert( OrderSide::Unknown != eOrderSide );
  assert( ( OrderType::Limit == eOrderType) || ( OrderType::Stop == eOrderType ) || ( OrderType::Trail == eOrderType ) );
  assert( nullptr != m_pWatch.get() );
  pOrder_t pOrder
   = OrderManager::LocalCommonInstance().ConstructOrder( m_pWatch->GetInstrument(), eOrderType, eOrderSide, nOrderQuantity, dblPrice1, m_row.idPosition );
  Register( pOrder );
  return pOrder;
}

Order::pOrder_t Position::ConstructOrder( // limit and stop
  OrderType::EOrderType eOrderType,
  OrderSide::EOrderSide eOrderSide,
  boost::uint32_t nOrderQuantity,
  double dblPrice1,
  double dblPrice2
) {
  assert( OrderSide::Unknown != eOrderSide );
  assert(
       ( OrderType::StopLimit == eOrderType )
    || ( OrderType::Trail == eOrderType )
    || ( OrderType::TrailLimit == eOrderType )
    );
  assert( nullptr != m_pWatch.get() );
  pOrder_t pOrder
   = OrderManager::LocalCommonInstance().ConstructOrder( m_pWatch->GetInstrument(), eOrderType, eOrderSide, nOrderQuantity, dblPrice1, dblPrice2, m_row.idPosition );
  Register( pOrder );
  return pOrder;
}

void Position::PlaceOrder( pOrder_t pOrder ) {

//  if ( OrderSide::Unknown != m_row.eOrderSidePending ) { // ensure new order matches existing orders
//    if ( ( m_row.eOrderSidePending != pOrder->GetOrderSide() ) && ( OrderType::Market == pOrder->GetOrderType() ) ) {  // check only for market orders, not limit orders?
//      throw std::runtime_error( "Position::PlaceOrder, new order does not match pending order type" );
//    }
//  }

  if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = pOrder->GetOrderSide();  // first to set non-zero gives us our predominant side

  // this is going to be problematic if we post a Combo or Bracket order set
  m_row.nPositionPending += pOrder->GetQuanOrdered();
  const auto id = pOrder->GetOrderId();
  if ( 0 == std::count_if(
    m_vAllOrders.begin(), m_vAllOrders.end(),  // this may not be efficient for large order lists, maybe use a map sometime
    [id]( pOrder_t& pOrder ){ return ( id == pOrder->GetOrderId() ); } ) ) {
      Register( pOrder );
    }
  OrderManager::LocalCommonInstance().PlaceOrder( &(*m_pExecutionProvider), pOrder );

  OnPositionChanged( *this );
}

void Position::Register( pOrder_t pOrder ) {
  m_vAllOrders.push_back( pOrder );
  m_vOpenOrders.push_back( pOrder );
  pOrder->OnExecution.Add( MakeDelegate( this, &Position::HandleExecution ) );
  pOrder->OnCommission.Add( MakeDelegate( this, &Position::HandleCommission ) );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Position::HandleCancellation ) );
}

void Position::UpdateOrder( pOrder_t pOrder ) {

  //if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = pOrder->GetOrderSide();  // first to set non-zero gives us our predominant side

  //m_row.nPositionPending += pOrder->GetQuantity();
  //m_vAllOrders.push_back( pOrder );
  //m_vOpenOrders.push_back( pOrder ); // TODO: check to see if still an Open Order?
  //pOrder->OnExecution.Add( MakeDelegate( this, &Position::HandleExecution ) );
  //pOrder->OnCommission.Add( MakeDelegate( this, &Position::HandleCommission ) );
  //pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Position::HandleCancellation ) );
  OrderManager::LocalCommonInstance().UpdateOrder( &(*m_pExecutionProvider), pOrder );

  //OnPositionChanged( *this );  // TODO: should this be invoked?
}

void Position::CancelOrders() {
  // may have a problem getting out of sync with broker if orders are cancelled by broker
  // todo:  on power up, need to relink these active orders back in with the position
  // or only use DAY orders
  //for ( std::vector<pOrder_t>::iterator iter = m_vOpenOrders.begin(); iter != m_vOpenOrders.end(); ++iter ) {
  //  CancelOrder( iter );  // this won't work as the iterator is invalidated with each order removal
  //}
  std::for_each(
    m_vOpenOrders.begin(), m_vOpenOrders.end(), // need to be aware of multi-thread invalidating interator?
    [this](pOrder_t& pOrder){
      CancelOrder( pOrder );
    });
}

void Position::CancelOrder( idOrder_t idOrder ) {
  for ( vOrders_t::iterator iter = m_vOpenOrders.begin(); iter != m_vOpenOrders.end(); ++iter ) {
    if ( idOrder == iter->get()->GetOrderId() ) {
      CancelOrder( iter );
      break;
    }
  }
}

void Position::HandleCancellation( const Order& order ) {
  Order::idOrder_t idOrder = order.GetOrderId();
  for ( vOrders_t::iterator iter = m_vOpenOrders.begin(); iter != m_vOpenOrders.end(); ++iter ) {
    if ( idOrder == iter->get()->GetOrderId() ) {
      if ( m_row.nPositionPending >= iter->get()->GetQuanRemaining() ) {
        m_row.nPositionPending -= iter->get()->GetQuanRemaining();
        if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = OrderSide::Unknown;
        //CancelOrder( iter );
        m_vClosedOrders.push_back( *iter );
        m_vOpenOrders.erase( iter );
        break;
      }
      else {
        throw std::runtime_error( "problems" );
      }
    }
  }
  OnPositionChanged( *this );
}

void Position::CancelOrder( vOrders_iter_t iter ) {
  CancelOrder( *iter );
}

void Position::CancelOrder( pOrder_t& pOrder ) {
  OrderManager::LocalCommonInstance().CancelOrder( pOrder->GetOrderId() );
}

void Position::ClosePosition( OrderType::EOrderType eOrderType ) {
  // should outstanding orders be auto cancelled?
  // position is closed with a market order, can try to do limit in the future, but need active market data
  if ( 0 != m_row.nPositionActive ) {
    switch ( m_row.eOrderSideActive ) {
      case OrderSide::Buy:
        PlaceOrder( eOrderType, OrderSide::Sell, m_row.nPositionActive );
        break;
      case OrderSide::Sell:
        PlaceOrder( eOrderType, OrderSide::Buy, m_row.nPositionActive );
        break;
      case OrderSide::Unknown:
        break;
    }
  }
}

void Position::UpdateRowValues( double price, boost::uint32_t quan, OrderSide::EOrderSide side ) {

  // can only be called by HandleExecution as HandleExecution uses the before and after values of RealizedPL

  double dblAvgConstructedCost = 0;
  double dblRealizedPL = 0;
  bool bTwoStep = false;
  boost::uint32_t nRemaining( 0 );

  switch ( m_row.eOrderSideActive ) {
    case OrderSide::Buy:  // existing is long
      switch ( side ) {
        case OrderSide::Buy:  // increase long
          m_row.nPositionActive += quan;
          m_row.dblConstructedValue += quan * price * m_dblMultiplier;
          break;
        case OrderSide::Sell:  // decrease long
          if ( quan > m_row.nPositionActive ) {
            bTwoStep = true;
            nRemaining = quan - m_row.nPositionActive;
            quan = m_row.nPositionActive;
          }
          dblAvgConstructedCost = m_row.dblConstructedValue / ( m_row.nPositionActive * m_dblMultiplier );
          dblRealizedPL = quan * ( price - dblAvgConstructedCost ) * m_dblMultiplier;
          m_row.dblRealizedPL += dblRealizedPL;
          m_row.nPositionActive -= quan;
          m_row.dblConstructedValue -= quan * dblAvgConstructedCost * m_dblMultiplier;
          //m_dblConstructedValue -= ( exec.GetSize() * exec.GetPrice() * m_dblMultiplier - dblRealizedPL );
          if ( 0 == m_row.nPositionActive ) {
            m_row.eOrderSideActive = OrderSide::Unknown;
            m_row.dblUnRealizedPL = 0.0;
            dblAvgConstructedCost = 0.0;
          }
          if ( bTwoStep ) {
            UpdateRowValues( price, nRemaining, side );
          }
          break;
      }
      break;
    case OrderSide::Sell:  // existing is short
      switch ( side ) {
        case OrderSide::Sell:  // increase short
          m_row.nPositionActive += quan;
          m_row.dblConstructedValue -= quan * price * m_dblMultiplier;
          break;
        case OrderSide::Buy:  // decrease short
          if ( quan > m_row.nPositionActive ) {
            bTwoStep = true;
            nRemaining = quan - m_row.nPositionActive;
            quan = m_row.nPositionActive;
          }
          dblAvgConstructedCost = m_row.dblConstructedValue / ( m_row.nPositionActive * m_dblMultiplier );
          dblRealizedPL = quan * ( - price - dblAvgConstructedCost ) * m_dblMultiplier;
          m_row.dblRealizedPL += dblRealizedPL;
          m_row.nPositionActive -= quan;
          m_row.dblConstructedValue -= quan * dblAvgConstructedCost * m_dblMultiplier;
          //m_dblConstructedValue += ( exec.GetSize() * exec.GetPrice() * m_dblMultiplier + dblRealizedPL );  // is this correctly calculated?
          if ( 0 == m_row.nPositionActive ) {
            m_row.eOrderSideActive = OrderSide::Unknown;
            m_row.dblUnRealizedPL = 0.0;
            dblAvgConstructedCost = 0.0;
          }
          if ( bTwoStep ) {
            UpdateRowValues( price, nRemaining, side );
          }
          break;
      }
      break;
    case OrderSide::Unknown:  // no active position, so start here
      assert( 0 == m_row.nPositionActive );
      m_row.eOrderSideActive = side;
      m_row.nPositionActive = quan;
      switch ( m_row.eOrderSideActive ) {
        case OrderSide::Buy:
          m_row.dblConstructedValue += quan * price * m_dblMultiplier;
          break;
        case OrderSide::Sell:
          m_row.dblConstructedValue -= quan * price * m_dblMultiplier;
          break;
      }
      break;
  }

}

// before entry to this method, sanity check:  side on execution is same as side on order
void Position::HandleExecution( const std::pair<const Order&, const Execution&>& status ) {

  // should be able to calculate profit/loss & position cost as executions are encountered
  // should be able to calculate position cost basis as position is updated (with and without commissions)
  // will need market feed in order to calculate profit/loss  -- handled in the OnQuote method

  const Order& order( status.first );
  const Execution& exec( status.second );
  const Order::idOrder_t orderId( order.GetOrderId() );

  const double dblOldRealizedPL = m_row.dblRealizedPL;
  const double dblOldUnRealizedPL = m_row.dblUnRealizedPL;

  //std::cout << "Position Exec: " << exec.GetSize() << "," << exec.GetPrice() << std::endl;

  // update position, regardless of whether we see order open or closed
  UpdateRowValues( exec.GetPrice(), exec.GetSize(), exec.GetOrderSide() );

  if ( ( 0 == m_row.nPositionActive ) && ( OrderSide::Unknown != m_row.eOrderSideActive ) ) {
    std::cout << "problems" << std::endl;
  }

  // check that we think that the order is still active
  bool bOrderFound = false;
  for ( std::vector<pOrder_t>::iterator iter = m_vOpenOrders.begin(); iter != m_vOpenOrders.end(); ++iter ) {
    if ( orderId == iter->get()->GetOrderId() ) {
      // update position based upon current position and what is executing
      //   decrease position when execution is opposite position
      //   increase position when execution is same as position
      m_row.nPositionPending -= exec.GetSize();
      if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = OrderSide::Unknown;

      if ( 0 == order.GetQuanRemaining() ) {  // move from open to closed on order filled
        m_vClosedOrders.push_back( *iter );
        m_vOpenOrders.erase( iter );
      }

      bOrderFound = true;
      break;
    }
  }
  if ( !bOrderFound ) {
    // need to handle the case where order was cancelled, but not in time to prevent execution
    throw std::runtime_error( "Position::HandleExecution doesn't have an Open Order" );
  }

  OnUnRealizedPL( PositionDelta_delegate_t( *this, dblOldUnRealizedPL, m_row.dblUnRealizedPL ) );  // used by portfolio updates

  OnExecutionRaw( execution_pair_t( *this, exec ) );
  OnUpdateExecutionForPortfolioManager( *this );

  OnExecution( PositionDelta_delegate_t( *this, dblOldRealizedPL, m_row.dblRealizedPL ) );  // used by portfolio updates

  OnPositionChanged( *this );

}

void Position::HandleCommission( const Order& order ) {
  //std::cout << "Position Comm: " << m_row.dblCommissionPaid << "," << order.GetCommission();

  double dblNewCommissionPaid = order.GetIncrementalCommission();
  //std::cout << "," << dblNewCommissionPaid << std::endl;

  if ( 0 != dblNewCommissionPaid ) {
    m_row.dblCommissionPaid += dblNewCommissionPaid;
    OnUpdateCommissionForPortfolioManager( *this );
    OnCommission( PositionDelta_delegate_t( *this, 0, dblNewCommissionPaid ) );
  }
}

std::ostream& operator<<( std::ostream& os, const Position& position ) {
  os
    << "Position " << position.m_pWatch->GetInstrument()->GetInstrumentName() << ": "
    << "Active " << position.m_row.nPositionActive
    << ", unRPL " << position.m_row.dblUnRealizedPL
    << ", RPL " << position.m_row.dblRealizedPL
    << ", Cmsn " << position.m_row.dblCommissionPaid
    << ", PL-C " << position.m_row.dblRealizedPL - position.m_row.dblCommissionPaid
    ;
  return os;
}

// process execution to convert Pending to Active

} // namespace tf
} // namespace ou
