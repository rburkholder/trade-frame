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
 * File:    WinRowElement.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 16:29
 */

 // basic element containing dynamically updated text & values
 // a window per value in a depth of market view port

#pragma once

#include "wx/event.h"
#include <wx/window.h>

#include <OUCommon/Colour.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

#define SYMBOL_WINELEMENT_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WINELEMENT_TITLE _("Window Element")
#define SYMBOL_WINELEMENT_IDNAME ID_WINELEMENT
#define SYMBOL_WINELEMENT_SIZE wxSize(100, 20)
#define SYMBOL_WINELEMENT_POSITION wxDefaultPosition

class WinRowElement: public wxWindow {
public:

  WinRowElement();

  WinRowElement(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WINELEMENT_IDNAME,
    const wxPoint& pos = SYMBOL_WINELEMENT_POSITION,
    const wxSize& size = SYMBOL_WINELEMENT_SIZE,
    long style = SYMBOL_WINELEMENT_STYLE
    );

  virtual ~WinRowElement();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WINELEMENT_IDNAME,
    const wxPoint& pos = SYMBOL_WINELEMENT_POSITION,
    const wxSize& size = SYMBOL_WINELEMENT_SIZE,
    long style = SYMBOL_WINELEMENT_STYLE
    );

  void SetText( const std::string& );
  void SetCanHaveFocus( bool );
  void SetColourBackground( ou::Colour::enumColour ); // convert to wxColour
  void SetColourForeground( ou::Colour::enumColour ); // convert to wxColour

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_WINELEMENT
  };

  bool m_bCanHaveFocus;
  bool m_bFocusSet;
  std::string m_sText;

  ou::Colour::enumColour m_ColourBackground;
  ou::Colour::enumColour m_ColourForeground;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  void OnFocusSet( wxFocusEvent& );
  void OnFocusKill( wxFocusEvent& );
  void OnMouseLeftUp( wxMouseEvent& );
  void OnMouseEnterWindow( wxMouseEvent& );
  void OnMouseLLeaveWindow( wxMouseEvent& );
  void OnContextMenu( wxContextMenuEvent& );
  void OnPaint( wxPaintEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

};

} // market depth
} // namespace tf
} // namespace ou





