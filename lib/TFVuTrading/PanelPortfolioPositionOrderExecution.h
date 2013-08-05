/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <OUCommon/Delegate.h>

#include "VuPortfolios.h"
#include "VuPositions.h"
#include "VuOrders.h"
#include "VuExecutions.h"
#include "VuTreePortfolioPositionOrder.h"

#include "ModelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELPPOE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPPOE_TITLE _("Portfolio Status")
#define SYMBOL_PANELPPOE_IDNAME ID_PANELPPOE
#define SYMBOL_PANELPPOE_SIZE wxSize(400, 300)
#define SYMBOL_PANELPPOE_POSITION wxDefaultPosition

class PanelPortfolioPositionOrderExecution: public wxPanel {
public:

  typedef ou::tf::ModelPortfolioPositionOrderExecution MPPOE_t;

  PanelPortfolioPositionOrderExecution( MPPOE_t* pMPPOE );
  PanelPortfolioPositionOrderExecution(
   MPPOE_t* pMPPOE,
   wxWindow* parent, 
   wxWindowID id = SYMBOL_PANELPPOE_IDNAME, 
   const wxPoint& pos = SYMBOL_PANELPPOE_POSITION, 
   const wxSize& size = SYMBOL_PANELPPOE_SIZE, 
   long style = SYMBOL_PANELPPOE_STYLE 
   );
  ~PanelPortfolioPositionOrderExecution(void);

  bool Create(
   wxWindow* parent, 
   wxWindowID id = SYMBOL_PANELPPOE_IDNAME, 
   const wxPoint& pos = SYMBOL_PANELPPOE_POSITION, 
   const wxSize& size = SYMBOL_PANELPPOE_SIZE, 
   long style = SYMBOL_PANELPPOE_STYLE
   );

  ou::Delegate<PanelPortfolioPositionOrderExecution*> NotifyClose;

  VuTreePortfolioPositionOrder* GetTree( void ) { return m_pDVPPOE; };

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_PANELPPOE
  };

  VuPortfolios* m_pDVPortfolios;
  VuPositions* m_pDVPositions;
  VuOrders* m_pDVOrders;
  VuExecutions* m_pDVExecutions;

  VuTreePortfolioPositionOrder* m_pDVPPOE;

  ou::tf::ModelPortfolioPositionOrderExecution* m_pMPPOE;

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };
  
  void OnFocusChange( wxFocusEvent& event );
  void OnClose( wxCloseEvent& event );


};

} // namespace tf
} // namespace ou
