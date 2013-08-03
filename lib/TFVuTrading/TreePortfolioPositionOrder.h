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

//#include <wx/treectrl.h>

#include "VuBase.h"

#include "ModelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class TreePortfolioPositionOrder: public VuBase {
public:
  TreePortfolioPositionOrder( ModelPortfolioPositionOrderExecution* );
  TreePortfolioPositionOrder( ModelPortfolioPositionOrderExecution*,
              wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDV_SINGLE | wxDV_NO_HEADER,
               const wxValidator& validator = wxDefaultValidator );
  ~TreePortfolioPositionOrder(void);
protected:
private:

  typedef ModelPortfolioPositionOrderExecution dvmdlPPOE_t;

  wxObjectDataPtr<dvmdlPPOE_t> m_pdvmdlPPOE;

  void Construct( void );

};

} // namespace tf
} // namespace ou
