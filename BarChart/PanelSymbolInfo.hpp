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
 * File:    PanelSymbolInfo.hpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 27, 2025 12:47:19
 */

#pragma once

#include <wx/panel.h>

#define SYMBOL_PANEL_SymbolInfo_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_SymbolInfo_TITLE _("Panel Symbol Info")
#define SYMBOL_PANEL_SymbolInfo_IDNAME ID_PANEL_SymbolInfo
#define SYMBOL_PANEL_SymbolInfo_SIZE wxDefaultSize
#define SYMBOL_PANEL_SymbolInfo_POSITION wxDefaultPosition

class wxButton;
class wxListBox;
class wxTextCtrl;
class wxStaticText;

class PanelSymbolInfo: public wxPanel {
public:

PanelSymbolInfo();
PanelSymbolInfo(
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_SymbolInfo_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_SymbolInfo_POSITION,
    const wxSize& size = SYMBOL_PANEL_SymbolInfo_SIZE,
    long style = SYMBOL_PANEL_SymbolInfo_STYLE );
  virtual ~PanelSymbolInfo();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PANEL_SymbolInfo_IDNAME,
    const wxPoint& pos = SYMBOL_PANEL_SymbolInfo_POSITION,
    const wxSize& size = SYMBOL_PANEL_SymbolInfo_SIZE,
    long style = SYMBOL_PANEL_SymbolInfo_STYLE );

  void CreateControls();

  using fBtnSave_t = std::function<void( const std::string&)>;
  using fBtnUndo_t = std::function<std::string()>;

  struct Fields {
    std::string sYield;
    std::string sLast;
    std::string sAmount;
    std::string sRate;
    std::string sExDiv;
    std::string sPayed;
    std::string sNotes;
    std::string sName;

    fBtnSave_t fBtnSave;
    fBtnUndo_t fBtnUndo;

    Fields()
    : fBtnSave( nullptr ), fBtnUndo( nullptr )
    {}

    Fields( Fields&& rhs )
    : sYield(   std::move( rhs.sYield ) )
    , sLast(    std::move( rhs.sLast ) )
    , sAmount(  std::move( rhs.sAmount ) )
    , sRate(    std::move( rhs.sRate ) )
    , sExDiv(   std::move( rhs.sExDiv ) )
    , sPayed(   std::move( rhs.sPayed ) )
    , sNotes(   std::move( rhs.sNotes ) )
    , sName(    std::move( rhs.sName ) )
    , fBtnSave( std::move( rhs.fBtnSave ) )
    , fBtnUndo( std::move( rhs.fBtnUndo ) )
    {}

    Fields( const Fields& rhs )
    : sYield(   rhs.sYield )
    , sLast(    rhs.sLast )
    , sAmount(  rhs.sAmount )
    , sRate(    rhs.sRate )
    , sExDiv(   rhs.sExDiv )
    , sPayed(   rhs.sPayed )
    , sNotes(   rhs.sNotes )
    , sName(    rhs.sName )
    , fBtnSave( rhs.fBtnSave )
    , fBtnUndo( rhs.fBtnUndo )
    {}

  };
  void SetFields( const Fields& );
  void SetTags( const wxArrayString& );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_SymbolInfo
  , ID_STATIC_Yield, ID_TEXT_Yield
  , ID_STATIC_Last, ID_TEXT_Last
  , ID_STATIC_Amount, ID_TEXT_Amount
  , ID_STATIC_Rate, ID_TEXT_Rate
  , ID_STATIC_ExDiv, ID_TEXT_ExDiv
  , ID_STATIC_Payed, ID_TEXT_Payed
  , ID_TEXT_Notes, ID_STATIC_NAME
  , ID_BTN_Undo, ID_BTN_Save
  , ID_LB_TAGS
  };

  wxTextCtrl* m_txtYield;
  wxTextCtrl* m_txtLast;
  wxTextCtrl* m_txtAmount;
  wxTextCtrl* m_txtRate;
  wxTextCtrl* m_txtDateExDiv;
  wxTextCtrl* m_txtDatePayed;
  wxTextCtrl* m_txtNotes;
  wxButton* m_btnUndo;
  wxButton* m_btnSave;
  wxStaticText* m_txtName;
  wxListBox* m_lbTags;

  fBtnSave_t m_fBtnSave;
  fBtnUndo_t m_fBtnUndo;

  void Init();

  void OnBTNUndoClick( wxCommandEvent& event );
  void OnBTNSaveClick( wxCommandEvent& event );

  void OnClose( wxCloseEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};