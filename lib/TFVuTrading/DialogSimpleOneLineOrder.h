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

#include "DialogBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//#define SYMBOL_DIALOGORDER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGORDER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGORDER_TITLE _("Order Creation")
#define SYMBOL_DIALOGORDER_IDNAME ID_DIALOGORDER
#define SYMBOL_DIALOGORDER_SIZE wxSize(399, 299)
#define SYMBOL_DIALOGORDER_POSITION wxDefaultPosition

class DialogSimpleOneLineOrder: public DialogBase {
  DECLARE_DYNAMIC_CLASS( DialogSimpleOneLineOrder )
public:

  struct DataExchange: DialogBase::DataExchange {
    wxString sBuySell;
    unsigned int nQuantity;
    wxString sDayGtc;
    wxString sLmtMktStp;
    double dblPrice1;
  };

  DialogSimpleOneLineOrder(void);
  DialogSimpleOneLineOrder( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGORDER_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGORDER_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGORDER_POSITION, 
    const wxSize& size = SYMBOL_DIALOGORDER_SIZE, 
    long style = SYMBOL_DIALOGORDER_STYLE );
  ~DialogSimpleOneLineOrder(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGORDER_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGORDER_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGORDER_POSITION, 
    const wxSize& size = SYMBOL_DIALOGORDER_SIZE, 
    long style = SYMBOL_DIALOGORDER_STYLE );

  virtual void SetDataExchange( DataExchange* pde );

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_DIALOGORDER,
    ID_ChcBuySell, ID_TxtQuantity, ID_ChcDayGtc, ID_ChcLmtMktStp, ID_TxtPrice1 };

    wxChoice* m_choiceBuySell;
    wxTextCtrl* m_txtQuantity;
    wxChoice* m_choiceDayGtc;
    wxChoice* m_choiceLmtMktStp;
    wxTextCtrl* m_txtPrice1;
    wxButton* m_btnSubmit;

};

} // namespace tf
} // namespace ou

