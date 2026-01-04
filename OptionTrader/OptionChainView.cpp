/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    OptionChainView.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:21
 */

#include <wx/icon.h>

#include "OptionChainView.hpp"

OptionChainView::OptionChainView()
: wxGrid() {
  Init();
}

OptionChainView::OptionChainView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxGrid()
{
  Init();
  Create(parent, id, pos, size, style, name );
}

OptionChainView::~OptionChainView() {
}

void OptionChainView::Init() {
}

bool OptionChainView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxGrid::Create(parent, id, pos, size, style );

  CreateControls();

  return true;
}

void OptionChainView::CreateControls() {

  Bind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this );

  //auto fRendererRight = []()->wxDataViewTextRenderer*{
  //  auto renderer = new wxDataViewTextRenderer();
  //  renderer->SetAlignment( wxALIGN_RIGHT );
  //  return renderer;
  //};

  SetDefaultColSize(50);
  SetDefaultRowSize(20);
  SetColLabelSize(22);
  SetRowLabelSize(50);

  EnableEditing( false );
  DisableDragRowSize();

  {
    // Todo: add intrinsic value, premium value for call & put, include last price?
    // Todo: use trades to indicate possible trade opportunities, as they may represent knowlegeable actors
    //       check if they correspond to peaks/valleys in the underlying
  }

  // the following does not work, will need to use wxGrid to provide column & header colours
  //wxColourDatabase cdb;

  //wxItemAttr attr1;
  //wxItemAttr attr2( attr1.GetTextColour(), attr1.GetBackgroundColour(), attr1.GetFont() );
  //attr2.SetBackgroundColour( cdb.Find( "RED" ) );
  //SetHeaderAttr( attr2 );
}

int OptionChainView::GetFirstVisibleRow() const {
  return GetFirstFullyVisibleRow();
}

int OptionChainView::GetVisibleRowCount() const {
  const int ixFirstRow( GetFirstFullyVisibleRow() );
  int nRows {};
  if ( -1 != ixFirstRow ) {
    int ixRow( ixFirstRow );
    const int cntRows( GetTable()->GetNumberRows() );
    while (
      ( IsVisible( ixRow, OptionChainModel::col_Strike, false ) )
      && ( cntRows > ixRow )
    ) {
      nRows++;
      ixRow++;
    }
  }
  return nRows;
}

void OptionChainView::SetVisible( int ixRow ) {
  MakeCellVisible( ixRow, OptionChainModel::col_Strike );  // nay need a col number
}

void OptionChainView::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this ) );

  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap OptionChainView::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon OptionChainView::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
