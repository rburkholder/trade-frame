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

GridOptionDetails::GridOptionDetails(void) {
  Init();
}

GridOptionDetails::GridOptionDetails( 
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle ) {
  Init();
  Create(parent, id, pos, size, style, sTitle );
}

GridOptionDetails::~GridOptionDetails(void) {
}

void GridOptionDetails::Init( void ) {
  m_pimpl.reset( new GridOptionDetails_impl( *this ) ); 
}

bool GridOptionDetails::Create( 
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
  
  Init();
  
  bool bReturn = Create(parent, id, pos, size, style, name );

  return bReturn;
}

void GridOptionDetails::UpdateCallGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdateCallGreeks( strike, greek );
}

void GridOptionDetails::UpdateCallQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdateCallQuote( strike, quote );
}

void GridOptionDetails::UpdateCallTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdateCallTrade( strike, trade );
}

void GridOptionDetails::UpdatePutGreeks( double strike, ou::tf::Greek& greek ) {
  m_pimpl->UpdatePutGreeks( strike, greek );
}

void GridOptionDetails::UpdatePutQuote( double strike, ou::tf::Quote& quote ) {
  m_pimpl->UpdatePutQuote( strike, quote );
}

void GridOptionDetails::UpdatePutTrade( double strike, ou::tf::Trade& trade ) {
  m_pimpl->UpdatePutTrade( strike, trade );
}

template void GridOptionDetails::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar, 
    const unsigned int file_version
);

template void GridOptionDetails::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar, 
    const unsigned int file_version
);

wxBitmap GridOptionDetails::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon GridOptionDetails::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou