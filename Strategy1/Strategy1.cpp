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

// Strategy1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "Strategy1.h"

IMPLEMENT_APP(AppStrategy1)

bool AppStrategy1::OnInit() {
  m_pStrategy = new Strategy;
  m_pStrategy->Start( "" );
  return 1;
}

int AppStrategy1::OnExit() {
  delete m_pStrategy;
  return 0;
}