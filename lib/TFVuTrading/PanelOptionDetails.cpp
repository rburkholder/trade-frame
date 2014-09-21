/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "PanelOptionDetails.h"
#include "PanelOptionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOptionDetails::PanelOptionDetails(void) {
  Init();
}

PanelOptionDetails::PanelOptionDetails( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelOptionDetails::~PanelOptionDetails(void) {
}

void PanelOptionDetails::Init( void ) {
  m_pimpl.reset( new PanelOptionDetails_impl( *this ) ); 
}

} // namespace tf
} // namespace ou