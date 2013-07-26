/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                  8                    *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include "TreePortfolioPositionOrder.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
TreePortfolioPositionOrder::TreePortfolioPositionOrder(void): wxTreeCtrl() {
}

TreePortfolioPositionOrder::TreePortfolioPositionOrder(
              wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              long style,
              const wxValidator& validator,
              const wxString& name ) 
  : wxTreeCtrl( parent, id, pos, size, style, validator, name )             
{
}

TreePortfolioPositionOrder::~TreePortfolioPositionOrder(void) {
}

} // namespace tf
} // namespace ou
