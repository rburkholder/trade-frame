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

#include "Strategy.h"

class AppStrategy1:
  public wxApp
{
public:
protected:
private:

  Strategy* m_pStrategy;

  virtual bool OnInit();
  virtual int OnExit();
};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppStrategy1)

