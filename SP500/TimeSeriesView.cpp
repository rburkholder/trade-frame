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
 * File:    TimeSeriesView.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 1, 2025 19:12:28
 */

#include <wx/icon.h>

#include "TimeSeriesView.hpp"

TimeSeriesView::TimeSeriesView()
: wxGrid()
{}

TimeSeriesView::TimeSeriesView(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style,
  const wxString& name )
{
  Init();
  Create(parent, id, pos, size, style, name );
}

TimeSeriesView::~TimeSeriesView() {
}

void TimeSeriesView::Init() {
}

bool TimeSeriesView::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style,
  const wxString& name )
{
  wxGrid::Create(parent, id, pos, size, style );
  CreateControls();
  return true;
}

void TimeSeriesView::CreateControls() {
  Bind( wxEVT_DESTROY, &TimeSeriesView::OnDestroy, this );
  HideRowLabels();
  //CreateGrid( 0, 7 );
}

void TimeSeriesView::OnDestroy( wxWindowDestroyEvent& event ) {
  assert( Unbind( wxEVT_DESTROY, &TimeSeriesView::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap TimeSeriesView::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TimeSeriesView::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
