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

#include <wx/combobox.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include "DialogBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_DIALOGINSTRUMENTSELECT_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGINSTRUMENTSELECT_TITLE _("Select An Instrument")
#define SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME ID_DIALOG_INSTRUMENTSELECT
#define SYMBOL_DIALOGINSTRUMENTSELECT_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGINSTRUMENTSELECT_POSITION wxDefaultPosition

class DialogInstrumentSelect: public DialogBase {
  DECLARE_DYNAMIC_CLASS( DialogInstrumentSelect )
public:

  typedef FastDelegate1<const std::string&,const std::string&> NameLookup_t; // in=name, out=description

  struct DataExchange: DialogBase::DataExchange {
    NameLookup_t lookup;
    wxString sSymbolName;
    DataExchange( void ): lookup( 0 ) {};
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

  virtual void SetDataExchange( DataExchange* pde );

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_DIALOG_INSTRUMENTSELECT,
    ID_CBSymbol, ID_LblDescription };

    wxComboBox* m_cbSymbol;
    wxStaticText* m_lblDescription;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;

  void HandleTextChange( wxCommandEvent& event );


  };

} // namespace tf
} // namespace ou
