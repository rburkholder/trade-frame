/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
 * Created on December 22, 2015, 3:04 PM
 */

#pragma once

#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
//#include <wx/srchctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#include <TFTrading/TradingEnumerations.h>

#include "DialogBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//#define SYMBOL_PANELPICKSYMBOL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_PANELPICKSYMBOL_STYLE wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_PANELPICKSYMBOL_TITLE _("Dialog Pick Symbol")
#define SYMBOL_PANELPICKSYMBOL_IDNAME ID_PANELPICKSYMBOL
#define SYMBOL_PANELPICKSYMBOL_SIZE wxSize(400, 300)
#define SYMBOL_PANELPICKSYMBOL_POSITION wxDefaultPosition

namespace DialogPickSymbol_local {

template<typename T>
struct FirstOrDefault {  // a combiner used for signals with return values
  typedef T result_type;
  template<typename InputIterator>
  T operator()( InputIterator first, InputIterator last ) const {
    if (first==last) return T();
    return *first;
  }
};

}

class DialogPickSymbol: public DialogBase {
public:
  
  typedef boost::signals2::signal<void (const std::string&, std::string&)> signalLookUp_t;
  typedef signalLookUp_t::slot_type slotLookUp_t;
  
  struct DataExchange: DialogBase::DataExchange {
    signalLookUp_t lookup; // // (1)in=name, (2)out=description
    wxString sUnderlyingSymbolName;  // can't be std::string, needs to handle native DataExchange
    std::string sCompositeName;
    std::string sComponsiteDescription;
    double dblStrike;
    InstrumentType::enumInstrumentTypes it;
    OptionSide::enumOptionSide os;
    std::uint16_t year;
    std::uint8_t month;
    std::uint8_t day;
    DataExchange( void ): DialogBase::DataExchange(), 
      dblStrike( 0.0 ), it( InstrumentType::Stock ), os( OptionSide::Put ) {}
  };
  
  DialogPickSymbol();
  DialogPickSymbol( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELPICKSYMBOL_IDNAME,
    const wxString& caption = SYMBOL_PANELPICKSYMBOL_TITLE, 
    const wxPoint& pos = SYMBOL_PANELPICKSYMBOL_POSITION, 
    const wxSize& size = SYMBOL_PANELPICKSYMBOL_SIZE, 
    long style = SYMBOL_PANELPICKSYMBOL_STYLE );
  virtual ~DialogPickSymbol();
  
  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELPICKSYMBOL_IDNAME, 
    const wxString& caption = SYMBOL_PANELPICKSYMBOL_TITLE, 
    const wxPoint& pos = SYMBOL_PANELPICKSYMBOL_POSITION, 
    const wxSize& size = SYMBOL_PANELPICKSYMBOL_SIZE, 
    long style = SYMBOL_PANELPICKSYMBOL_STYLE );
  
  virtual void SetDataExchange( DataExchange* pde );

  typedef boost::signals2::signal<bool (DataExchange*), DialogPickSymbol_local::FirstOrDefault<bool> > signalComposeComposite_t;
  typedef signalComposeComposite_t::slot_type slotComposeComposite_t;
  signalComposeComposite_t signalComposeComposite;
  
protected:
  
private:
  
  enum { ID_NULL=wxID_HIGHEST, ID_PANELPICKSYMBOL,
    ID_RADIO_EQUITY, ID_RADIO_OPTION, ID_RADIO_FUTURE, ID_RADIO_FOPTION,
    ID_TEXT_SYMBOL, ID_STATIC_SYMBOL_DESCRIPTION, 
    ID_TEXT_COMPOSITE, ID_STATIC_COMPOSITE_DESCRIPTION, 
    ID_DATE_EXPIRY, 
    ID_TEXT_STRIKE,
    ID_RADIO_PUT, ID_RADIO_CALL, 
    ID_RADIO_USD, ID_RADIO_CAD,
    ID_BTN_SAVE, ID_BTN_CANCEL
  };
  
   // might use a combo box, or custom widget to pop up symbol list as selections are refined
  
    wxRadioButton* m_radioEquity;
    wxRadioButton* m_radioOption;
    wxRadioButton* m_radioFuture;
    wxRadioButton* m_radioFOption;
    wxTextCtrl* m_textSymbol;
    wxStaticText* m_txtSymbolDescription;
    wxTextCtrl* m_textComposite;
    wxStaticText* m_txtCompositeDescription;
    wxDatePickerCtrl* m_dateExpiry;
    wxTextCtrl* m_textStrike;
    wxRadioButton* m_radioOptionPut;
    wxRadioButton* m_radioOptionCall;
    wxRadioButton* m_radioCurrencyUSD;
    wxRadioButton* m_radioCurrencyCAD;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;

  void Init( void );
  void CreateControls( void );
  
  void DisableOptionFields( void );
  
  void HandleRadioEquity( wxCommandEvent& event );
  void HandleRadioOption( wxCommandEvent& event );
  void HandleRadioFuture( wxCommandEvent& event );
  void HandleRadioFOption( wxCommandEvent& event );
  
  void HandleRadioPut( wxCommandEvent& event );
  void HandleRadioCall( wxCommandEvent& event );
  
  void HandleExpiryChanged( wxDateEvent& event );
  
  void HandleSymbolChange( wxCommandEvent& event );
  
  void UpdateComposite( void );
  
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips( void ) { return true; }

};

} // namespace tf
} // namespace ou
