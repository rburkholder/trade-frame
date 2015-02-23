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

#include "stdafx.h"

#include "VuTreePortfolioPositionOrder.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
VuTreePortfolioPositionOrder::VuTreePortfolioPositionOrder( ModelPortfolioPositionOrderExecution* pMPPOE )
  : VuBase(), m_pdvmdlPPOE( pMPPOE ) {
    Construct();
}

VuTreePortfolioPositionOrder::VuTreePortfolioPositionOrder(
  ModelPortfolioPositionOrderExecution* pMPPOE,
  wxWindow *parent, wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style,
  const wxValidator& validator ) 
  : VuBase( parent, id, pos, size, style, validator ), m_pdvmdlPPOE( pMPPOE )          
{
    Construct();
}

VuTreePortfolioPositionOrder::~VuTreePortfolioPositionOrder(void) {
}

void VuTreePortfolioPositionOrder::Construct( void ) {

  AssociateModel( m_pdvmdlPPOE.get() );

//  structPopulateColumns f( this );
//  m_pdvmdlPPOE.get()->IterateColumnNames( f );
  wxDataViewColumn* col = new wxDataViewColumn( "Portfolio Manager", new wxDataViewTextRenderer(), 0 );
  col->SetAlignment( wxAlignment::wxALIGN_LEFT );
  this->AppendColumn( col );

  // need to work on auto-width setting to match width of space available for tree
}

} // namespace tf
} // namespace ou
