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

#include "FrameGrid.h"

// style: wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX
FrameGrid::FrameGrid(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
  Init();
  Create(parent, title, pos, size, style);
}

FrameGrid::~FrameGrid(void) {
}

void FrameGrid::Init( void ) {
}

bool FrameGrid::Create( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style) {
  wxFrame::Create( parent, wxID_ANY, title, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void FrameGrid::CreateControls( void ) {

  FrameGrid* itemFrame1 = this;

  wxGrid* itemGrid2 = new wxGrid( itemFrame1, wxID_ANY, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
  itemGrid2->SetDefaultColSize(50);
  itemGrid2->SetDefaultRowSize(25);
  itemGrid2->SetColLabelSize(25);
  itemGrid2->SetRowLabelSize(50);
  itemGrid2->CreateGrid(5, 5, wxGrid::wxGridSelectCells);

}

void FrameGrid::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

