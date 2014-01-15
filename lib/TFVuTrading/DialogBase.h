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
// started 2013/11/13

#pragma once

#include <wx/dialog.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame

class DialogBase: public wxDialog {
public:

  struct DataExchange {
    bool bOk;
    DataExchange( void ): bOk( false ) {};
    // todo:  pass in sorted array of pre-existing instruments
  };

  DialogBase(void);
//  DialogBase( wxWindow* parent, 
//    wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style );
  ~DialogBase(void);

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  virtual void SetDataExchange( DataExchange* pde );

  typedef fastdelegate::FastDelegate1<DataExchange*,void> OnDoneHandler_t;
  void SetOnDoneHandler( OnDoneHandler_t function ) {
    m_OnDoneHandler = function;
  }

protected:

  DataExchange* m_pDataExchange;

  OnDoneHandler_t m_OnDoneHandler;

  virtual bool Create( wxWindow* parent, 
    wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& sizeE, long style );

private:

  void Init( void );

  void OnCloseWindow( wxCloseEvent& event );
  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnCancelOrClose( void );

};


} // namespace tf
} // namespace ou
