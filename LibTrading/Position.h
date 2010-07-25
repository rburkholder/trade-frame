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

#include "boost/shared_ptr.hpp"

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "Order.h"
#include "ProviderInterface.h"

// Multiple position records grouped together would be a multi-legged instrument, aka Combo
//   -- not sure how to construct this yet
// A Portfolio should be a collection of position records, whether individual positions, or Combos
// check that orders for both sell side and buy side are not opened simultaneously
// a position is provider dependent, ie, only one provider per position
// Create Delegates so trade and market data updates propogate to combo and portfolio

class CPosition {
public:

  typedef boost::shared_ptr<CPosition> pPosition_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef CProviderInterfaceBase::pProvider_ref pProvider_ref;

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_ref pInstrument_ref;

  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::pOrder_ref pOrder_ref;

  CPosition( pInstrument_ref, pProvider_ref pExecutionProvider, pProvider_ref pDataProvider );
  CPosition( pInstrument_ref, pProvider_ref pExecutionProvider, pProvider_ref pDataProvider, const std::string& sNotes );
  ~CPosition(void);

  const std::string& Notes( void ) { return m_sNotes; };
  void Append( std::string& sNotes ) { m_sNotes += sNotes; };

  COrder::pOrder_t PlaceOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity
    );
  COrder::pOrder_t PlaceOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1
    );
  COrder::pOrder_t PlaceOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );
  void CancelOrders( void );
  void ClosePosition( void );

protected:

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pDataProvider;

  pInstrument_t m_pInstrument;
  std::string m_sNotes;

  // all pending orders must be on the same side
  // pending orders need to cancelled in order to change sides
  // use an opposing position if playing both sides of the market
  OrderSide::enumOrderSide m_eOrderSidePending;  
  unsigned long m_nPositionPending;

  // indicates whether we are in a long or short position
  OrderSide::enumOrderSide m_eOrderSideActive;  
  unsigned long m_nPositionActive;

  double m_dblAverageCostPerShare;  // based upon position trades
  double m_dblConstructedValue;  // based upon position trades
  double m_dblMarketValue;  // based upon market quotes

  double m_dblUnRealizedPL;  // based upon market quotes
  double m_dblRealizedPL;  // based upon position trades

  double m_dblCommissionPaid;

  std::vector<pOrder_t> m_OpenOrders;  // active orders waiting to be executed or cancelled
  std::vector<pOrder_t> m_ClosedOrders;  // orders that have executed or have cancelled
  std::vector<pOrder_t> m_AllOrders;  // keeps track of all orders in case we have to search both lists

private:

  void HandleExecution( std::pair<const COrder&, const CExecution&>& );

  void ProcessOrder( pOrder_t pOrder );

};
