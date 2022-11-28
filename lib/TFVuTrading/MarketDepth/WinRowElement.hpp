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

#include <functional>

#include <wx/window.h>

#include <OUCommon/Colour.h>

#include <TFVuTrading/Mouse.hpp>

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
    wxWindowID id = SYMBOL_WINELEMENT_ID,
    const wxPoint& pos = SYMBOL_WINELEMENT_POSITION,
    const wxSize& size = SYMBOL_WINELEMENT_SIZE,
    long style = SYMBOL_WINELEMENT_STYLE
    );

  virtual ~WinRowElement();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WINELEMENT_ID,
    const wxPoint& pos = SYMBOL_WINELEMENT_POSITION,
    const wxSize& size = SYMBOL_WINELEMENT_SIZE,
    long style = SYMBOL_WINELEMENT_STYLE
    );

  using EButton = ou::tf::Mouse::EButton;
  using EColour = ou::Colour::wx::EColour;

  using fClick_t = std::function<void(EButton,bool,bool,bool)>;

  void SetText( const std::string& );
  void SetText( const std::string&, bool bHighlight );
  void SetCanHaveFocus( bool );

  void SetColourBackground( EColour colour );
  void SetColourForeground( EColour colour );
  void SetColourHighlight( EColour colour );
  void SetColours( EColour colourB, EColour colourF, EColour colourH );

  void Set( fClick_t&& );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, SYMBOL_WINELEMENT_ID
  };

  bool m_bCanHaveFocus;
  bool m_bFocusSet;
  bool m_bHighlight;
  std::string m_sText;

  EColour m_colourBackground;
  EColour m_colourForeground;
  EColour m_colourHighlight;

  long m_style;  // wxCENTER, wxLEFT, wxRIGHT

  fClick_t m_fClick;

  void Init();
  void CreateControls();

  void Paint();
  void Render( wxDC& );

  void OnFocusSet( wxFocusEvent& );
  void OnFocusKill( wxFocusEvent& );

  void OnMouseLeftUp( wxMouseEvent& );
  void OnMouseMiddleUp( wxMouseEvent& );
  void OnMouseRightUp( wxMouseEvent& );

  void OnMouseEnterWindow( wxMouseEvent& );
  void OnMouseLLeaveWindow( wxMouseEvent& );

  void OnContextMenu( wxContextMenuEvent& );

  void OnPaint( wxPaintEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

};

} // market depth
} // namespace tf
} // namespace ou
