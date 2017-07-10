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
 * File:   NotebookOptionChains.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 2, 2017, 8:16 PM
 */

#include <boost/lexical_cast.hpp>

#include "NotebookOptionChains.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

NotebookOptionChains::NotebookOptionChains(): wxNotebook() {
  Init();
}

NotebookOptionChains::NotebookOptionChains( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxNotebook( parent, id, pos, size, style, name )
{
  Init();
  //Create(parent, id, pos, size, style);
}

NotebookOptionChains::~NotebookOptionChains() {
}

void NotebookOptionChains::Init() {
}

bool NotebookOptionChains::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {
  
  Init();
  
  bool bReturn = Create(parent, id, pos, size, style, name );

  Bind( wxEVT_NOTEBOOK_PAGE_CHANGING, &NotebookOptionChains::OnPageChanging, this );
  Bind( wxEVT_NOTEBOOK_PAGE_CHANGED, &NotebookOptionChains::OnPageChanged, this );
  
  return bReturn;
}

void NotebookOptionChains::SetName( const std::string& sName ) {
  
}

void NotebookOptionChains::Clear() {
  m_mapOptionExpiry.clear();
}

void NotebookOptionChains::Add( boost::gregorian::date date, double strike, ou::tf::OptionSide::enumOptionSide side, const std::string& sSymbol ) {
  
  mapOptionExpiry_t::iterator iterExpiry = m_mapOptionExpiry.find( date );
  if ( m_mapOptionExpiry.end() == iterExpiry ) {
    std::string sDate = boost::lexical_cast<std::string>( date.year() ) + "/";
    sDate += date.month() < 10 ? "0" : "" + boost::lexical_cast<std::string>( date.month() ) + "/";
    sDate += date.day() < 10 ? "0" : "" + boost::lexical_cast<std::string>( date.day() );
    // may need a sizer to go along with this
    auto* p = new GridOptionDetails( this, -1, wxDefaultPosition, wxDefaultSize, 0, sSymbol );
    size_t nPage( m_mapOptionExpiry.size() );
    InsertPage( nPage, p, sDate );
    iterExpiry = m_mapOptionExpiry.insert( 
      m_mapOptionExpiry.begin(), mapOptionExpiry_t::value_type( date, Tab( nPage, sDate, p ) ) );
  }
  
  mapStrike_t& mapStrike( iterExpiry->second.mapStrike ); // assumes single thread
  
  mapStrike_t::iterator iterStrike = mapStrike.find( strike );
  if ( mapStrike.end() == iterStrike ) {
    iterStrike = mapStrike.insert( mapStrike.begin(), mapStrike_t::value_type( strike, Row( mapStrike.size() ) ) );
  }
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      assert( "" == iterStrike->second.sCall );
      iterStrike->second.sCall = sSymbol;
      break;
    case ou::tf::OptionSide::Put:
      assert( "" == iterStrike->second.sPut );
      iterStrike->second.sPut = sSymbol;
      break;
  }
  
  
}

// build grids on demand, or pre-build?
void NotebookOptionChains::OnPageChanging( wxBookCtrlEvent& event ) {
  
}

void NotebookOptionChains::OnPageChanged( wxBookCtrlEvent& event ) {
  
}

void NotebookOptionChains::OnDestroy( wxWindowDestroyEvent& event ) {
  event.Skip();
}

void NotebookOptionChains::Save( boost::archive::text_oarchive& oa) {
  //oa & m_splitter->GetSashPosition();
  //m_pTreeOps->Save<TreeItemRoot>( oa );
}

void NotebookOptionChains::Load( boost::archive::text_iarchive& ia) {
  //int pos;
  //ia & pos;
  //m_splitter->SetSashPosition( pos );
  //m_pTreeOps->Load<TreeItemRoot>( ia );
}
wxBitmap NotebookOptionChains::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon NotebookOptionChains::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
