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

#include "FrameInstrumentStatus.h"

// style: wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX
FrameInstrumentStatus::FrameInstrumentStatus(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
  Init();
  Create(parent, title, pos, size, style);
}

FrameInstrumentStatus::~FrameInstrumentStatus(void) {
}

void FrameInstrumentStatus::Init( void ) {
}

bool FrameInstrumentStatus::Create( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style) {
  wxFrame::Create( parent, wxID_ANY, title, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void FrameInstrumentStatus::CreateControls( void ) {

  FrameInstrumentStatus* itemFrame1 = this;

  m_gridStatus = new wxGrid( itemFrame1, wxID_ANY, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxRESIZE_BORDER );

  m_gridStatus->SetDefaultColSize(50);
  m_gridStatus->SetDefaultRowSize(25);
  m_gridStatus->SetColLabelSize(25);
  m_gridStatus->SetRowLabelSize(100);
  m_gridStatus->CreateGrid(0, 5, wxGrid::wxGridSelectCells);

  m_gridStatus->SetColLabelValue( 0, "Low" );
  m_gridStatus->SetColLabelValue( 1, "Price" );
  m_gridStatus->SetColLabelValue( 2, "High" );
  m_gridStatus->SetColLabelValue( 3, "ROC" );
  m_gridStatus->SetColLabelValue( 4, "%D" );

}

void FrameInstrumentStatus::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

