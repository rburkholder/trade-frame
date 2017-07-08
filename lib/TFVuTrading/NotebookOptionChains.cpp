/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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
/* 
 * File:   WinOptionChains.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 2, 2017, 8:16 PM
 */

#include "NotebookOptionChains.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

WinOptionChains::WinOptionChains() {
  Init();
}

WinOptionChains::WinOptionChains( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

WinOptionChains::~WinOptionChains() {
}

wxBitmap WinOptionChains::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon WinOptionChains::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

void WinOptionChains::Save( boost::archive::text_oarchive& oa) {
  //oa & m_splitter->GetSashPosition();
  //m_pTreeOps->Save<TreeItemRoot>( oa );
}

void WinOptionChains::Load( boost::archive::text_iarchive& ia) {
  //int pos;
  //ia & pos;
  //m_splitter->SetSashPosition( pos );
  //m_pTreeOps->Load<TreeItemRoot>( ia );
}


} // namespace tf
} // namespace ou
