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
#include <vector>

#include "boost/shared_ptr.hpp"

#include "TradingEnumerations.h"
#include "Instrument.h"
#include "Order.h"
#include "ProviderInterface.h"

// Multiple position records grouped together would be a multi-legged instrument
//   -- not sure how to construct this yet
// A Portfolio should be a collection of position records, whether individual positions, or multi-positions
// check that orders for both sell side and buy side are not opened simultaneously
// a position is provider dependent, ie, only one provider per position

class CPosition {
public:

  typedef boost::shared_ptr<CPosition> pPosition_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;
  typedef CProviderInterfaceBase::pProvider_ref pProvider_ref;

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_ref pInstrument_ref;

  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::pOrder_ref pOrder_ref;

  CPosition( pProvider_ref, pInstrument_ref );
  CPosition( pProvider_ref, pInstrument_ref, const std::string& sNotes );
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

  pProvider_t m_pProvider;
  pInstrument_t m_pInstrument;
  std::string m_sNotes;

  OrderSide::enumOrderSide m_eOrderSidePending;
  unsigned long m_nPositionPending;

  OrderSide::enumOrderSide m_eOrderSideActive;
  unsigned long m_nPositionActive;

  double m_dblPrice;
  double m_dblAverageCost;
  double m_dblCommission;

  std::vector<pOrder_t> m_OpenOrders;
  std::vector<pOrder_t> m_ClosedOrders;

  void ProcessOrder( pOrder_t pOrder );

private:
};
