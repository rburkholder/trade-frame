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

#include "stdafx.h"

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

void PanelOptionDetails::UpdateCallGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdateCallGreeks( strike, greek );
}

void PanelOptionDetails::UpdateCallQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdateCallQuote( strike, quote );
}

void PanelOptionDetails::UpdateCallTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdateCallTrade( strike, trade );
}

void PanelOptionDetails::UpdatePutGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdatePutGreeks( strike, greek );
}

void PanelOptionDetails::UpdatePutQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdatePutQuote( strike, quote );
}

void PanelOptionDetails::UpdatePutTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdatePutTrade( strike, trade );
}

wxBitmap PanelOptionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelOptionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void PanelOptionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void PanelOptionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

} // namespace tf
} // namespace ou