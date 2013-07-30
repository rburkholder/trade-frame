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

#include "StdAfx.h"

#include "VuOrders.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

VuOrders::VuOrders(void): VuBase() {
  Construct();
}

VuOrders::VuOrders( wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : VuBase( parent, id, pos, size, style, validator ) {
    Construct();
}

VuOrders::~VuOrders(void) {
}

void VuOrders::Construct( void ) {
        
  m_pdvmdlOrders = new dvmdlOrders_t;
  AssociateModel( m_pdvmdlOrders.get() );

  structPopulateColumns f( this );
  m_pdvmdlOrders.get()->IterateColumnNames( f );

}

} // namespace tf
} // namespace ou
