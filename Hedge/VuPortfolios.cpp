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

#include "VuPortfolios.h"

VuPortfolios::VuPortfolios(void)
  : wxDataViewCtrl()
{
  Construct();
}

VuPortfolios::VuPortfolios( wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : wxDataViewCtrl( parent, id, pos, size, style, validator ) {
  Construct();
}

VuPortfolios::~VuPortfolios(void) {
}

void VuPortfolios::Construct( void ) {
        
//  m_pdvmPortfolios = new dvmPorfolios_t;
//  AssociateModel( m_pdvmPortfolios.get() );

}
