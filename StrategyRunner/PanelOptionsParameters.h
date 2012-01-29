/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#define SYMBOL_PANELOPTIONSPARAMETERS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELOPTIONSPARAMETERS_TITLE _("Options Parameters")
#define SYMBOL_PANELOPTIONSPARAMETERS_IDNAME ID_PANELOPTIONSPARAMETERS
#define SYMBOL_PANELOPTIONSPARAMETERS_SIZE wxSize(400, 300)
#define SYMBOL_PANELOPTIONSPARAMETERS_POSITION wxDefaultPosition

class PanelOptionsParameters: public wxPanel {
public:

  PanelOptionsParameters(void);
  PanelOptionsParameters(
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELOPTIONSPARAMETERS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELOPTIONSPARAMETERS_POSITION, 
    const wxSize& size = SYMBOL_PANELOPTIONSPARAMETERS_SIZE, 
    long style = SYMBOL_PANELOPTIONSPARAMETERS_STYLE );
  ~PanelOptionsParameters(void);

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELOPTIONSPARAMETERS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELOPTIONSPARAMETERS_POSITION, 
    const wxSize& size = SYMBOL_PANELOPTIONSPARAMETERS_SIZE, 
    long style = SYMBOL_PANELOPTIONSPARAMETERS_STYLE );
  
  void Init();
  void CreateControls();

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  typedef FastDelegate0<> OnStart_t;
  void SetOnStart( OnStart_t function ) {
    m_OnStart = function;
  }

  void SetBtnStartEnable( bool );

  std::string GetUnderlying( void );
  boost::gregorian::date GetOptionFarDate( void );
  boost::gregorian::date GetOptionNearDate( void );

  void SetOptionFarDate( boost::gregorian::date );
  void SetOptionNearDate( boost::gregorian::date );

protected:
private:
  enum { ID_Null=wxID_HIGHEST, ID_PANELOPTIONSPARAMETERS, 
    ID_LBL_Underlying, ID_TXT_Underlying, 
    ID_LBL_OPTIONNEARDATE, ID_DATE_NearDate,
    ID_LBL_OPTIONFARDATE, ID_DATE_FarDate,
    ID_BTN_START
  };

  wxTextCtrl* m_txtUnderlying;
  wxDatePickerCtrl* m_ctrlNearDate;
  wxDatePickerCtrl* m_ctrlFarDate;
  wxButton* m_btnStart;

  OnStart_t m_OnStart;

  void OnBtnStartClicked( wxCommandEvent& event );

};

