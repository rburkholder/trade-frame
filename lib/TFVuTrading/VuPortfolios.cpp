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

#include <wx/any.h>

#include "VuPortfolios.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

VuPortfolios::VuPortfolios(void)
  : VuBase()
{
  Construct();
}

VuPortfolios::VuPortfolios( wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : VuBase( parent, id, pos, size, style, validator ),
    item1( reinterpret_cast<void*>( 1 ) )
{
  Construct();
}

VuPortfolios::~VuPortfolios(void) {
}

void VuPortfolios::Construct( void ) {
        
  m_pdvmdlPortfolios = new dvmdlPorfolios_t;
  AssociateModel( m_pdvmdlPortfolios.get() );

  structPopulateColumns f( this );
  m_pdvmdlPortfolios.get()->IterateColumnNames( f );

  wxAny any = "test data";
//  m_pdvmdlPortfolios.get()->ChangeValue( any, item1, 0 );
//  m_pdvmdlPortfolios.get()->ItemAdded( item0, item1 );

}

} // namespace tf
} // namespace ou
