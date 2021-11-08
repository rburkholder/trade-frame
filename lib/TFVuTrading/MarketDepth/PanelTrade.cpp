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

#include <wx/sizer.h>

#include "WinElement.h"
#include "PanelTrade.h"

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
  PanelTrade* itemPanel1 = this;

  WinElement* element1 = new WinElement( this, wxID_ANY, wxPoint(  5,  5 ), wxSize( 100, 20 ) );
  WinElement* element2 = new WinElement( this, wxID_ANY, wxPoint(  5, 25 ), wxSize( 100, 20 ) );
}

} // market depth
} // namespace tf
} // namespace ou
