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

#include "FramePortfolioPositionOrderExec.h"

#include "AppHedge.h"

IMPLEMENT_APP(AppHedge)

bool AppHedge::OnInit() {

  wxFrame *frame = new FramePortfolioPositionOrderExec("Hedge", wxPoint(50,50), wxSize(600,900));
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

int AppHedge::OnExit() {
  return 0;
}
 