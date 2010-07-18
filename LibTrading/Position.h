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

// Multiple position records grouped together would be a multi-legged instrument -- not sure how to construct this yet
// A Portfolio should be a collection of position records, whether individual positions, or multi-positions
// check that orders for both sell side and buy side are not opened simultaneously

class CPosition {
public:

  typedef boost::shared_ptr<CPosition> pPosition_t;

  CPosition( CInstrument::pInstrument_ref );
  CPosition( CInstrument::pInstrument_ref, const std::string& sNotes );
  ~CPosition(void);

  const std::string& Notes( void ) { return m_sNotes; };
  void Append( std::string& sNotes ) { m_sNotes += sNotes; };

  COrder::pOrder_ref PlaceOrder( // market
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity
    );
  COrder::pOrder_ref PlaceOrder( // limit or stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1
    );
  COrder::pOrder_ref PlaceOrder( // limit and stop
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide,
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2
    );
  void CancelOrders( void );
  void ClosePosition( void );

protected:

  CInstrument::pInstrument_t m_pInstrument;
  std::string m_sNotes;
  int m_nPosition;
  double m_dblPrice;
  double m_dblAverageCost;
  double m_dblCommission;

  std::vector<COrder::pOrder_t> m_OpenOrders;
  std::vector<COrder::pOrder_t> m_ClosedOrders;

private:
};
