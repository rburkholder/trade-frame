/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <map>

#include <TFTrading/Position.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

  class OrdersOutstanding {
public:

  typedef ou::tf::Order::idOrder_t idOrder_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Position::pOrder_t pOrder_t;

  enum enumState {
    EStateOpenWaitingFill, EStateOpenCancelling, EStateOpen, 
    EStateProfit, EStateLoss, EStateEven, EStateClosing, EStateCancelled
  };

  struct structRoundTrip {
    enumState eState;
    pOrder_t pOrderEntry;
    pOrder_t pOrderExit;
    double dblBasis; // average price at which entry order filled
    double dblTarget; // target exit value
    double dblStop; // target stop out
    double dblSlope1, dblSlope2, dblSlope3;  // stats for post analysis
    double dblSlopeSlope1, dblSlopeSlope2;
    double dblSlopeBollingerOffset;
    structRoundTrip( void ): eState( EStateOpenWaitingFill ), dblBasis( 0.0 ) {};
    structRoundTrip( pOrder_t entry )
      : eState( EStateOpenWaitingFill ), pOrderEntry( entry ), dblBasis( 0.0 ) {};
    structRoundTrip( pOrder_t entry, double target, double stop )
      : eState( EStateOpenWaitingFill ), pOrderEntry( entry ), dblBasis( 0.0 ),
        dblTarget( target ), dblStop( stop ) {};
  };
  typedef boost::shared_ptr<structRoundTrip> pRoundTrip_t;

protected:

  typedef std::multimap<double, pRoundTrip_t> mapOrders_t;
  typedef std::pair<double, pRoundTrip_t> mapOrders_pair_t;
  typedef mapOrders_t::iterator mapOrders_iter_t;
  mapOrders_t m_mapOrdersToMatch;

  typedef std::vector<pRoundTrip_t> vCompletedRoundTrip_t;
  typedef vCompletedRoundTrip_t::const_iterator vCompletedRoundTrip_citer_t;
  vCompletedRoundTrip_t m_vCompletedRoundTrips;

public:

  OrdersOutstanding( pPosition_t pPosition );
  virtual ~OrdersOutstanding( void ) {};

  void AddOrderFilling( structRoundTrip* pTrip );  // migrate to using this instead
  void CancelAllButNEntryOrders( unsigned int n );
  void CancelAllMatchingOrders( void );
  void CancelAndCloseAllOrders( void );
  void PostMortemReport( void );

  void SetGlobalStop( double stop ) { m_dblGlobalStop = stop; };
  void ResetGlobalStop( void ) { m_dblGlobalStop = 0.0; };

  size_t GetCountOfOutstandingEntries( void ) const { return m_mapEntryOrdersFilling.size(); };
  size_t GetCountOfOutstandingMatches( void ) const { return m_mapOrdersToMatch.size(); };

  // should be protected but doesn't work there
  void HandleMatchingOrderFilled( const ou::tf::Order& order );
  void HandleMatchingOrderCancelled( const ou::tf::Order& order );

protected:

  double m_dblGlobalStop;

  pPosition_t m_pPosition;

  typedef std::map<idOrder_t, pRoundTrip_t> mapOrdersFilling_t;
  typedef mapOrdersFilling_t::iterator mapOrdersFilling_iter_t;
  mapOrdersFilling_t m_mapEntryOrdersFilling;

  boost::posix_time::time_duration m_durRoundTripTime;
  unsigned int m_cntRoundTrips;

  boost::posix_time::time_duration m_durForceRoundTripClose;

  void CheckBaseOrder( const ou::tf::Quote& quote );
  bool CancelAndCloseInProgress( void );

  void PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType, ou::tf::OrderSide::enumOrderSide, boost::uint32_t nOrderQuantity );
  void PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType, ou::tf::OrderSide::enumOrderSide, boost::uint32_t nOrderQuantity, double dblPrice1 );
  void PlaceOrder( pOrder_t& pOrder, const std::string& sDescription, ou::tf::OrderType::enumOrderType, ou::tf::OrderSide::enumOrderSide, boost::uint32_t nOrderQuantity, double dblPrice1, double dblPrice2 );
  void PlaceOrder( pOrder_t& pOrder );
  
private:

  bool m_bCancelAndCloseInProgress;
  enum stateCancelAndClose {
    CACStarted, CACWaitingForEntryCancels, CACWaitingForExitCancels, CACWaitingForMatchingCloses, CACDone
  } m_stateCancelAndClose;

  time_duration m_durOrderOpenTimeOut;

  void HandleBaseOrderFilled( const ou::tf::Order& order );
  void HandleBaseOrderCancelled( const ou::tf::Order& order );

};

class OrdersOutstandingLongs: public OrdersOutstanding {
public:
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Position::pOrder_t pOrder_t;
  OrdersOutstandingLongs( pPosition_t pPosition ): OrdersOutstanding( pPosition ) {};
  ~OrdersOutstandingLongs( void ) {};
  void HandleQuote( const ou::tf::Quote& quote );  // set from external
protected:
private:
};

class OrdersOutstandingShorts: public OrdersOutstanding {
public:
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Position::pOrder_t pOrder_t;
  OrdersOutstandingShorts( pPosition_t pPosition ): OrdersOutstanding( pPosition ) {};
  ~OrdersOutstandingShorts( void ) {};
  void HandleQuote( const ou::tf::Quote& quote );  // set from external
protected:
private:
};

} // namespace tf
} // namespace ou
