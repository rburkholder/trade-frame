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

//#define SYMBOL_DIALOGNEWPORTFOLIO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGNEWPORTFOLIO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGNEWPORTFOLIO_TITLE _("Add New Portfolio")
#define SYMBOL_DIALOGNEWPORTFOLIO_IDNAME ID_DIALOGNEWPORTFOLIO
#define SYMBOL_DIALOGNEWPORTFOLIO_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGNEWPORTFOLIO_POSITION wxDefaultPosition

class DialogNewPortfolio: public DialogBase {
  DECLARE_DYNAMIC_CLASS( DialogNewPortfolio )
public:

  struct DataExchange: DialogBase::DataExchange {
    wxString sPortfolioId;
    wxString sDescription;
    // todo:  pass in sorted array of pre-existing instruments
  };

  DialogNewPortfolio(void);
  DialogNewPortfolio( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGNEWPORTFOLIO_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGNEWPORTFOLIO_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGNEWPORTFOLIO_POSITION, 
    const wxSize& size = SYMBOL_DIALOGNEWPORTFOLIO_SIZE, 
    long style = SYMBOL_DIALOGNEWPORTFOLIO_STYLE );
  ~DialogNewPortfolio(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_DIALOGNEWPORTFOLIO_IDNAME, 
    const wxString& caption = SYMBOL_DIALOGNEWPORTFOLIO_TITLE, 
    const wxPoint& pos = SYMBOL_DIALOGNEWPORTFOLIO_POSITION, 
    const wxSize& size = SYMBOL_DIALOGNEWPORTFOLIO_SIZE, 
    long style = SYMBOL_DIALOGNEWPORTFOLIO_STYLE );

  virtual void SetDataExchange( DataExchange* pde );

protected:

  void Init();
  void CreateControls();

private:
  enum { ID_Null=wxID_HIGHEST, ID_DIALOGNEWPORTFOLIO,
    ID_LblPortfolioId, ID_TxtPortfolioId, ID_LblDescription, ID_TxtDescription };

    wxTextCtrl* m_txtPortfolioId;
    wxTextCtrl* m_txtDescription;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;
};

} // namespace tf
} // namespace ou
