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
  
TreePortfolioPositionOrder::TreePortfolioPositionOrder( ModelPortfolioPositionOrderExecution* pMPPOE )
  : VuBase(), m_pdvmdlPPOE( pMPPOE ) {
}

TreePortfolioPositionOrder::TreePortfolioPositionOrder(
  ModelPortfolioPositionOrderExecution* pMPPOE,
  wxWindow *parent, wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style,
  const wxValidator& validator ) 
  : VuBase( parent, id, pos, size, style, validator ), m_pdvmdlPPOE( pMPPOE )          
{
}

TreePortfolioPositionOrder::~TreePortfolioPositionOrder(void) {
}

void TreePortfolioPositionOrder::Construct( void ) {
//  m_pdvmdlPortfolios = new dvmdlPorfolios_t;
  AssociateModel( m_pdvmdlPPOE.get() );

//  structPopulateColumns f( this );
//  m_pdvmdlPortfolios.get()->IterateColumnNames( f );
}

} // namespace tf
} // namespace ou
