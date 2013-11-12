/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// started 2013/11/11

#pragma once

#include <wx/dialog.h>
#include <wx/combobox.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_DIALOGINSTRUMENTSELECT_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGINSTRUMENTSELECT_TITLE _("DialogInstrumentSelect")
#define SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME ID_DIALOG_INSTRUMENTSELECT
#define SYMBOL_DIALOGINSTRUMENTSELECT_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGINSTRUMENTSELECT_POSITION wxDefaultPosition

class DialogInstrumentSelect: public wxDialog {
  DECLARE_DYNAMIC_CLASS( DialogInstrumentSelect )
public:

  struct DataExchange {
    bool bOk;
    wxString sSymbolName;
    DataExchange( void ): bOk( false ) {};
    // todo:  pass in sorted array of pre-existing instruments
  };

  DialogInstrumentSelect(void);
  DialogInstrumentSelect( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGINSTRUMENTSELECT_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGINSTRUMENTSELECT_POSITION, 
    const wxSize& size = SYMBOL_DIALOGINSTRUMENTSELECT_SIZE, 
    long style = SYMBOL_DIALOGINSTRUMENTSELECT_STYLE );
  
  ~DialogInstrumentSelect(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGINSTRUMENTSELECT_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGINSTRUMENTSELECT_POSITION, 
    const wxSize& size = SYMBOL_DIALOGINSTRUMENTSELECT_SIZE, 
    long style = SYMBOL_DIALOGINSTRUMENTSELECT_STYLE );

  void SetDataExchange( DataExchange* pde );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  typedef fastdelegate::FastDelegate1<DataExchange*,void> OnDoneHandler_t;
  void SetOnDoneHandler( OnDoneHandler_t function ) {
    m_OnDoneHandler = function;
  }

protected:

  void Init();
  void CreateControls();

private:
  enum { ID_Null=wxID_HIGHEST, ID_DIALOG_INSTRUMENTSELECT,
    ID_CBSymbol, ID_LblDescription, ID_BTNOK, ID_BTNCancel };

  OnDoneHandler_t m_OnDoneHandler;

  DataExchange* m_pDataExchange;

    wxComboBox* m_cbSymbol;
    wxStaticText* m_lblDescription;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;

  void OnClose( wxCloseEvent& event );
  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  };

} // namespace tf
} // namespace ou
