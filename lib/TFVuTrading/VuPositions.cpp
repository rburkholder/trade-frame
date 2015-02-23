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

#include "stdafx.h"

#include "VuPositions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

VuPositions::VuPositions( ModelPosition* pmp )
  : VuBase(), m_dvmdlPositions( pmp )
{
  Construct();
}

VuPositions::VuPositions( ModelPosition* pmp, 
  wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : VuBase( parent, id, pos, size, style, validator ), m_dvmdlPositions( pmp )
{
    Construct();
}

VuPositions::~VuPositions(void) {
}

void VuPositions::Construct( void ) {
        
//  m_dvmdlPositions = new dvmdlPositions_t;
  AssociateModel( m_dvmdlPositions.get() );

  structPopulateColumns f( this );
  m_dvmdlPositions.get()->IterateColumnNames( f );

}

} // namespace tf
} // namespace ou
