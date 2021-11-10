/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    PanelTrade.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 15:55
 */

#include <iostream>

#include <wx/sizer.h>
#include <wx/window.h>

#include "PanelTrade.h"
#include "wx/gdicmn.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

PanelTrade::PanelTrade( void ) {
  Init();
};

PanelTrade::PanelTrade( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelTrade::~PanelTrade(void) {
  // test for open and then close?
}

void PanelTrade::Init( void ) {
  m_nRowCount = 0;
  m_nVisibleRows = 0;
  m_nFramedRows = 0;
  m_nCenteredRows = 0;
}

bool PanelTrade::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;

}

void PanelTrade::CreateControls( void ) {

  //PanelTrade* itemPanel1 = this;
  DrawRows();

}

void PanelTrade::DrawRows() {

  wxSize sizeClient = GetClientSize();

  assert( 0 == m_nRowCount );
  m_nRowCount = ( sizeClient.GetHeight() - 2 * BorderWidth ) / RowHeight;

  m_nVisibleRows = m_nRowCount - 1; // first row is header row
  m_nFramedRows = m_nVisibleRows / FramedRows;
  m_nCenteredRows = ( ( m_nVisibleRows - m_nFramedRows ) /2 ) - 1; // eliminates up/down jitter

  std::cout << sizeClient.GetHeight() << "," << m_nRowCount << "," << m_nFramedRows << "," << m_nCenteredRows << std::endl;

  int yOffset = BorderWidth; // start offset with border

  int ixRowElements = 0;
  m_vRowElements.resize( m_nRowCount );

  RowElements* pRow = new RowElements( this, wxPoint( BorderWidth, yOffset ), RowHeight, true );
  m_vRowElements[ ixRowElements ] = pRow;
  yOffset += RowHeight;
  ixRowElements++;

  while ( ixRowElements < m_nRowCount ) {
    RowElements* pRow = new RowElements( this, wxPoint( BorderWidth, yOffset ), RowHeight, false );
    m_vRowElements[ ixRowElements ] = pRow;
    yOffset += RowHeight;
    ixRowElements++;
  }

  // resize panel?
}

} // market depth
} // namespace tf
} // namespace ou
