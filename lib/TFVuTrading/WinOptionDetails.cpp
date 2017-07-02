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

#include "WinOptionDetails.h"
#include "WinOptionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

WinOptionDetails::WinOptionDetails(void) {
  Init();
}

WinOptionDetails::WinOptionDetails( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

WinOptionDetails::~WinOptionDetails(void) {
}

void WinOptionDetails::Init( void ) {
  m_pimpl.reset( new WinOptionDetails_impl( *this ) ); 
}

void WinOptionDetails::UpdateCallGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdateCallGreeks( strike, greek );
}

void WinOptionDetails::UpdateCallQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdateCallQuote( strike, quote );
}

void WinOptionDetails::UpdateCallTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdateCallTrade( strike, trade );
}

void WinOptionDetails::UpdatePutGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdatePutGreeks( strike, greek );
}

void WinOptionDetails::UpdatePutQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdatePutQuote( strike, quote );
}

void WinOptionDetails::UpdatePutTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdatePutTrade( strike, trade );
}

wxBitmap WinOptionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon WinOptionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

template void WinOptionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void WinOptionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

} // namespace tf
} // namespace ou