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

#include "VuPositions.h"

VuPositions::VuPositions(void): VuBase() {
  Construct();
}

VuPositions::VuPositions( wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : VuBase( parent, id, pos, size, style, validator ) {
    Construct();
}

VuPositions::~VuPositions(void) {
}

void VuPositions::Construct( void ) {
        
  m_dvmdlPositions = new dvmdlPositions_t;
  AssociateModel( m_dvmdlPositions.get() );

  structPopulateColumns f( this );
  m_dvmdlPositions.get()->IterateColumnNames( f );

}
