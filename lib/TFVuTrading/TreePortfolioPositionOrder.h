/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013/07/25

#pragma once

#include <wx/treectrl.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class TreePortfolioPositionOrder: public wxTreeCtrl {
public:
  TreePortfolioPositionOrder(void);
  TreePortfolioPositionOrder(wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTreeCtrlNameStr);
  ~TreePortfolioPositionOrder(void);
protected:
private:
};

} // namespace tf
} // namespace ou
