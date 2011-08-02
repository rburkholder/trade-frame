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

#include "ThreadMain.h"

ThreadMain::ThreadMain(void) {

  std::string sDbName;

  switch ( m_eMode ) {
    case EModeSimulation:
      sDbName = ":memory:";
//      m_pExecutionProvider = m_sim;
//      m_pDataProvider = m_sim;
      break;
    case EModeLive:
      sDbName = "Hedge.db";
//      m_pExecutionProvider = m_tws;
//      m_pDataProvider = m_tws;
      break;
  }

  m_db.Open( sDbName );

}


ThreadMain::~ThreadMain(void) {
  m_db.Close();
}
